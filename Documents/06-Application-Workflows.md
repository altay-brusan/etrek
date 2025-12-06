# Etrek Application Workflows

## Overview

This document provides comprehensive documentation of all major workflows in the Etrek medical imaging application, including application initialization, authentication, DICOM worklist management, and PACS integration.

## Table of Contents

1. [Application Initialization](#1-application-initialization)
2. [Authentication Workflow](#2-authentication-workflow)
3. [DICOM Worklist Query Workflow](#3-dicom-worklist-query-workflow)
4. [Launch Strategies](#4-launch-strategies)
5. [PACS Integration Workflow](#5-pacs-integration-workflow)
6. [Examination Workflow](#6-examination-workflow)
7. [Context Management](#7-context-management)

---

## 1. Application Initialization

### Entry Point (main.cpp)

The application starts with a robust initialization sequence that ensures single-instance execution and proper resource setup.

#### Step-by-Step Initialization

**1. Application Setup**

```cpp
int main(int argc, char *argv[]) {
    // Enable memory leak detection (Windows debug builds)
    #if defined(_DEBUG) && defined(_MSC_VER)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    // Create Qt application
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("EtrekApp");
    QCoreApplication::setOrganizationName("Etrek");
    QCoreApplication::setApplicationVersion("1.0.0");
}
```

**2. Single Instance Lock**

```cpp
// Prevent multiple instances from running
QString lockPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                   + "/etrek_app.lock";
QLockFile lock(lockPath);

if (!lock.tryLock(0)) {
    QMessageBox::warning(nullptr, "Application Already Running",
                        "Another instance of Etrek is already running.");
    return EXIT_FAILURE;
}
```

**3. Signal Handling**

```cpp
// Install OS signal handlers for graceful/immediate shutdown
SignalGuard signalGuard;
// Captures SIGINT, SIGTERM
// Calls std::_Exit(EXIT_FAILURE) for immediate termination
```

**4. Logger Shutdown Hook**

```cpp
// Ensure logger is properly shutdown when application quits
QObject::connect(&app, &QCoreApplication::aboutToQuit, []() {
    Etrek::Core::Log::LoggerProvider::Instance().Shutdown();
});
```

**5. Parse Launch Mode**

```cpp
const auto arguments = QCoreApplication::arguments();
const auto mode = Etrek::Specification::ParseLaunchMode(arguments);

// Supported modes:
// - MainApp (default)
// - UserManager (usermgr)
// - SettingManager (settingmgr)
// - Demo (demo)
// - Developer (developer)
```

**6. Create and Initialize ApplicationService**

```cpp
Etrek::Application::Service::ApplicationService applicationService(&app);
applicationService.initialize(mode);
```

**7. Run Event Loop**

```cpp
return app.exec();
```

### ApplicationService Initialization Sequence

#### Initialization Dispatcher

```cpp
void ApplicationService::initialize(LaunchMode mode) {
    // 1. Register Qt meta-types for signal/slot communication
    qRegisterMetaType<Role>("Role");
    qRegisterMetaType<User>("User");
    qRegisterMetaType<QVector<int>>("QVector<int>");

    // 2. Create and execute launch strategy
    auto strategy = createLaunchStrategy(mode);  // Factory pattern
    strategy->launch(this);  // Polymorphic execution
}
```

#### MainAppLaunchStrategy (Full Application Launch)

**Progress**: 0% → 100%

```
┌──────────────────────────────────────────────────────────┐
│  Splash Screen: "Loading Etrek..."                      │
└──────────────────────────────────────────────────────────┘
                       ↓
┌──────────────────────────────────────────────────────────┐
│  1. Load Settings (10%)                                  │
│     - Read ./setting/Settings.json                       │
│     - Parse database connection settings                 │
│     - Parse RIS connection configurations                │
│     - Parse file logger settings                         │
└──────────────────────────────────────────────────────────┘
                       ↓
┌──────────────────────────────────────────────────────────┐
│  2. Setup Logger (10%)                                   │
│     - Initialize spdlog file logger                      │
│     - Create ApplicationService logger                   │
│     - Configure log rotation                             │
└──────────────────────────────────────────────────────────┘
                       ↓
┌──────────────────────────────────────────────────────────┐
│  3. Initialize Database (15%)                            │
│     - Connect to MySQL database                          │
│     - Create database if missing                         │
│     - Execute schema setup script                        │
│     - Verify tables exist                                │
└──────────────────────────────────────────────────────────┘
                       ↓
┌──────────────────────────────────────────────────────────┐
│  4. Initialize Authentication (30%)                      │
│     - Create AuthenticationService                       │
│     - Initialize CryptoManager                           │
│     - Load user roles from database                      │
└──────────────────────────────────────────────────────────┘
                       ↓
┌──────────────────────────────────────────────────────────┐
│  5. Initialize RIS Connections (20%)                     │
│     - Create ModalityWorklistManager                     │
│     - Load worklist profiles                             │
│     - Start DICOM C-FIND timer (5 min interval)         │
│     - Start DICOM C-ECHO timer (30 sec interval)        │
└──────────────────────────────────────────────────────────┘
                       ↓
┌──────────────────────────────────────────────────────────┐
│  6. Load Main Window (10%)                               │
│     - Use MainWindowBuilder                              │
│     - Create MainWindow + MainWindowDelegate             │
│     - Inject DelegateParameter                           │
└──────────────────────────────────────────────────────────┘
                       ↓
┌──────────────────────────────────────────────────────────┐
│  7. Connect Signals (1%)                                 │
│     - Wire inter-service communication                   │
└──────────────────────────────────────────────────────────┘
                       ↓
┌──────────────────────────────────────────────────────────┐
│  8. Close Splash Screen                                  │
└──────────────────────────────────────────────────────────┘
                       ↓
┌──────────────────────────────────────────────────────────┐
│  9. Authenticate User (blocking)                         │
│     - Show LoginDialog                                   │
│     - Validate credentials                               │
│     - Create SessionContext on success                   │
└──────────────────────────────────────────────────────────┘
                       ↓
           ┌───────────┴───────────┐
           ▼                       ▼
    ┌──────────────┐      ┌──────────────┐
    │ Success      │      │ Failure      │
    │ Show MainWin │      │ Exit App     │
    └──────────────┘      └──────────────┘
```

### Service Initialization Dependencies

```
SettingProvider (JSON files)
    ↓
LoggerProvider (spdlog)
    ↓
DatabaseSetupManager (MySQL + Qt SQL)
    ↓
┌─────────────────────────────────────────────┐
│  Repository Layer (all require database)    │
├─ AuthenticationRepository                   │
├─ WorklistRepository                         │
├─ DeviceRepository                           │
├─ PacsNodeRepository                         │
└─ ScanProtocolRepository                     │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│  Service Layer                               │
├─ AuthenticationService                      │
├─ ModalityWorklistManager                    │
└─ ContextManager                             │
└─────────────────────────────────────────────┘
    ↓
MainWindow + MainWindowDelegate
```

---

## 2. Authentication Workflow

### Two-Phase Authentication

#### Phase 1: Initialization

```cpp
void ApplicationService::intializeAuthentication(ProgressCallback callback) {
    callback("Initializing Authentication Service...", 30);

    // Create authentication service
    m_authService = new AuthenticationService(
        m_authRepository,
        m_cryptoManager,
        this  // Qt parent
    );

    callback("Authentication Service Ready", 30);
}
```

#### Phase 2: User Login

**Step-by-Step Login Flow**

```
ApplicationService::authenticateUser()
    ↓
┌──────────────────────────────────────────────────────────┐
│  1. Query Active Users                                   │
│     - AuthenticationRepository::getAllActiveUsers()      │
│     - Filter by is_active = TRUE AND is_deleted = FALSE  │
└──────────────────────────────────────────────────────────┘
    ↓
    Decision: Any active users?
    ↓                         ↓
   YES                       NO
    ↓                         ↓
    │               ┌──────────────────────┐
    │               │ Show User Manager    │
    │               │ Create first admin   │
    │               └──────────────────────┘
    ↓                         ↓
┌──────────────────────────────────────────────────────────┐
│  2. Display Login Dialog                                 │
│     - Show list of active usernames                      │
│     - User selects username                              │
│     - User enters password                               │
└──────────────────────────────────────────────────────────┘
    ↓
┌──────────────────────────────────────────────────────────┐
│  3. Retrieve User Credentials                            │
│     - LoginDialog::getLoginCredentials()                 │
│     - Returns: (User, QString password)                  │
└──────────────────────────────────────────────────────────┘
    ↓
┌──────────────────────────────────────────────────────────┐
│  4. Fetch User from Database                             │
│     - AuthenticationRepository::getUser(username)        │
│     - Returns User entity with encrypted PasswordHash    │
└──────────────────────────────────────────────────────────┘
    ↓
┌──────────────────────────────────────────────────────────┐
│  5. Decrypt and Verify Password                          │
│     - CryptoManager::decryptPassword(passwordHash)       │
│     - Compare entered password with decrypted            │
│     - Case-insensitive comparison                        │
└──────────────────────────────────────────────────────────┘
    ↓
    Decision: Password matches?
    ↓                         ↓
   YES                       NO
    ↓                         ↓
    │               ┌──────────────────────┐
    │               │ Clear password field │
    │               │ Retry login dialog   │
    │               └──────────────────────┘
    ↓                         ↓
┌──────────────────────────────────────────────────────────┐
│  6. Create Session Context                               │
│     - SessionContext(user, workstation)                  │
│     - ContextManager::setSessionContext(sessionCtx)      │
└──────────────────────────────────────────────────────────┘
    ↓
┌──────────────────────────────────────────────────────────┐
│  7. Return Authenticated User                            │
│     - Result<std::optional<User>>::Success(user)         │
└──────────────────────────────────────────────────────────┘
    ↓
┌──────────────────────────────────────────────────────────┐
│  8. Show Main Window                                     │
│     - ApplicationService::showMainWindow()               │
└──────────────────────────────────────────────────────────┘
```

### User Entity

```cpp
class User {
public:
    int Id = -1;
    QString Username;           // Unique, login identifier
    QString Name;              // First name
    QString Surname;           // Last name
    QString PasswordHash;      // Encrypted password (OpenSSL)
    bool IsActive = true;      // Can login?
    bool IsDeleted = false;    // Soft delete
    QDateTime CreateDate;
    QDateTime UpdateDate;
    QVector<Role> Roles;       // RBAC roles
};
```

### Password Security

**Encryption**: OpenSSL-based cryptography via `CryptoManager`

```cpp
// Password hashing during user creation
QString encryptedPassword = cryptoManager.encryptPassword(plainPassword);
user.PasswordHash = encryptedPassword;
repository.createUser(user);

// Password verification during login
QString decryptedPassword = cryptoManager.decryptPassword(user.PasswordHash);
bool isValid = (plainPassword.compare(decryptedPassword, Qt::CaseInsensitive) == 0);
```

---

## 3. DICOM Worklist Query Workflow

### Architecture Overview

The worklist system uses a **multi-threaded producer-consumer pattern**:

- **Main Thread**: UI updates, timer management
- **Worker Thread**: DCMTK DICOM operations (C-FIND, C-ECHO)
- **Synchronization**: Qt signals/slots, `QMutex` for thread safety

### Components

```
┌─────────────────────────────────────────────────────────┐
│  Main Thread                                            │
│  ┌──────────────────────────────────────────────────┐  │
│  │  ModalityWorklistManager                         │  │
│  │  - Owns WorklistQueryService                     │  │
│  │  - Manages QThread (worker thread)               │  │
│  │  - Owns QTimer (C-FIND every 5 min)              │  │
│  │  - Owns QTimer (C-ECHO every 30 sec)             │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                       ↕ (signals/slots)
┌─────────────────────────────────────────────────────────┐
│  Worker Thread (QThread)                                │
│  ┌──────────────────────────────────────────────────┐  │
│  │  WorklistQueryService                            │  │
│  │  - Owns DcmSCU (DCMTK DICOM SCU)                 │  │
│  │  - Performs C-FIND operations                    │  │
│  │  - Performs C-ECHO operations                    │  │
│  │  - Parses DICOM datasets                         │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                       ↕ (DICOM network)
┌─────────────────────────────────────────────────────────┐
│  PACS/RIS Server (DICOM SCP)                            │
│  - Modality Worklist SCP                                │
│  - Responds to C-FIND requests                          │
│  - Sends worklist entries                               │
└─────────────────────────────────────────────────────────┘
```

### Step-by-Step Worklist Query Flow

#### Initialization Phase

**1. RIS Connection Initialization**

```cpp
void ApplicationService::initializeRisConnections(ProgressCallback callback) {
    callback("Initializing RIS Connections...", 20);

    // Create worklist repository
    auto worklistRepository = std::make_shared<WorklistRepository>(m_databaseConnectionSetting);

    // Load RIS connection settings
    RisConnectionSetting risStd = m_settingProvider->getRisConnectionSetting();

    // Create modality worklist manager
    m_modalityWorklistManager = new ModalityWorklistManager(
        worklistRepository,
        risStd,
        this  // Qt parent
    );

    // Load worklist profiles from database
    auto profiles = worklistRepository->getProfiles();

    // Set default profile (e.g., "DxWorklist")
    if (!profiles.value.isEmpty()) {
        m_modalityWorklistManager->setActiveProfile(profiles.value.first());
    }

    // Start periodic C-FIND queries
    m_modalityWorklistManager->startWorklistQueryFromRis();

    callback("RIS Connections Ready", 20);
}
```

**2. Query Service Setup**

```cpp
void ModalityWorklistManager::setActiveProfile(const WorklistProfile& profile) {
    m_profile = profile;
    prepareQueryService();
}

void ModalityWorklistManager::prepareQueryService() {
    // Stop existing query service
    stopWorklistQueryFromRis();

    // Load DICOM tags for this profile
    auto tagResult = m_repository->getTagsByProfile(m_profile.Id);

    // Create new query service
    m_queryService = std::make_unique<WorklistQueryService>();
    m_queryService->setSettings(m_risSettings);
    m_queryService->setPresentationContext(m_profile.Context);
    m_queryService->setWorklistTags(tagResult.value);

    // Create worker thread
    m_queryThread = new QThread(this);
    m_queryService->moveToThread(m_queryThread);

    // Wire signals
    connect(m_queryThread, &QThread::started,
            this, &ModalityWorklistManager::performWorklistQuery);
    connect(this, &ModalityWorklistManager::QueryRequested,
            m_queryService.get(), &WorklistQueryService::getWorklistEntries);
    connect(m_queryService.get(), &WorklistQueryService::QueryCompleted,
            this, &ModalityWorklistManager::handleNewQueryResults,
            Qt::QueuedConnection);  // Cross-thread

    // Start thread
    m_queryThread->start();
}
```

#### Timer-Based Query Execution

**3. Start Periodic Queries**

```cpp
void ModalityWorklistManager::startWorklistQueryFromRis() {
    // C-FIND timer (default 5 minutes)
    m_findTimer = new QTimer(this);
    m_findTimer->setInterval(300000);  // 5 minutes in milliseconds
    connect(m_findTimer, &QTimer::timeout,
            this, &ModalityWorklistManager::performWorklistQuery);
    m_findTimer->start();

    // C-ECHO timer (30 seconds)
    m_echoTimer = new QTimer(this);
    m_echoTimer->setInterval(30000);  // 30 seconds
    connect(m_echoTimer, &QTimer::timeout,
            this, &ModalityWorklistManager::performEchoRequest);
    m_echoTimer->start();

    // Perform initial query immediately
    performWorklistQuery();
}
```

**4. Perform Worklist Query (Timer Callback)**

```cpp
void ModalityWorklistManager::performWorklistQuery() {
    if (m_isFindRunning) {
        qDebug() << "[WARN] C-FIND skipped: Previous request still running.";
        return;  // Prevent overlapping requests
    }

    if (m_queryService && m_queryThread && m_queryThread->isRunning()) {
        m_isFindRunning = true;
        emit QueryRequested();  // Signal to worker thread
    }
}
```

#### DCMTK C-FIND Operation (Worker Thread)

**5. Execute C-FIND Request**

```cpp
void WorklistQueryService::getWorklistEntries() {
    QList<WorklistEntry> worklistEntries;

    // Ensure DICOM association is ready
    if (!isConnected()) {
        auto prepareResult = prepareAssociation();
        if (!prepareResult.isSuccess) {
            emit QueryCompleted(worklistEntries);  // Empty result
            return;
        }
    }

    // Build DICOM query dataset from tags
    std::unique_ptr<DcmDataset> query = createWorklistQuery(m_worklistTags);

    // Execute C-FIND request (presentation context ID = 3)
    OFList<QRResponse*> responses;
    OFCondition cond = m_dcmScu->sendFINDRequest(3, query.get(), &responses);

    if (cond.good()) {
        // Parse responses
        for (auto* rsp : responses) {
            if (rsp->m_status == STATUS_Success || rsp->m_status == STATUS_Pending) {
                WorklistEntry entry = parseDatasetToWorklist(rsp->m_dataset, m_worklistTags);
                worklistEntries.append(entry);
            }
            delete rsp;  // Clean up response
        }
    }

    // Emit results (cross-thread signal)
    emit QueryCompleted(worklistEntries);
}
```

**6. Create DICOM Query Dataset**

```cpp
std::unique_ptr<DcmDataset> WorklistQueryService::createWorklistQuery(
    const QList<DicomTag>& tags)
{
    auto dataset = std::make_unique<DcmDataset>();

    for (const auto& tag : tags) {
        // Create DICOM tag identifier
        DcmTagKey tagKey(tag.GroupHex, tag.ElementHex);

        // Handle sequences (nested tags)
        if (tag.PgroupHex != 0 && tag.PelementHex != 0) {
            // Nested tag in sequence (e.g., ScheduledProcedureStepSequence)
            DcmTagKey parentKey(tag.PgroupHex, tag.PelementHex);

            // Get or create parent sequence
            DcmSequenceOfItems* seq = nullptr;
            dataset->findAndGetSequence(parentKey, seq);
            if (!seq) {
                dataset->insertEmptyElement(parentKey);
                dataset->findAndGetSequence(parentKey, seq);
            }

            // Insert tag into sequence item
            DcmItem* item = new DcmItem();
            item->insertEmptyElement(tagKey);
            seq->insert(item);
        } else {
            // Top-level tag
            dataset->insertEmptyElement(tagKey);
        }
    }

    return dataset;
}
```

**7. Parse DICOM Response to WorklistEntry**

```cpp
WorklistEntry WorklistQueryService::parseDatasetToWorklist(
    DcmDataset* dataset,
    const QList<DicomTag>& tags)
{
    WorklistEntry entry;
    entry.Source = Source::RIS;
    entry.Status = ProcedureStepStatus::PENDING;

    // Extract ScheduledProcedureStepSequence if present
    DcmSequenceOfItems* spsSeq = nullptr;
    dataset->findAndGetSequence(DCM_ScheduledProcedureStepSequence, spsSeq);

    for (const auto& tag : tags) {
        DcmTagKey tagKey(tag.GroupHex, tag.ElementHex);
        OFString valueStr;

        // Extract from sequence or top-level
        if (spsSeq && tag.PgroupHex != 0) {
            DcmItem* spsItem = spsSeq->getItem(0);
            spsItem->findAndGetOFString(tagKey, valueStr);
        } else {
            dataset->findAndGetOFString(tagKey, valueStr);
        }

        // Create WorklistAttribute
        WorklistAttribute attr;
        attr.DicomTagId = tag.Id;
        attr.TagValue = QString::fromStdString(valueStr.c_str());
        entry.Attributes.append(attr);
    }

    return entry;
}
```

#### Result Handling (Main Thread)

**8. Handle Query Results**

```cpp
void ModalityWorklistManager::handleNewQueryResults(const QList<WorklistEntry>& entries) {
    // Reset running flag
    m_isFindRunning = false;

    // Process each worklist entry
    for (const auto& entry : entries) {
        // Check if entry already exists
        auto existingResult = m_repository->getWorklistEntry(entry, m_profile);

        if (!existingResult.isSuccess) {
            // New entry: insert into database
            WorklistEntry newEntry = entry;
            newEntry.Source = Source::RIS;
            newEntry.Status = ProcedureStepStatus::PENDING;
            newEntry.ProfileId = m_profile.Id;

            auto createResult = m_repository->createWorklistEntry(newEntry);
            if (createResult.isSuccess) {
                qDebug() << "New worklist entry added:" << newEntry.Id;
            }
        } else {
            // Existing entry: update logic (currently commented out)
            // Can implement update logic here
        }
    }

    // Emit signal to notify UI
    emit WorklistUpdated();
}
```

### DICOM Association Management

**9. Prepare Association**

```cpp
Result<void> WorklistQueryService::prepareAssociation() {
    // Release existing association
    if (m_dcmScu->isConnected()) {
        m_dcmScu->releaseAssociation();
    }

    // Setup connection parameters
    setupTheConnectionParameters();

    // Add presentation contexts
    // C-ECHO
    m_dcmScu->addPresentationContext(
        UID_VerificationSOPClass,           // 1.2.840.10008.1.1
        UID_LittleEndianImplicitTransferSyntax
    );

    // C-FIND (Modality Worklist)
    m_dcmScu->addPresentationContext(
        UID_ModalityWorklistInformationModelFIND,  // 1.2.840.10008.5.1.4.31
        UID_LittleEndianExplicitTransferSyntax
    );

    // Initialize network
    OFCondition initCond = m_dcmScu->initNetwork();
    if (initCond.bad()) {
        return Result<void>::Failure("Failed to initialize DICOM network");
    }

    // Negotiate association
    OFCondition assocCond = m_dcmScu->negotiateAssociation();
    if (assocCond.bad()) {
        return Result<void>::Failure("Failed to negotiate DICOM association");
    }

    // Validate with C-ECHO
    OFCondition echoCond = m_dcmScu->sendECHORequest(0);
    if (echoCond.bad()) {
        return Result<void>::Failure("C-ECHO verification failed");
    }

    return Result<void>::Success();
}
```

**10. Connection Parameters**

```cpp
void WorklistQueryService::setupTheConnectionParameters() {
    m_dcmScu->setAETitle(m_settings->getCallingAETitle());      // Local AE (e.g., "ETREK-MODALITY")
    m_dcmScu->setPeerAETitle(m_settings->getCalledAETitle());   // Remote AE (e.g., "PACS-WORKLIST")
    m_dcmScu->setPeerHostName(m_settings->getHostIP());         // PACS IP (e.g., "192.168.1.100")
    m_dcmScu->setPeerPort(m_settings->getPort());               // DICOM port (e.g., 104)
}
```

### Data Flow Summary

```
[Timer: 5 min] ────────────────┐
                               ▼
        ┌─────────────────────────────────────┐
        │  ModalityWorklistManager            │
        │  (Main Thread)                      │
        └─────────────────┬───────────────────┘
                          │ emit QueryRequested
                          ▼
        ┌─────────────────────────────────────┐
        │  WorklistQueryService               │
        │  (Worker Thread)                    │
        │  1. Prepare association             │
        │  2. Create DICOM query dataset      │
        │  3. Send C-FIND request             │
        │  4. Parse responses                 │
        └─────────────────┬───────────────────┘
                          │ emit QueryCompleted(entries)
                          ▼
        ┌─────────────────────────────────────┐
        │  ModalityWorklistManager            │
        │  (Main Thread)                      │
        │  1. Check for existing entries      │
        │  2. Insert new entries to database  │
        │  3. Emit WorklistUpdated signal     │
        └─────────────────┬───────────────────┘
                          │ WorklistUpdated signal
                          ▼
        ┌─────────────────────────────────────┐
        │  WorkListPageDelegate               │
        │  (Main Thread)                      │
        │  1. Refresh table model             │
        │  2. Update UI                       │
        └─────────────────────────────────────┘
```

---

## 4. Launch Strategies

### Strategy Pattern Implementation

Etrek uses the Strategy pattern to support different application launch modes.

### LaunchMode Enum

```cpp
namespace Etrek::Specification {
    enum class LaunchMode {
        MainApp,        // Full application with authentication
        UserManager,    // User administration only
        SettingManager, // Settings editor only
        Demo,           // Demo/testing mode
        Developer,      // Developer diagnostics
        Unknown
    };

    LaunchMode ParseLaunchMode(const QStringList& args) {
        if (args.contains("usermgr", Qt::CaseInsensitive))
            return LaunchMode::UserManager;
        if (args.contains("settingmgr", Qt::CaseInsensitive))
            return LaunchMode::SettingManager;
        if (args.contains("demo", Qt::CaseInsensitive))
            return LaunchMode::Demo;
        if (args.contains("developer", Qt::CaseInsensitive))
            return LaunchMode::Developer;
        return LaunchMode::MainApp;
    }
}
```

### ILaunchStrategy Interface

```cpp
class ILaunchStrategy {
public:
    virtual ~ILaunchStrategy() = default;
    virtual void launch(ApplicationService* service) = 0;
};
```

### Strategy Factory

```cpp
std::unique_ptr<ILaunchStrategy> ApplicationService::createLaunchStrategy(LaunchMode mode) {
    switch (mode) {
        case LaunchMode::MainApp:
            return std::make_unique<MainAppLaunchStrategy>();
        case LaunchMode::UserManager:
            return std::make_unique<UserManagerLaunchStrategy>();
        case LaunchMode::SettingManager:
            return std::make_unique<SettingManagerLaunchStrategy>();
        case LaunchMode::Demo:
            return std::make_unique<DemoLaunchStrategy>();
        case LaunchMode::Developer:
            return std::make_unique<DeveloperLaunchStrategy>();
        default:
            return std::make_unique<MainAppLaunchStrategy>();
    }
}
```

### MainAppLaunchStrategy Details

See [Application Initialization](#1-application-initialization) section above for complete flow.

**Key Characteristics**:
- Full service initialization (logger, database, auth, RIS, PACS)
- Requires user authentication
- Displays splash screen with progress
- Initializes all modules

### Other Launch Strategies (Stubs)

**DemoLaunchStrategy**:
- Intended for demo/testing without database
- Currently logs "Demo mode launched"
- Future: Mock data, limited functionality

**DeveloperLaunchStrategy**:
- Intended for developer shortcuts and debugging
- Currently stub implementation
- Future: Debug panels, diagnostic tools

**UserManagerLaunchStrategy**:
- Intended for user administration interface
- Currently stub implementation
- Future: CRUD operations on users/roles without full app

**SettingManagerLaunchStrategy**:
- Intended for settings editor
- Currently stub implementation
- Future: Edit configuration files without full app

---

## 5. PACS Integration Workflow

### PACS Node Configuration

#### PacsNode Entity

```cpp
class PacsNode {
public:
    int Id = -1;
    PacsEntityType Type;      // Archive, MPPS, MWL, etc.
    QString HostName;         // DNS name
    QString HostIp;           // IP address (preferred for connection)
    int Port = 0;             // DICOM port (typically 104)
    QString CalledAet;        // Remote AE Title (SCP)
    QString CallingAet;       // Local AE Title (SCU)
    bool IsDefault = false;   // Default node for this type
};
```

#### PacsEntityType Enum

```cpp
enum class PacsEntityType {
    Archive,      // DICOM Archive (C-STORE SCP)
    MPPS,         // Modality Performed Procedure Step
    MWL,          // Modality Worklist (C-FIND SCP)
    PrintScp,     // Print SCP
    CustomEntity  // Custom/user-defined
};
```

### PACS Configuration Workflow

**1. Add PACS Node**

```cpp
PacsNodeRepository pacsRepo(dbConnection);

PacsNode archiveNode;
archiveNode.Type = PacsEntityType::Archive;
archiveNode.HostName = "pacs01.hospital.org";
archiveNode.HostIp = "192.168.1.100";
archiveNode.Port = 104;
archiveNode.CalledAet = "PACS-ARCHIVE";
archiveNode.CallingAet = "ETREK-MODALITY";
archiveNode.IsDefault = true;

auto addResult = pacsRepo.addPacsNode(archiveNode);
```

**2. Retrieve PACS Nodes**

```cpp
auto allNodes = pacsRepo.getPacsNodes();

// Filter by type
QList<PacsNode> archives;
for (const auto& node : allNodes.value) {
    if (node.Type == PacsEntityType::Archive) {
        archives.append(node);
    }
}
```

### Multi-PACS Support Architecture

```
┌─────────────────────────────────────────────────────────┐
│  Etrek Modality                                         │
│  AE Title: "ETREK-MODALITY"                             │
└─────────────────┬───────────────────────────────────────┘
                  │
        ┌─────────┼─────────┬─────────┐
        ▼         ▼         ▼         ▼
┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐
│ Archive  │ │   MWL    │ │   MPPS   │ │  Print   │
│   PACS   │ │   PACS   │ │   PACS   │ │   SCP    │
│   SCP    │ │   SCP    │ │   SCP    │ │          │
└──────────┘ └──────────┘ └──────────┘ └──────────┘
  C-STORE      C-FIND       N-CREATE     N-PRINT
```

### PACS-Worklist Integration

The worklist system acts as the consumer of MWL (Modality Worklist) PACS:

```
┌─────────────────────────────────────────────────────────┐
│  PACS Server (Worklist SCP)                             │
│  - Maintains scheduled procedures                       │
│  - Responds to C-FIND requests                          │
└─────────────────┬───────────────────────────────────────┘
                  │ C-FIND Response (WorklistEntry objects)
                  ▼
┌─────────────────────────────────────────────────────────┐
│  WorklistQueryService                                   │
│  - Periodic C-FIND queries                              │
│  - Parses DICOM datasets                                │
└─────────────────┬───────────────────────────────────────┘
                  │ WorklistEntry objects
                  ▼
┌─────────────────────────────────────────────────────────┐
│  ModalityWorklistManager                                │
│  - Receives worklist entries                            │
│  - Checks for duplicates                                │
└─────────────────┬───────────────────────────────────────┘
                  │ Persist to local database
                  ▼
┌─────────────────────────────────────────────────────────┐
│  WorklistRepository                                     │
│  - Stores worklist entries locally                      │
│  - Enables offline operation                            │
└─────────────────┬───────────────────────────────────────┘
                  │ Display in UI
                  ▼
┌─────────────────────────────────────────────────────────┐
│  WorkListPage (UI)                                      │
│  - User selects worklist entry                          │
│  - Initiates examination                                │
└─────────────────┬───────────────────────────────────────┘
                  │ After examination
                  ▼
┌─────────────────────────────────────────────────────────┐
│  C-STORE to Archive PACS                                │
│  - Send acquired images                                 │
└─────────────────────────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────────┐
│  MPPS Status Update                                     │
│  - Report procedure completion                          │
└─────────────────────────────────────────────────────────┘
```

---

## 6. Examination Workflow

### Workflow Context Pattern

When a user selects a worklist entry to start an examination, Etrek uses the Context Manager to maintain examination state.

### Step-by-Step Examination Flow

**1. User Selects Worklist Entry**

```cpp
// In WorkListPage
void WorkListPage::onTableDoubleClicked(const QModelIndex& index) {
    int entryId = getEntryIdFromRow(index.row());
    emit worklistItemDoubleClicked(entryId);
}
```

**2. WorkListPageDelegate Handles Selection**

```cpp
void WorkListPageDelegate::onWorklistItemDoubleClicked(int entryId) {
    // Retrieve worklist entry
    auto entryResult = repository->getWorklistEntryById(entryId);
    if (!entryResult.isSuccess) return;

    // Create examination context
    auto examContext = std::make_shared<ExaminationContext>(entryResult.value);

    // Set workflow context
    if (auto ctxMgr = contextManager.lock()) {
        ctxMgr->setWorkflowContext("Examination", examContext);
    }

    // Signal to start examination
    emit startExamination(entryId);
}
```

**3. Open View Selection Dialog**

```cpp
// MainWindowDelegate or WorkListPageDelegate
void onStartExamination(int entryId) {
    // Build view selection dialog
    ViewSelectionDialogBuilder builder;
    auto [dialog, delegate] = builder.build(m_params, mainWindow, this);

    // Connect delegate signals
    connect(delegate, &ViewSelectionDialogDelegate::examinationReady,
            this, [this](int procedureId, const QVector<int>& viewIds) {
                // Start examination with selected procedure and views
                startExaminationWithViews(procedureId, viewIds);
            });

    dialog->exec();
}
```

**4. User Selects Procedure and Views**

```cpp
// ViewSelectionDialogDelegate
void ViewSelectionDialogDelegate::onAcceptSelection() {
    int selectedProcedureId = ui->getSelectedProcedureId();
    QVector<int> selectedViewIds = ui->getSelectedViewIds();

    emit examinationReady(selectedProcedureId, selectedViewIds);
}
```

**5. Start Imaging Acquisition**

```cpp
void startExaminationWithViews(int procedureId, const QVector<int>& viewIds) {
    // Retrieve procedure and views
    auto procedureResult = scanProtocolRepository->getProcedureById(procedureId);
    auto viewsResult = scanProtocolRepository->getViewsByIds(viewIds);

    // Create examination page
    ExaminationPageBuilder builder;
    auto [examPage, examDelegate] = builder.build(m_params, nullptr, this);

    // Configure examination
    examDelegate->setProcedure(procedureResult.value);
    examDelegate->setViews(viewsResult.value);

    // Display examination page
    mainWindow->loadPage(examPage);
}
```

### Examination Context

```cpp
class ExaminationContext : public IWorkflowContext {
public:
    ExaminationContext(const WorklistEntry& entry)
        : m_worklistEntry(entry) {}

    WorklistEntry worklistEntry() const { return m_worklistEntry; }
    QString patientName() const { return getAttributeValue("PatientName"); }
    QString patientId() const { return getAttributeValue("PatientID"); }
    QString accessionNumber() const { return getAttributeValue("AccessionNumber"); }
    QString bodyPartExamined() const { return getAttributeValue("BodyPartExamined"); }

private:
    WorklistEntry m_worklistEntry;

    QString getAttributeValue(const QString& tagName) const {
        for (const auto& attr : m_worklistEntry.Attributes) {
            if (attr.TagName == tagName) {
                return attr.TagValue;
            }
        }
        return QString();
    }
};
```

---

## 7. Context Management

### Context Types

Etrek uses two categories of context:

#### Session Context (Persistent)

Created after successful login, persists for entire user session:

```cpp
class SessionContext : public ISessionContext {
public:
    SessionContext(const User& user, const QString& workstation)
        : m_user(user),
          m_signInTime(QDateTime::currentDateTime()),
          m_workstationName(workstation.isEmpty() ? QHostInfo::localHostName() : workstation)
    {}

    User currentUser() const override { return m_user; }
    QString username() const override { return m_user.Username; }
    QString userFullName() const override { return m_user.Name + " " + m_user.Surname; }
    QDateTime signInTime() const override { return m_signInTime; }
    QString workstationName() const override { return m_workstationName; }
    bool isLoggedIn() const override { return m_user.Id != -1; }

private:
    User m_user;
    QDateTime m_signInTime;
    QString m_workstationName;
};
```

#### Workflow Context (Transient)

Created for specific workflows, cleared when workflow completes:

```cpp
class IWorkflowContext {
public:
    virtual ~IWorkflowContext() = default;
    virtual QString workflowType() const = 0;
};

// Example: ExaminationContext
class ExaminationContext : public IWorkflowContext {
    WorklistEntry m_entry;
public:
    QString workflowType() const override { return "Examination"; }
    WorklistEntry worklistEntry() const { return m_entry; }
};
```

### Context Manager

Thread-safe context storage and retrieval:

```cpp
class ContextManager : public IContextManager {
    Q_OBJECT
public:
    // Session context
    void setSessionContext(std::shared_ptr<ISessionContext> context) override {
        QMutexLocker locker(&m_mutex);
        m_sessionContext = context;
        emit sessionContextChanged();
    }

    std::shared_ptr<ISessionContext> sessionContext() const override {
        QMutexLocker locker(&m_mutex);
        return m_sessionContext;
    }

    // Workflow context
    void setWorkflowContext(const QString& key,
                           std::shared_ptr<IWorkflowContext> context) override {
        QMutexLocker locker(&m_mutex);
        m_workflowContexts[key] = context;
        emit workflowContextChanged(key);
    }

    std::shared_ptr<IWorkflowContext> workflowContext(const QString& key) const override {
        QMutexLocker locker(&m_mutex);
        return m_workflowContexts.value(key, nullptr);
    }

signals:
    void sessionContextChanged();
    void workflowContextChanged(const QString& key);

private:
    mutable QMutex m_mutex;
    std::shared_ptr<ISessionContext> m_sessionContext;
    QMap<QString, std::shared_ptr<IWorkflowContext>> m_workflowContexts;
};
```

### Context Usage in Delegates

**Accessing Session Context**:

```cpp
// Via DelegateParameter
WorkListPageDelegate::WorkListPageDelegate(const DelegateParameter& params, ...) {
    if (params.sessionContext) {
        QString currentUser = params.sessionContext->userFullName();
        qDebug() << "Logged in as:" << currentUser;
    }
}

// Via ContextManager
if (auto ctxMgr = params.contextManager.lock()) {
    auto sessionCtx = ctxMgr->sessionContext();
    if (sessionCtx) {
        User user = sessionCtx->currentUser();
    }
}
```

**Setting Workflow Context**:

```cpp
// When starting examination
auto examContext = std::make_shared<ExaminationContext>(worklistEntry);
if (auto ctxMgr = contextManager.lock()) {
    ctxMgr->setWorkflowContext("Examination", examContext);
}
```

**Retrieving Workflow Context**:

```cpp
// In ExaminationPageDelegate
if (auto ctxMgr = params.contextManager.lock()) {
    auto workflowCtx = ctxMgr->workflowContext("Examination");
    if (auto examCtx = std::dynamic_pointer_cast<ExaminationContext>(workflowCtx)) {
        QString patientName = examCtx->patientName();
        QString accessionNo = examCtx->accessionNumber();
    }
}
```

---

## Summary

This comprehensive workflow documentation covers all major application flows:

1. **Application Initialization**: Robust multi-phase startup with progress tracking
2. **Authentication**: Secure login with encrypted passwords and RBAC
3. **DICOM Worklist Query**: Multi-threaded C-FIND operations with periodic updates
4. **Launch Strategies**: Flexible application modes for different use cases
5. **PACS Integration**: Multi-PACS support for Archive, MWL, and MPPS
6. **Examination Workflow**: Context-driven imaging acquisition
7. **Context Management**: Thread-safe session and workflow state management

All workflows follow established patterns:
- **Builder Pattern**: Lazy construction of UI components
- **Model-View-Delegate**: Clear separation of concerns
- **Repository Pattern**: Abstracted data access
- **Strategy Pattern**: Pluggable initialization strategies
- **Observer Pattern**: Qt signals/slots for loose coupling

---

**See Also**:
- [Model-View-Delegate Pattern](03-Model-View-Delegate-Pattern.md)
- [Builder Pattern](04-Builder-Pattern.md)
- [Architecture Overview](01-Architecture-Overview.md)
- [Database Schema](02-Database-Schema.md)
