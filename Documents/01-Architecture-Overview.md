# E-TREK Architecture Overview

## Introduction

E-TREK is a medical imaging Digital Radiography (DR) system designed for capturing, processing, and managing X-ray images in clinical environments. Built with Qt 6.5.3 and C++17, the application follows a modular architecture that separates concerns across distinct packages, each responsible for a specific domain of functionality.

The system integrates with hospital information systems through DICOM Modality Worklist (MWL), controls X-ray hardware including generators and detectors, and provides comprehensive image viewing capabilities using VTK for medical image visualization. The architecture emphasizes maintainability, testability, and clear separation between user interface, business logic, and data access layers.

## Project Organization

The codebase is organized into functional modules, each compiled as a separate shared library. This modular approach allows teams to work independently on different aspects of the system while maintaining clear boundaries between components.

```
etrek/
├── Executable/          # Application entry point (main.cpp)
├── Application/         # Business logic, delegates, and page builders
├── Core/                # Foundation services (logging, security, settings)
├── View/                # User interface components (pages, widgets, dialogs)
├── Common/              # Shared interfaces and type definitions
├── Worklist/            # DICOM Modality Worklist management
├── Device/              # X-ray hardware configuration and control
├── Dicom/               # DICOM protocol implementation
├── Pacs/                # Picture Archiving and Communication System
├── ScanProtocol/        # Imaging procedures and technique parameters
├── ImageViewer/         # VTK-based image visualization
└── ThirdPartyLibraries/ # External dependencies (Qt, VTK, DCMTK, etc.)
```

The dependency hierarchy flows from high-level modules down to foundational services. The Application module depends on View for UI components and Core for services. Lower-level modules like Core and Common have no dependencies on upper layers, ensuring a clean architecture where changes in business logic don't ripple down to infrastructure code.

## Main Window and Page Architecture

The user interface follows a single-window, multi-page design pattern. The `MainWindow` class serves as the application shell, providing a toolbar for navigation and a central content area where pages are dynamically loaded and unloaded based on user actions.

### MainWindow Structure

The MainWindow contains a toolbar with actions for navigating between different functional areas of the application. When a user clicks a toolbar action, the MainWindow emits a signal that the MainWindowDelegate handles by loading the appropriate page. Only one page is visible at a time, and the previous page is properly cleaned up before loading a new one.

```cpp
// MainWindow provides the shell and navigation signals
class MainWindow : public QMainWindow {
signals:
    void LoadSystemPageAction();    // Settings page
    void LoadWorklistPageAction();  // Patient worklist
    void LoadExamPage();            // Examination/imaging
    void LoadViewPage();            // Image viewer
    void LoadOutputPage();          // PACS export
};
```

The MainWindow maintains a `m_pageContainer` layout where pages are inserted. The `loadPage()` method handles the transition between pages, ensuring proper cleanup of the previous page and smooth loading of the new one. During page loading, a busy cursor is displayed and toolbar actions are temporarily disabled to prevent user confusion.

### Page Loading Flow

When a navigation action is triggered, the following sequence occurs:

1. The MainWindow emits a signal (e.g., `LoadWorklistPageAction`)
2. The MainWindowDelegate receives the signal and calls `prepareLoadingPage()` to show a busy cursor
3. The delegate uses a Builder to create the page and its delegate
4. The page widget is passed to `loadPage()` which adds it to the content area
5. Finally, `finishLoadingPage()` restores the cursor and re-enables actions

This pattern ensures that page transitions are smooth and that resources from previous pages are properly released before new pages are loaded.

## Application Package

The Application package is the orchestration layer of the system. It contains the business logic that coordinates between the user interface (View) and the data/services layers (Core, Device, Worklist, etc.). The package is organized around two key patterns: Builders and Delegates.

### Builders

Builders are factory classes responsible for constructing pages along with their associated delegates and dependencies. A Builder receives a `DelegateParameter` structure containing shared resources like the database connection and context manager. Using these parameters, the Builder creates repository instances and wires everything together.

The Builder pattern isolates construction complexity from the page and delegate classes themselves. Pages don't need to know how to create their repositories or where the database connection comes from. This makes pages easier to test and maintain.

### Delegates

Delegates handle the business logic for their associated pages. While the page (in the View layer) manages UI rendering and user interaction, the delegate implements what happens when the user performs an action. Delegates hold references to repositories and services, coordinate data operations, and emit signals to update the UI.

This separation means that UI code in the View layer remains focused on presentation, while business rules and data access logic live in the Application layer where they can be more easily tested and modified.

### DelegateParameter

The `DelegateParameter` structure serves as a dependency injection container, passed from the application startup through to individual page builders. It contains:

- The active database connection
- The context manager (user session, workflow state)
- Any shared services needed across pages

By centralizing these dependencies in a single structure, the codebase avoids scattered dependency management and makes it clear what resources are available during page construction.

## Example: WorklistPage

To illustrate these patterns in practice, consider how the WorklistPage is constructed and operates. The WorklistPage displays the DICOM Modality Worklist, allowing operators to view scheduled procedures, add new patients, and select items for examination.

### Building the WorklistPage

When the user navigates to the worklist, the `MainWindowDelegate` calls the `WorkListPageBuilder`:

```cpp
void MainWindowDelegate::onLoadWorklistPageAction() {
    m_mainWindow->prepareLoadingPage();

    // Builder creates the page and delegate with all dependencies
    WorkListPageBuilder builder;
    auto [page, delegate] = builder.build(m_delegateParameter, m_mainWindow, this);

    m_worklistPageDelegate = delegate;

    // Connect delegate signals to handle navigation
    connect(delegate, &WorkListPageDelegate::startExamination,
            this, &MainWindowDelegate::onStartExamination);

    m_mainWindow->loadPage(page);
    m_mainWindow->finishLoadingPage();
}
```

