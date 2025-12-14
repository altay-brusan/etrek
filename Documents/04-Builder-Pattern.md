# Builder Pattern in Etrek

## Overview

Etrek implements a **template-based, hierarchical Builder pattern** for constructing UI component pairs (Widget + Delegate). This pattern separates construction complexity from runtime behavior, provides clear dependency injection, and enables lazy component creation.

## Core Interface

### IWidgetDelegateBuilder Template

**Location**: `Common/Include/IWidgetDelegateBuilder.h`

All builders inherit from this template interface:

```cpp
template<typename WidgetType, typename DelegateType>
class IWidgetDelegateBuilder {
public:
    /**
     * @brief Build the widget and its delegate.
     *
     * @param params Parameters for delegate configuration (database connection, context)
     * @param parentWidget Optional parent widget (Qt parent for lifetime management)
     * @param parentDelegate Optional parent delegate (QObject parent for lifetime management)
     * @return std::pair<WidgetType*, DelegateType*> The created widget and delegate pointers
     */
    virtual std::pair<WidgetType*, DelegateType*>
        build(const DelegateParameter& params,
              QWidget* parentWidget = nullptr,
              QObject* parentDelegate = nullptr) = 0;

    virtual ~IWidgetDelegateBuilder() = default;
};
```

**Key Design Decisions**:

1. **Template-based**: Type safety and flexibility
2. **Returns `std::pair`**: Both widget and delegate in single call
3. **Raw pointers**: Leverage Qt's parent-child lifetime management
4. **DelegateParameter**: Centralized dependency injection container

## Dependency Injection: DelegateParameter

**Location**: `Common/Include/DelegateParameter.h`

The central dependency injection container:

```cpp
struct DelegateParameter {
    // Database connection for builders to create repositories
    std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> dbConnection;

    // Map of delegate names to instances for inter-delegate dependencies
    QMap<QString, QWeakPointer<IDelegate>> delegates;

    // Context manager for accessing session and workflow contexts
    std::weak_ptr<Etrek::Context::IContextManager> contextManager;

    // Pre-fetched session context (optional, for convenience)
    std::shared_ptr<Etrek::Context::ISessionContext> sessionContext;

    // Pre-fetched workflow context (optional)
    std::shared_ptr<Etrek::Context::IWorkflowContext> workflowContext;
};
```

**Architectural Pattern**:

- **DelegateParameter contains ONLY `dbConnection`**, NOT repositories
- Builders receive `DelegateParameter` and create repositories from `dbConnection`
- Builders pass repository instances to delegates
- Delegates NEVER receive `dbConnection`, only repositories
- Each builder independently creates its required repositories

## All Builder Classes

Etrek has **17 builder classes** across different modules:

### Application Module (4 builders)

| Builder | Widget | Delegate | Purpose |
|---------|--------|----------|---------|
| MainWindowBuilder | MainWindow | MainWindowDelegate | Main application window; page orchestration |
| SystemSettingPageBuilder | SystemSettingPage | SystemSettingPageDelegate | Composite settings page with 12 sub-configurations |
| ViewSelectionDialogBuilder | ViewSelectionDialog | ViewSelectionDialogDelegate | Procedure/view selection for examinations |
| WorkListPageBuilder | WorkListPage | WorkListPageDelegate | DICOM worklist display and management |

### Device Module (6 builders)

| Builder | Widget | Delegate | Dependencies |
|---------|--------|----------|--------------|
| ConnectionSetupBuilder | ConnectionSetupWidget | ConnectionSetupDelegate | None (UI only) |
| WorkflowConfigurationBuilder | WorkflowConfigurationWidget | WorkflowConfigurationDelegate | DeviceRepository |
| GeneratorConfigurationBuilder | GeneratorConfigurationWidget | GeneratorConfigurationDelegate | DeviceRepository |
| DetectorConfigurationBuilder | DetectorConfigurationWidget | DetectorConfigurationDelegate | DeviceRepository |
| CollimatorConfigurationBuilder | CollimatorConfigurationWidget | CollimatorConfigurationDelegate | DeviceRepository |
| DapConfigurationBuilder | DapConfigurationWidget | DapConfigurationDelegate | DeviceRepository |

### Worklist Module (1 builder)

| Builder | Widget | Delegate | Dependencies |
|---------|--------|----------|--------------|
| WorkListConfigurationBuilder | WorkListConfigurationWidget | WorkListConfigurationDelegate | WorklistRepository |

### PACS Module (1 builder)

