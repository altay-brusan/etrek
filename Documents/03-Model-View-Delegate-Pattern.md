# Model-View-Delegate Pattern in Etrek

## Overview

Etrek implements a sophisticated **Model-View-Delegate (MVD) pattern**, which is a variation of the Model-View-Controller (MVC) pattern where Delegates act as controllers. This pattern provides excellent separation of concerns between UI presentation, business logic, and data access while managing complex inter-component communication.

## Architecture

### Core Concepts

The MVD pattern in Etrek consists of three main layers:

1. **Model**: Data entities, repositories, and Qt models (QAbstractTableModel, QStandardItemModel)
2. **View**: Qt Widgets (Pages, Dialogs, Configuration Widgets) that render UI
3. **Delegate**: Controller-like objects that mediate between View and Model, handling business logic

```
┌─────────────────────────────────────────────────────────────┐
│                       User Interface Layer                   │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │  Page    │  │  Dialog  │  │  Widget  │  │ Qt Models│   │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘   │
│       │ signals     │ signals     │ signals     │           │
└───────┼─────────────┼─────────────┼─────────────┼───────────┘
        │             │             │             │
        ▼             ▼             ▼             ▼
┌─────────────────────────────────────────────────────────────┐
│                      Delegate Layer                          │
│  ┌──────────────────────────────────────────────────┐       │
│  │  Delegate (QObject + IDelegate + IPageAction)    │       │
│  │  - Receives signals from View                     │       │
│  │  - Calls repositories for data access            │       │
│  │  - Updates Qt models                             │       │
│  │  - Emits signals to other delegates              │       │
│  └────────┬────────────────────────────────────┬────┘       │
└───────────┼────────────────────────────────────┼────────────┘
            │                                    │
            ▼                                    ▼
┌─────────────────────────────────────────────────────────────┐
│                      Model/Data Layer                        │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │Repository│  │ Entities │  │ Qt Models│  │ Services │   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
└─────────────────────────────────────────────────────────────┘
```

## Delegate Interfaces

### IDelegate - Base Interface

**Location**: `Common/Include/IDelegate.h`

Every delegate implements this interface to provide identification and dependency injection:

```cpp
class IDelegate {
public:
    virtual ~IDelegate() = default;

    // Returns unique name for identification
    virtual QString name() const = 0;

    // Injects prerequisite delegates this delegate depends on
    virtual void attachDelegates(const QVector<QObject*>& delegates) = 0;
};

Q_DECLARE_INTERFACE(IDelegate, "com.etrek.IDelegate/1.0")
```

### IPageAction - Page Lifecycle Management

**Location**: `Common/Include/IPageAction.h`

Delegates managing configuration pages implement this interface:

```cpp
class IPageAction {
public:
    virtual ~IPageAction() = default;

    // Apply intermediate changes without finalizing
    virtual void apply() = 0;

    // Finalize and persist all changes
    virtual void accept() = 0;

    // Discard changes and revert to last saved state
    virtual void reject() = 0;
};

Q_DECLARE_INTERFACE(IPageAction, "com.etrek.IPageAction/1.0")
```

### DelegateParameter - Dependency Container

**Location**: `Common/Include/DelegateParameter.h`