### Inside the Builder

The `WorkListPageBuilder::build()` method creates the necessary repositories and wires them to the page and delegate:

```cpp
std::pair<WorkListPage*, WorkListPageDelegate*>
WorkListPageBuilder::build(const DelegateParameter& params,
                           QWidget* parentWidget,
                           QObject* parentDelegate) {
    // Create repositories from the shared database connection
    auto worklistRepository = std::make_shared<WorklistRepository>(params.dbConnection);
    auto scanRepository = std::make_shared<ScanProtocolRepository>(params.dbConnection);
    auto dicomRepository = std::make_shared<DicomRepository>(params.dbConnection);

    // Create the page widget
    auto* page = new WorkListPage(worklistRepository, parentWidget);

    // Create the delegate with repositories and context
    auto* delegate = new WorkListPageDelegate(
        page,
        worklistRepository,
        scanRepository,
        dicomRepository,
        params.dbConnection,
        params.contextManager,
        parentDelegate);

    return { page, delegate };
}
```

### Delegate Responsibilities

The `WorkListPageDelegate` handles business operations such as:

- Querying the MWL from connected RIS systems
- Filtering and searching worklist entries
- Creating new local patients
- Starting examinations when items are selected
- Coordinating with the ContextManager to track workflow state

When the user selects a worklist item and clicks "Start Exam", the delegate validates the selection, updates the workflow context, and emits a signal that causes the MainWindowDelegate to navigate to the ExamPage.

## Application Startup

The application initialization follows a well-defined sequence managed by the `ApplicationService` class. Different launch modes (normal, demo, settings manager, user manager) are handled through the Strategy pattern.

### Launch Modes

The application supports multiple launch modes specified via command-line arguments:

- **(default)**: Full application with authentication and all features
- **--demo**: Testing mode with sample data
- **--setting-manager**: Opens only the system settings interface
- **--user-manager**: Opens only the user account management interface
- **--developer**: Diagnostic and debugging tools

Each mode is implemented as a strategy class that configures which services to initialize and which UI to display. This approach allows the same codebase to serve different purposes without conditional logic scattered throughout.

### Initialization Sequence

When the application starts in normal mode, the following initialization occurs:

1. **Logging Setup**: The spdlog-based logging system is configured with file rotation and appropriate log levels.

2. **Settings Loading**: Configuration is read from `Settings.json`, including database connection parameters, RIS connections, and file paths.

3. **Database Connection**: A connection to the MySQL database is established using the configured credentials.

4. **Authentication**: The login dialog is presented, and the user must authenticate before proceeding.

5. **Context Initialization**: The ContextManager is created to track user session and workflow state throughout the application lifecycle.

6. **Main Window Creation**: The MainWindowBuilder constructs the main window and its delegate, wiring up navigation signals.

7. **Initial Page Load**: Depending on configuration, the application may automatically navigate to the WorklistPage or wait for user action.

## Core Services

The Core package provides foundational services used throughout the application. These services are designed to be stateless where possible and thread-safe where necessary.

### Logging

The logging system is built on spdlog and provides structured, rotated log files. The `LoggerProvider` singleton manages logger instances, and components can request loggers for specific subsystems. Log files are automatically rotated based on size and age, with configurable retention policies.

### Security

User authentication is handled by the `AuthenticationService`, which validates credentials against hashed passwords stored in the database. The `CryptoManager` provides password hashing using industry-standard algorithms. Session tokens track logged-in users and their roles throughout the application.

### Settings

The `SettingProvider` loads configuration from JSON files at startup. Database passwords can be stored encrypted, with automatic decryption during loading. Settings include database connection parameters, RIS configurations, logging preferences, and system behavior flags.

### Context Management

The `ContextManager` maintains application state that spans across pages and operations. The `SessionContext` tracks the currently logged-in user and their permissions, while the `WorkflowContext` tracks the current examination state, selected worklist item, and imaging progress.

## Design Principles

### Separation of Concerns

Each module has a single, well-defined responsibility. The View layer handles only UI rendering and user interaction. The Application layer contains business logic and workflow coordination. The Core layer provides infrastructure services. This separation makes the codebase easier to understand, test, and modify.

### Dependency Direction

Dependencies flow inward toward more stable, foundational code. Upper layers (Application, View) depend on lower layers (Core, Common), but never the reverse. This ensures that changes to business logic or UI don't require modifications to infrastructure code.

### Memory Management

The codebase uses smart pointers (`std::shared_ptr`, `std::unique_ptr`) for heap-allocated objects and relies on Qt's parent-child ownership model for QObject-derived classes. Raw `new` and `delete` are avoided in business logic, with RAII patterns ensuring proper resource cleanup.

### Qt Integration

The application leverages Qt's signal/slot mechanism for loose coupling between components. Qt's meta-object system enables runtime type information and property binding. Asynchronous operations use Qt's event loop rather than manual threading where possible.

## Third-Party Dependencies

The application relies on several external libraries, all organized in the `ThirdPartyLibraries/` directory:

- **Qt 6.5.3**: Provides the application framework including UI widgets, networking, database access, and OpenGL integration
- **VTK 9.5**: Powers the medical image visualization with support for windowing, measurements, and annotations
- **DCMTK**: Implements the DICOM protocol for worklist queries, image storage, and PACS communication
- **OpenSSL**: Provides cryptographic functions for password hashing and secure connections
- **spdlog**: High-performance logging with support for multiple sinks and automatic rotation
- **MySQL**: Relational database for storing patient data, worklist entries, and system configuration

---

**Next**: See [02-Database-Schema.md](./02-Database-Schema.md) for database design documentation.