| Builder | Widget | Delegate | Dependencies |
|---------|--------|----------|--------------|
| PacsEntityConfigurationBuilder | PacsEntityConfigurationWidget | PacsEntityConfigurationDelegate | PacsNodeRepository |

### ScanProtocol Module (3 builders)

| Builder | Widget | Delegate | Dependencies |
|---------|--------|----------|--------------|
| ProcedureConfigurationBuilder | ProcedureConfigurationWidget | ProcedureConfigurationDelegate | ScanProtocolRepository |
| ViewConfigurationBuilder | ViewConfigurationWidget | ViewConfigurationDelegate | ScanProtocolRepository |
| TechniqueConfigurationBuilder | TechniqueConfigurationWidget | TechniqueConfigurationDelegate | ScanProtocolRepository |

### Dicom Module (1 builder)

| Builder | Widget | Delegate | Dependencies |
|---------|--------|----------|--------------|
| ImageCommentConfigurationBuilder | ImageCommentConfigurationWidget | ImageCommentConfigurationDelegate | ImageCommentRepository |

### Device Connectivity (1 builder)

| Builder | Widget | Delegate | Dependencies |
|---------|--------|----------|--------------|
| ConnectionSettingBuilder | ConnectionSettingWidget | QObject (no delegate) | None |

## Builder Construction Patterns

### Pattern 1: Simple UI-Only Builder (No Repository)

For widgets that don't require data persistence:

**Example: ConnectionSetupBuilder**

```cpp
std::pair<ConnectionSetupWidget*, ConnectionSetupDelegate*>
ConnectionSetupBuilder::build(
    const DelegateParameter& params,
    QWidget* parentWidget,
    QObject* parentDelegate)
{
    // Step 1: Create widget
    auto widget = new ConnectionSetupWidget(parentWidget);

    // Step 2: Create delegate
    auto delegate = new ConnectionSetupDelegate(widget, parentDelegate);

    // Step 3: Wire signals (internal to delegate constructor)

    return { widget, delegate };
}
```

### Pattern 2: Repository-Based Read-Only Builder

For widgets that display data but don't modify it:

**Example: ImageCommentConfigurationBuilder**

```cpp
std::pair<ImageCommentConfigurationWidget*, ImageCommentConfigurationDelegate*>
ImageCommentConfigurationBuilder::build(
    const DelegateParameter& params,
    QWidget* parentWidget,
    QObject* parentDelegate)
{
    // Step 1: Create repository from dbConnection
    auto repository = std::make_shared<ImageCommentRepository>(params.dbConnection);

    // Step 2: Fetch initial data
    auto commentsResult = repository->getAllComments();

    // Step 3: Create widget with data
    auto widget = new ImageCommentConfigurationWidget(
        commentsResult.value,
        parentWidget
    );

    // Step 4: Create delegate WITHOUT repository (read-only)
    auto delegate = new ImageCommentConfigurationDelegate(widget, parentDelegate);

    return { widget, delegate };
}
```

### Pattern 3: Repository-Based CRUD Builder

For widgets that require full CRUD operations:

**Example: GeneratorConfigurationBuilder**

```cpp
std::pair<GeneratorConfigurationWidget*, GeneratorConfigurationDelegate*>
GeneratorConfigurationBuilder::build(
    const DelegateParameter& params,
    QWidget* parentWidget,
    QObject* parentDelegate)
{
    // Step 1: Create repository
    auto repository = std::make_shared<DeviceRepository>(params.dbConnection, nullptr);

    // Step 2: Fetch initial data
    auto generatorsResult = repository->getGeneratorList();
    auto tubesResult = repository->getXRayTubesList();

    // Step 3: Create widget with data
    auto widget = new GeneratorConfigurationWidget(
        generatorsResult.value,
        tubesResult.value,
        parentWidget
    );

    // Step 4: Create delegate WITH repository (needs CRUD access)
    auto delegate = new GeneratorConfigurationDelegate(
        widget,
        repository,  // Repository injected for CRUD operations
        parentDelegate
    );

    return { widget, delegate };
}
```

### Pattern 4: Multi-Repository Builder

For complex components requiring multiple data sources:

**Example: WorkListPageBuilder**