This structure carries configuration and context to builders:

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

    // Pre-fetched workflow context relevant to the delegate
    std::shared_ptr<Etrek::Context::IWorkflowContext> workflowContext;
};
```

**Key Pattern**: DelegateParameter ONLY contains `dbConnection`, not repositories. Builders create repositories from `dbConnection` and pass them to delegates.

## View Components and Delegates

### Application-Level Components

| View Component | Delegate | Purpose |
|----------------|----------|---------|
| **MainWindow** | MainWindowDelegate | Main application window; orchestrates page switching |
| **WorkListPage** | WorkListPageDelegate | Displays and manages DICOM worklist entries |
| **SystemSettingPage** | SystemSettingPageDelegate | Composite page with 12 configuration sub-pages |
| **ViewSelectionDialog** | ViewSelectionDialogDelegate | Dialog for selecting procedures and radiographic views |

### Configuration Widgets (System Settings Sub-Pages)

#### Device Module

| Widget | Delegate | Purpose |
|--------|----------|---------|
| GeneratorConfigurationWidget | GeneratorConfigurationDelegate | X-ray generator configuration and management |
| DetectorConfigurationWidget | DetectorConfigurationDelegate | Digital detector setup and calibration |
| CollimatorConfigurationWidget | CollimatorConfigurationDelegate | Collimator configuration |
| DapConfigurationWidget | DapConfigurationDelegate | Dose-Area Product (DAP) meter configuration |
| WorkflowConfigurationWidget | WorkflowConfigurationDelegate | General equipment and environment settings |

#### Scan Protocol Module

| Widget | Delegate | Purpose |
|--------|----------|---------|
| ProcedureConfigurationWidget | ProcedureConfigurationDelegate | Define imaging procedures (exams) |
| ViewConfigurationWidget | ViewConfigurationDelegate | Configure radiological views (AP, LAT, etc.) |
| TechniqueConfigurationWidget | TechniqueConfigurationDelegate | Exposure technique parameters (kVp, mAs, grid) |

#### Integration & Connectivity

| Widget | Delegate | Purpose |
|--------|----------|---------|
| ConnectionSetupWidget | ConnectionSetupDelegate | RIS/PACS network connectivity |
| WorkListConfigurationWidget | WorkListConfigurationDelegate | DICOM worklist field mapping |
| PacsEntityConfigurationWidget | PacsEntityConfigurationDelegate | PACS node configuration (archive, MPPS) |
| ImageCommentConfigurationWidget | ImageCommentConfigurationDelegate | Predefined image comment templates |

## Signal-Slot Communication Pattern

### View → Delegate Communication

Views emit signals that delegates receive and handle:

**Example: WorkListPage → WorkListPageDelegate**

```cpp
// View (WorkListPage.h)
class WorkListPage : public QWidget {
    Q_OBJECT
signals:
    void addNewPatient();
    void updatePatient();
    void worklistItemDoubleClicked(int entryId);
    void filterDateSpanChanged(const DateTimeSpan& date);
    void searchName(const QString& name);
};

// Delegate (WorkListPageDelegate.h)
class WorkListPageDelegate : public QObject, public IDelegate, public IPageAction {
    Q_OBJECT
    Q_INTERFACES(IDelegate IPageAction)

private slots:
    void onAddNewPatient();
    void onUpdatePatient();
    void onWorklistItemDoubleClicked(int entryId);
    void onFilterDateRangeChanged(const DateTimeSpan& date);
    void onSearchName(const QString& name);
};

// Delegate implementation (WorkListPageDelegate.cpp)
WorkListPageDelegate::WorkListPageDelegate(WorkListPage* ui, ...) {
    // Wire view signals to delegate slots
    connect(ui, &WorkListPage::addNewPatient,
            this, &WorkListPageDelegate::onAddNewPatient);
    connect(ui, &WorkListPage::worklistItemDoubleClicked,
            this, &WorkListPageDelegate::onWorklistItemDoubleClicked);
    connect(ui, &WorkListPage::filterDateSpanChanged,
            this, &WorkListPageDelegate::onFilterDateRangeChanged);
}
```

### Delegate → Delegate Communication

Delegates can communicate with each other through signals:

**Example: WorkListPageDelegate → ViewSelectionDialogDelegate**

```cpp
void WorkListPageDelegate::onWorklistItemDoubleClicked(int entryId) {
    // Set workflow context
    auto entry = repository->getWorklistEntryById(entryId);
    auto examContext = std::make_shared<ExaminationContext>(entry.value);

    if (auto ctxMgr = contextManager.lock()) {
        ctxMgr->setWorkflowContext("Examination", examContext);
    }

    // Open view selection dialog
    ViewSelectionDialogBuilder builder;
    auto [dialog, delegate] = builder.build(m_params, ui, this);

    // Connect delegate signals
    connect(delegate, &ViewSelectionDialogDelegate::examinationReady,
            this, [this](int procedureId, const QVector<int>& viewIds) {
                emit startExamination(procedureId);
            });

    dialog->exec();
}
```

### Hierarchical Delegate Pattern

Parent delegates manage child delegates through the `attachDelegates()` interface:

**Example: SystemSettingPageDelegate Managing 12 Child Delegates**

```cpp
class SystemSettingPageDelegate : public QObject, public IDelegate, public IPageAction {
    Q_OBJECT

public:
    void attachDelegates(const QVector<QObject*>& list) override {
        for (auto* obj : list) {
            if (!delegates.contains(obj)) {
                delegates.append(QPointer<QObject>(obj));
            }
        }
    }

    void apply() override {
        // Broadcast apply to all child delegates
        for (const auto& delegatePtr : delegates) {
            if (!delegatePtr) continue;

            if (auto* action = qobject_cast<IPageAction*>(delegatePtr.data())) {
                action->apply();
            }
        }
    }