```cpp
std::pair<WorkListPage*, WorkListPageDelegate*>
WorkListPageBuilder::build(
    const DelegateParameter& params,
    QWidget* parentWidget,
    QObject* parentDelegate)
{
    // Step 1: Create multiple repositories
    auto worklistRepository =
        std::make_shared<WorklistRepository>(params.dbConnection);
    auto scanRepository =
        std::make_shared<ScanProtocolRepository>(params.dbConnection, nullptr);
    auto dicomRepository =
        std::make_shared<DicomRepository>(params.dbConnection);
    auto dicomTagRepository =
        std::make_shared<DicomTagRepository>(params.dbConnection);

    // Step 2: Create interface pointer for view
    auto irepository = std::static_pointer_cast<IWorklistRepository>(worklistRepository);

    // Step 3: Create widget
    auto widget = new WorkListPage(irepository, parentWidget);

    // Step 4: Create delegate with ALL repositories
    auto delegate = new WorkListPageDelegate(
        widget,
        worklistRepository,     // Concrete implementation
        scanRepository,
        dicomRepository,
        dicomTagRepository,
        params.dbConnection,
        params.contextManager,
        parentDelegate
    );

    return { widget, delegate };
}
```

## Composite Builder Pattern: SystemSettingPageBuilder

The most complex builder demonstrates hierarchical composition:

**Location**: `Application/Builder/SystemSettingPageBuilder.cpp`

### Build Process

```cpp
std::pair<SystemSettingPage*, SystemSettingPageDelegate*>
SystemSettingPageBuilder::build(
    const DelegateParameter& params,
    QWidget* parentWidget,
    QObject* parentDelegate)
{
    // Step 1: Create parent page and delegate
    auto page = new SystemSettingPage(parentWidget);
    auto systemSettingPageDelegate = new SystemSettingPageDelegate(page, parentDelegate);

    // Step 2: Prepare delegation parameters
    DelegateParameter delegateParameters;
    delegateParameters.dbConnection = params.dbConnection;
    delegateParameters.contextManager = params.contextManager;
    delegateParameters.sessionContext = params.sessionContext;
    delegateParameters.workflowContext = params.workflowContext;

    // Step 3: Build 12 sub-widgets using specialized builders
    // Each sub-builder creates its own repositories

    // Device Configuration Builders
    WorkflowConfigurationBuilder workflowBuilder;
    auto [workflowWidget, workflowDelegate] =
        workflowBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setWorkflowConfigurationWidget(workflowWidget);

    GeneratorConfigurationBuilder generatorBuilder;
    auto [generatorWidget, generatorDelegate] =
        generatorBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setGeneratorConfigurationWidget(generatorWidget);

    DetectorConfigurationBuilder detectorBuilder;
    auto [detectorWidget, detectorDelegate] =
        detectorBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setDetectorConfigurationWidget(detectorWidget);

    CollimatorConfigurationBuilder collimatorBuilder;
    auto [collimatorWidget, collimatorDelegate] =
        collimatorBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setCollimatorConfigurationWidget(collimatorWidget);

    DapConfigurationBuilder dapBuilder;
    auto [dapWidget, dapDelegate] =
        dapBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setDapConfigurationWidget(dapWidget);

    // Scan Protocol Builders
    ProcedureConfigurationBuilder procedureBuilder;
    auto [procedureWidget, procedureDelegate] =
        procedureBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setProcedureConfigurationWidget(procedureWidget);

    ViewConfigurationBuilder viewBuilder;
    auto [viewWidget, viewDelegate] =
        viewBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setViewConfigurationWidget(viewWidget);

    TechniqueConfigurationBuilder techniqueBuilder;
    auto [techniqueWidget, techniqueDelegate] =
        techniqueBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setTechniqueConfigurationWidget(techniqueWidget);

    // Integration Builders
    WorkListConfigurationBuilder worklistBuilder;
    auto [worklistWidget, worklistDelegate] =
        worklistBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setWorkListConfigurationWidget(worklistWidget);

    PacsEntityConfigurationBuilder pacsBuilder;
    auto [pacsWidget, pacsDelegate] =
        pacsBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setPacsEntityConfigurationWidget(pacsWidget);

    ConnectionSetupBuilder connectionBuilder;
    auto [connectionWidget, connectionDelegate] =
        connectionBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setConnectionSetupWidget(connectionWidget);

    ImageCommentConfigurationBuilder commentBuilder;
    auto [commentWidget, commentDelegate] =
        commentBuilder.build(delegateParameters, page, systemSettingPageDelegate);
    page->setImageCommentConfigurationWidget(commentWidget);

    // Step 4: Collect all child delegates
    QVector<QObject*> delegates = {
        workflowDelegate,
        generatorDelegate,
        detectorDelegate,
        collimatorDelegate,
        dapDelegate,
        procedureDelegate,
        viewDelegate,
        techniqueDelegate,
        worklistDelegate,
        pacsDelegate,
        connectionDelegate,
        commentDelegate
    };

    // Step 5: Attach child delegates to parent
    systemSettingPageDelegate->attachDelegates(delegates);

    return { page, systemSettingPageDelegate };
}
```