    void accept() override {
        // Finalize all child delegates
        for (const auto& delegatePtr : delegates) {
            if (auto* action = qobject_cast<IPageAction*>(delegatePtr.data())) {
                action->accept();
            }
        }
        emit closeSettings();
    }

private:
    QVector<QPointer<QObject>> delegates;  // Child delegates
};
```

## Models in MVD Pattern

### Qt Standard Models

#### QStandardItemModel

Used for in-memory tabular data:

```cpp
// In WorkListPageDelegate
baseModel = new QStandardItemModel(this);
baseModel->setColumnCount(10);
baseModel->setHorizontalHeaderLabels({
    "Patient Name", "Patient ID", "Birth Date", "Gender",
    "Accession No", "Study", "Body Part", "Source", "Created", "Status"
});

// Populate model
QList<QStandardItem*> row = createRowForEntry(worklistEntry);
baseModel->appendRow(row);

// Connect to view
proxyModel->setSourceModel(baseModel);
ui->setProxyModel(proxyModel);
```

#### QSortFilterProxyModel (Custom)

**WorklistFilterProxyModel** provides multi-criteria filtering:

```cpp
class WorklistFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    void setDateRangeFilter(const QDate& from, const QDate& to);
    void setSourceFilter(const QString& source);
    void setSearchName(const QString& name);
    void setSearchPatientId(const QString& id);
    void setSearchAccessionNo(const QString& accNo);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
};
```

### Custom Table Models

#### GeneratorTableModel

```cpp
class GeneratorTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Columns {
        ColManufacturer = 0,
        ColIsActive,
        ColModel,
        ColPartNumber,
        ColSerial,
        ColTypeNumber,
        ColTechSpecs,
        ColOutput1, ColOutput1Active,
        ColOutput2, ColOutput2Active,
        ColMfgDate, ColInstDate, ColCalibDate,
        ColCount
    };

    void setDataSource(const QVector<Generator>& generators);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};
```

### Domain Models

#### PatientModel (Business Logic Model)

Converts DICOM worklist entries to application-specific patient representation:

```cpp
class PatientModel {
public:
    QString firstName, middleName, lastName;
    QString patientId;
    QDate dateOfBirth;
    int age;
    Gender gender;
    QString referringPhysician;
    QString patientLocation;
    QString admissionNumber;
    QString accessionNumber;
    QList<BodyPartSelection> selectedBodyParts;

    bool isValid() const;
    static PatientModel fromWorklistEntry(const WorklistEntry& entry);
};
```

## Composite Pattern: SystemSettingPage

The SystemSettingPage demonstrates hierarchical composition:

```
SystemSettingPage (QWidget)
│
├─ SystemSettingPageDelegate (QObject + IDelegate + IPageAction)
│   │
│   ├─ WorkflowConfigurationDelegate
│   ├─ GeneratorConfigurationDelegate
│   ├─ DetectorConfigurationDelegate
│   ├─ CollimatorConfigurationDelegate
│   ├─ DapConfigurationDelegate
│   ├─ ProcedureConfigurationDelegate
│   ├─ ViewConfigurationDelegate
│   ├─ TechniqueConfigurationDelegate
│   ├─ WorkListConfigurationDelegate
│   ├─ PacsEntityConfigurationDelegate
│   ├─ ConnectionSetupDelegate
│   └─ ImageCommentConfigurationDelegate
│
└─ Page Signals: saveSettings, applySettings, closeSettings
```

**Event Propagation**: When user clicks "Apply":

1. SystemSettingPage emits `applySettings` signal
2. SystemSettingPageDelegate receives signal via `apply()` slot
3. Parent delegate iterates through all 12 child delegates
4. Each child's `apply()` method is called (polymorphic dispatch)
5. Each child handles its own state persistence

## Repository Injection Pattern

Delegates receive repositories (not database connections) for data access:

**Example: WorkListPageDelegate**

```cpp
class WorkListPageDelegate {
public:
    WorkListPageDelegate(
        WorkListPage* ui,
        std::shared_ptr<WorklistRepository> repository,
        std::shared_ptr<ScanProtocolRepository> scanRepository,
        std::shared_ptr<DicomRepository> dicomRepository,
        std::shared_ptr<DicomTagRepository> dicomTagRepository,
        std::shared_ptr<DatabaseConnectionSetting> dbConnection,
        std::weak_ptr<IContextManager> contextManager,
        QObject* parent = nullptr
    );

private:
    WorkListPage* ui;
    std::shared_ptr<WorklistRepository> repository;
    std::shared_ptr<ScanProtocolRepository> scanRepository;
    std::shared_ptr<DicomRepository> dicomRepository;
    std::shared_ptr<DicomTagRepository> dicomTagRepository;
    std::weak_ptr<IContextManager> contextManager;
};
```

**Key Architectural Rule**: Delegates NEVER receive `dbConnection` directly. Builders create repositories from `dbConnection` and inject them into delegates.

## Lazy Construction Pattern

Pages are built on-demand when user navigates to them:

```cpp
void MainWindowDelegate::onLoadSystemPageAction() {
    // Clean up existing page
    if (m_systemSettingPageDelegate) {
        m_systemSettingPageDelegate->deleteLater();
        m_systemSettingPageDelegate = nullptr;
    }

    // Build new page using builder
    SystemSettingPageBuilder builder;
    auto [page, delegate] = builder.build(m_params, nullptr, this);
    m_systemSettingPageDelegate = delegate;

    // Wire delegate signals
    connect(delegate, &SystemSettingPageDelegate::closeSettings,
            this, [page, this]() {
                if (m_mainWindow) {
                    m_mainWindow->closePage();
                }
            });

    // Display page
    m_mainWindow->loadPage(page);
}
```

## Complete Example: WorkListPageDelegate

### View Definition (WorkListPage.h)

```cpp
class WorkListPage : public QWidget {
    Q_OBJECT

public:
    explicit WorkListPage(std::shared_ptr<IWorklistRepository> repository,
                         QWidget* parent = nullptr);