### Hierarchical Structure

```
SystemSettingPageBuilder
├─ Creates SystemSettingPage + SystemSettingPageDelegate
├─ Invokes 12 sub-builders:
│   ├─ WorkflowConfigurationBuilder
│   │   └─ Creates DeviceRepository
│   ├─ GeneratorConfigurationBuilder
│   │   └─ Creates DeviceRepository + fetches generators/tubes
│   ├─ DetectorConfigurationBuilder
│   │   └─ Creates DeviceRepository + fetches detectors
│   ├─ CollimatorConfigurationBuilder
│   │   └─ Creates DeviceRepository
│   ├─ DapConfigurationBuilder
│   │   └─ Creates DeviceRepository
│   ├─ ProcedureConfigurationBuilder
│   │   └─ Creates ScanProtocolRepository + fetches procedures
│   ├─ ViewConfigurationBuilder
│   │   └─ Creates ScanProtocolRepository + fetches views
│   ├─ TechniqueConfigurationBuilder
│   │   └─ Creates ScanProtocolRepository + fetches techniques
│   ├─ WorkListConfigurationBuilder
│   │   └─ Creates WorklistRepository
│   ├─ PacsEntityConfigurationBuilder
│   │   └─ Creates PacsNodeRepository
│   ├─ ConnectionSetupBuilder
│   │   └─ No repository
│   └─ ImageCommentConfigurationBuilder
│       └─ Creates ImageCommentRepository + fetches comments
└─ Injects all child delegates into parent delegate
```

## Signal and Slot Wiring

### Builder Responsibility

Builders are responsible for wiring signals and slots. This typically happens in the delegate constructor:

**Example: MainWindowDelegate**

```cpp
MainWindowDelegate::MainWindowDelegate(
    const DelegateParameter& params,
    MainWindow* widget,
    QObject* parent)
    : QObject(parent), m_params(params), m_mainWindow(widget)
{
    // Wire MainWindow signals to delegate slots
    connect(m_mainWindow, &MainWindow::LoadSystemPageAction,
            this, &MainWindowDelegate::onLoadSystemPageAction);
    connect(m_mainWindow, &MainWindow::LoadWorklistPageAction,
            this, &MainWindowDelegate::onLoadWorklistPageAction);
    connect(m_mainWindow, &MainWindow::aboutToClose,
            this, &MainWindowDelegate::aboutToClose);
}
```

### Cross-Delegate Signal Wiring

Builders also wire cross-delegate signals after construction:

**Example: MainWindowBuilder**

```cpp
std::pair<MainWindow*, MainWindowDelegate*>
MainWindowBuilder::build(
    const DelegateParameter& params,
    QWidget* parentWidget,
    QObject* parentDelegate)
{
    auto widget = new MainWindow(parentWidget);
    auto delegate = new MainWindowDelegate(params, widget, parentDelegate);

    // Wire delegate-to-widget signals
    connect(delegate, &MainWindowDelegate::applicationClosed,
            widget, &MainWindow::close);

    return { widget, delegate };
}
```

## Lazy Construction Pattern

Pages are built on-demand when user navigates to them, not at application startup:

**Example: MainWindowDelegate Lazy Page Loading**

```cpp
void MainWindowDelegate::onLoadSystemPageAction()
{
    // Clean up existing page delegate
    if (m_systemSettingPageDelegate) {
        m_systemSettingPageDelegate->deleteLater();
        m_systemSettingPageDelegate = nullptr;
    }

    // Build page on-demand
    SystemSettingPageBuilder builder;
    auto [page, delegate] = builder.build(m_params, nullptr, this);
    m_systemSettingPageDelegate = delegate;

    // Wire page delegate signals
    connect(delegate, &SystemSettingPageDelegate::closeSettings,
            this, [page, this]() {
                if (m_mainWindow) {
                    m_mainWindow->closePage();
                }
            });

    // Display page
    m_mainWindow->loadPage(page);
    m_mainWindow->finishLoadingPage();
}
```

**Benefits**:

1. **Reduced startup time**: Only MainWindow is built initially
2. **Lower memory footprint**: Pages created only when needed
3. **Dynamic replacement**: Old pages can be destroyed and rebuilt
4. **Resource optimization**: Database connections created per-page lifecycle

## Dependency Injection Flow

### Full Application Startup

```
ApplicationService::loadMainWindow()
    ↓
1. Create DelegateParameter:
   - dbConnection (from SettingProvider)
   - contextManager (created by ApplicationService)
   - sessionContext (set after authentication)
    ↓
2. Instantiate MainWindowBuilder
    ↓
3. Call builder.build(params)
    ↓
    MainWindowBuilder::build()
        ↓
        1. new MainWindow(parentWidget)
        2. new MainWindowDelegate(params, mainWindow, parentDelegate)
        3. Connect signals/slots
        4. return {mainWindow, delegate}
    ↓
4. Store result:
   - m_mainWindow = result.first
   - m_mainWindowDelegate = result.second
    ↓
5. Display: m_mainWindow->show()
```

### Lazy Page Construction (User Action)

```
User clicks "System Settings" button
    ↓
MainWindow::LoadSystemPageAction() signal
    ↓
MainWindowDelegate::onLoadSystemPageAction() slot
    ↓
1. Create DelegateParameter (same as main window)
2. Instantiate SystemSettingPageBuilder
3. Call builder.build(params)
    ↓
    SystemSettingPageBuilder::build()
        ↓
        1. new SystemSettingPage()
        2. new SystemSettingPageDelegate()
        3. FOR EACH of 12 sub-builders:
           a. Create sub-builder
           b. Call sub-builder.build(params, page, systemSettingPageDelegate)
              - Sub-builder creates its own repositories from params.dbConnection
           c. Add widget to page
           d. Collect delegate
        4. Call systemSettingPageDelegate->attachDelegates(allSubDelegates)
        5. return {page, pageDelegate}
    ↓
4. Wire signal: pageDelegate.closeSettings → mainWindow.closePage()
5. Display: mainWindow->loadPage(page)
```

## Builder Patterns and Best Practices

### Pattern 1: Repository Creation Responsibility

**Builders create repositories, NOT delegates**:

```cpp
// CORRECT: Builder creates repository
auto repository = std::make_shared<DeviceRepository>(params.dbConnection, nullptr);
auto delegate = new GeneratorConfigurationDelegate(widget, repository, parentDelegate);

// INCORRECT: Delegate creates repository
// auto delegate = new GeneratorConfigurationDelegate(widget, params.dbConnection, parentDelegate);
```

### Pattern 2: Stateless Builders

Builders have no state and can be instantiated per-use:

```cpp
// Create builder on-demand
SystemSettingPageBuilder builder;
auto result = builder.build(params);
// Builder can be destroyed immediately after build()
```

### Pattern 3: Qt Ownership Model

Use raw pointers with Qt parent-child mechanism:

```cpp
// Widget parent = UI container
auto widget = new WorkListPage(parentWidget);

// Delegate parent = higher-level delegate or ApplicationService
auto delegate = new WorkListPageDelegate(widget, ..., parentDelegate);

// Qt automatically destroys both when parents are destroyed
```

### Pattern 4: Smart Pointers for Repositories

Repositories use shared pointers for proper lifetime management:

```cpp
// Shared pointer ensures cleanup when builder scope ends
auto repository = std::make_shared<WorklistRepository>(params.dbConnection);

// Multiple repositories can be passed to single delegate
auto delegate = new WorkListPageDelegate(
    widget,
    repository1,
    repository2,
    repository3,
    ...
);
```

### Pattern 5: Interface-Based View Construction

Views receive interface pointers, delegates receive concrete implementations:

```cpp
// View receives interface for flexibility
auto irepository = std::static_pointer_cast<IWorklistRepository>(worklistRepository);
auto widget = new WorkListPage(irepository, parentWidget);

// Delegate receives concrete implementation for full access
auto delegate = new WorkListPageDelegate(
    widget,
    worklistRepository,  // Concrete implementation
    ...
);
```

## Complete Example: WorkListPageBuilder

### Builder Header (WorkListPageBuilder.h)