    void setProxyModel(WorklistFilterProxyModel* model);

signals:
    void addNewPatient();
    void updatePatient();
    void worklistItemDoubleClicked(int entryId);
    void filterDateSpanChanged(const DateTimeSpan& date);
    void filterSourceChanged(const QString& source);
    void clearAllFilters();
    void searchName(const QString& name);

private:
    Ui::WorkListPage* ui;
    WorklistFilterProxyModel* proxyModel;
};
```

### Delegate Definition (WorkListPageDelegate.h)

```cpp
class WorkListPageDelegate : public QObject, public IDelegate, public IPageAction {
    Q_OBJECT
    Q_INTERFACES(IDelegate IPageAction)

public:
    explicit WorkListPageDelegate(
        WorkListPage* ui,
        std::shared_ptr<WorklistRepository> repository,
        std::shared_ptr<ScanProtocolRepository> scanRepository,
        std::shared_ptr<DicomRepository> dicomRepository,
        std::shared_ptr<DicomTagRepository> dicomTagRepository,
        std::shared_ptr<DatabaseConnectionSetting> dbConnection,
        std::weak_ptr<IContextManager> contextManager,
        QObject* parent = nullptr
    );

    QString name() const override { return "WorkListPageDelegate"; }
    void attachDelegates(const QVector<QObject*>& delegates) override;

signals:
    void closeWorklist();
    void startExamination(int entryId);

private slots:
    void onWorklistItemDoubleClicked(int entryId);
    void onUpdatePatient();
    void onAddNewPatient();
    void onFilterDateRangeChanged(const DateTimeSpan& date);
    void onSourceChanged(const QString& source);
    void onClearFilters();
    void onSearchName(const QString& name);

    // IPageAction implementation
    void apply() override;
    void accept() override;
    void reject() override;

private:
    WorkListPage* ui;
    QPointer<QStandardItemModel> baseModel;
    QPointer<WorklistFilterProxyModel> proxyModel;
    std::shared_ptr<WorklistRepository> repository;
    std::shared_ptr<ScanProtocolRepository> scanRepository;
    std::shared_ptr<DicomRepository> dicomRepository;
    std::shared_ptr<DicomTagRepository> dicomTagRepository;
    std::weak_ptr<IContextManager> contextManager;
};
```

### Delegate Implementation (WorkListPageDelegate.cpp)

```cpp
WorkListPageDelegate::WorkListPageDelegate(
    WorkListPage* ui,
    std::shared_ptr<WorklistRepository> repository,
    std::shared_ptr<ScanProtocolRepository> scanRepository,
    std::shared_ptr<DicomRepository> dicomRepository,
    std::shared_ptr<DicomTagRepository> dicomTagRepository,
    std::shared_ptr<DatabaseConnectionSetting> dbConnection,
    std::weak_ptr<IContextManager> contextManager,
    QObject* parent)
    : QObject(parent),
      ui(ui),
      repository(repository),
      scanRepository(scanRepository),
      dicomRepository(dicomRepository),
      dicomTagRepository(dicomTagRepository),
      contextManager(contextManager)
{
    // Create models
    baseModel = new QStandardItemModel(this);
    proxyModel = new WorklistFilterProxyModel(this);
    proxyModel->setSourceModel(baseModel);
    ui->setProxyModel(proxyModel);

    // Wire view signals to delegate slots
    connect(ui, &WorkListPage::addNewPatient,
            this, &WorkListPageDelegate::onAddNewPatient);
    connect(ui, &WorkListPage::worklistItemDoubleClicked,
            this, &WorkListPageDelegate::onWorklistItemDoubleClicked);
    connect(ui, &WorkListPage::filterDateSpanChanged,
            this, &WorkListPageDelegate::onFilterDateRangeChanged);
    connect(ui, &WorkListPage::searchName,
            this, &WorkListPageDelegate::onSearchName);

    // Load initial data
    loadWorklistData();
}