```cpp
#ifndef WORKLISTPAGEBUILDER_H
#define WORKLISTPAGEBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "WorkListPageDelegate.h"
#include "WorkListPage.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Application::Delegate {

    class WorkListPageBuilder
        : public IWidgetDelegateBuilder<WorkListPage, WorkListPageDelegate>
    {
    public:
        WorkListPageBuilder();
        ~WorkListPageBuilder();

        std::pair<WorkListPage*, WorkListPageDelegate*>
            build(const DelegateParameter& params,
                  QWidget* parentWidget = nullptr,
                  QObject* parentDelegate = nullptr) override;
    };
}

#endif // WORKLISTPAGEBUILDER_H
```

### Builder Implementation (WorkListPageBuilder.cpp)

```cpp
#include "WorkListPageBuilder.h"
#include "WorklistRepository.h"
#include "ScanProtocolRepository.h"
#include "DicomRepository.h"
#include "DicomTagRepository.h"

namespace Etrek::Application::Delegate {

WorkListPageBuilder::WorkListPageBuilder() = default;
WorkListPageBuilder::~WorkListPageBuilder() = default;

std::pair<WorkListPage*, WorkListPageDelegate*>
WorkListPageBuilder::build(
    const DelegateParameter& params,
    QWidget* parentWidget,
    QObject* parentDelegate)
{
    // Step 1: Create multiple repositories from dbConnection
    auto worklistRepository =
        std::make_shared<Etrek::Worklist::Repository::WorklistRepository>(
            params.dbConnection
        );

    auto scanRepository =
        std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(
            params.dbConnection,
            nullptr
        );

    auto dicomRepository =
        std::make_shared<Etrek::Dicom::Repository::DicomRepository>(
            params.dbConnection
        );

    auto dicomTagRepository =
        std::make_shared<Etrek::Dicom::Repository::DicomTagRepository>(
            params.dbConnection
        );

    // Step 2: Create interface pointer for view
    auto irepository = std::static_pointer_cast<
        Etrek::Worklist::Repository::IWorklistRepository
    >(worklistRepository);

    // Step 3: Create widget with interface
    auto widget = new WorkListPage(irepository, parentWidget);

    // Step 4: Create delegate with all concrete repositories
    auto delegate = new WorkListPageDelegate(
        widget,
        worklistRepository,
        scanRepository,
        dicomRepository,
        dicomTagRepository,
        params.dbConnection,
        params.contextManager,
        parentDelegate
    );

    // Signals are wired in delegate constructor

    return { widget, delegate };
}

} // namespace Etrek::Application::Delegate
```

## Builder Lifecycle and Memory Management

### Creation and Destruction

```cpp
// 1. Builder created on stack (automatic storage)
SystemSettingPageBuilder builder;

// 2. build() called, creates widget and delegate
auto [page, delegate] = builder.build(params, parentWidget, parentDelegate);

// 3. Builder goes out of scope and is destroyed
//    (Widget and delegate remain alive due to Qt parent ownership)

// 4. When parent is destroyed, Qt destroys children automatically
//    No manual delete needed
```

### Repository Lifetime

```cpp
// Repository created with shared_ptr
auto repository = std::make_shared<WorklistRepository>(params.dbConnection);

// Repository injected into delegate
auto delegate = new WorkListPageDelegate(widget, repository, ...);

// Repository's lifetime:
// - Kept alive by shared_ptr in delegate
// - Destroyed when delegate is destroyed
// - Automatic cleanup, no manual delete needed
```

## Summary

The Builder pattern in Etrek provides:

1. **Template-based standardization**: All builders follow `IWidgetDelegateBuilder<Widget, Delegate>`
2. **Centralized dependency injection**: `DelegateParameter` carries all dependencies
3. **Repository creation responsibility**: Builders create repositories, delegates receive them
4. **Lazy construction**: Components built on-demand, not at startup
5. **Hierarchical composition**: Parent builders orchestrate child builders (SystemSettingPageBuilder)
6. **Qt integration**: Leverages parent-child ownership for automatic memory management
7. **Stateless builders**: No builder state, can be instantiated per-use
8. **17 builder classes**: Across Application, Device, Worklist, PACS, ScanProtocol, and Dicom modules

This architecture enables:

- **Testability**: Mock repositories can be injected
- **Maintainability**: Clear construction logic separate from runtime behavior
- **Scalability**: New builders easily added following established pattern
- **Performance**: Lazy loading reduces startup time and memory usage
- **Type safety**: Template-based approach catches errors at compile time

---

**See Also**:
- [Model-View-Delegate Pattern Documentation](03-Model-View-Delegate-Pattern.md)
- [Architecture Overview](01-Architecture-Overview.md)
- [Workflow Documentation](06-Application-Workflows.md)