void WorkListPageDelegate::onAddNewPatient() {
    auto regionsResult = scanRepository->getAllAnatomicRegions();
    auto partsResult = scanRepository->getAllBodyParts();

    AddPatientDialog dlg(regionsResult.value, partsResult.value, ui);
    if (dlg.exec() == QDialog::Accepted) {
        auto patientModel = dlg.getPatientModel();
        // Create worklist entry from patient model
        WorklistEntry entry = createWorklistEntry(patientModel);
        auto result = repository->createWorklistEntry(entry);

        if (result.isSuccess) {
            // Add to model
            QList<QStandardItem*> row = createRowForEntry(entry);
            baseModel->appendRow(row);
        }
    }
}

void WorkListPageDelegate::onWorklistItemDoubleClicked(int entryId) {
    // Retrieve worklist entry
    auto entryResult = repository->getWorklistEntryById(entryId);
    if (!entryResult.isSuccess) return;

    // Set examination context
    auto examContext = std::make_shared<ExaminationContext>(entryResult.value);
    if (auto ctxMgr = contextManager.lock()) {
        ctxMgr->setWorkflowContext("Examination", examContext);
    }

    // Signal to start examination
    emit startExamination(entryId);
}

void WorkListPageDelegate::onFilterDateRangeChanged(const DateTimeSpan& date) {
    proxyModel->setDateRangeFilter(date.fromDate, date.toDate);
}

void WorkListPageDelegate::apply() {
    // Apply intermediate changes (if any)
}

void WorkListPageDelegate::accept() {
    // Finalize changes (if any)
}

void WorkListPageDelegate::reject() {
    // Discard changes (if any)
}
```

## Best Practices

### 1. Clear Separation of Concerns

- **Views**: Only UI rendering, emit signals for user actions
- **Delegates**: Business logic, repository calls, model updates
- **Models**: Data representation and storage

### 2. Signal-Slot for Communication

- Use Qt's signal-slot mechanism for loose coupling
- Views signal user actions, delegates handle them
- Delegates signal workflow events, other delegates respond

### 3. Repository Abstraction

- Delegates interact with repositories, not raw database connections
- Repositories encapsulate SQL queries and data access logic
- Use interfaces (`IWorklistRepository`) for testability

### 4. Qt Parent-Child Ownership

- Delegates are QObjects with parent ownership
- Qt automatically destroys children when parent is destroyed
- No manual memory management needed for most objects

### 5. Weak Pointers for Delegates

- Use `QWeakPointer` for delegate references to avoid circular dependencies
- Context manager and inter-delegate references use weak pointers
- Check validity before dereferencing: `if (auto ptr = weakPtr.lock())`

### 6. Context Injection

- Use `DelegateParameter` to inject context, database connection, and other dependencies
- Context manager provides session and workflow context
- Pre-fetch frequently used context for performance

## Summary

The Model-View-Delegate pattern in Etrek provides:

- **Clear separation**: UI, business logic, and data access are distinct
- **Loose coupling**: Signal-slot communication, interface-based design
- **Reusability**: Delegates can be composed hierarchically
- **Testability**: Repository injection enables unit testing
- **Qt integration**: Leverages Qt's parent-child ownership and meta-object system
- **Scalability**: Easily add new pages, widgets, and delegates following the established pattern

This pattern is consistently applied across all 15+ pages, 12+ configuration widgets, and their corresponding delegates throughout the application.

---

**See Also**:
- [Builder Pattern Documentation](04-Builder-Pattern.md)
- [Architecture Overview](01-Architecture-Overview.md)
- [Workflow Documentation](06-Application-Workflows.md)
