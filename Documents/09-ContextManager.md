# Context Management System

## Overview

The Context Management system provides a centralized mechanism for tracking and sharing context information across the Etrek application. It enables decoupled communication between different parts of the application without creating tight dependencies.

## Architecture

### Context Types

The system supports two main categories of context:

1. **Session Context** (`ISessionContext`)
   - Persists for the duration of a user session
   - Contains: logged-in user, sign-in time, workstation name, institution info
   - Created after successful authentication

2. **Workflow Context** (`IWorkflowContext`)
   - Transient state for specific workflows
   - Example: `ExaminationContext` for Worklist → Examination flow
   - Created when user initiates a workflow (e.g., double-clicks worklist item)

### Module Structure

```
Common/Include/Context/
├── IContext.h              # Base interface for all contexts
├── ISessionContext.h       # Session context interface
├── IWorkflowContext.h      # Workflow context base interface
├── IExaminationContext.h   # Examination workflow interface
└── IContextManager.h       # Context manager interface

Core/Context/
├── ContextManager.h/.cpp   # Concrete context manager
├── SessionContext.h/.cpp   # Session context implementation
└── ExaminationContext.h/.cpp # Examination context implementation
```

### Dependency Flow

```
Common (interfaces only, no implementation)
    ↑
Core (ContextManager, concrete contexts)
    ↑
Application/Worklist/View (uses context via injection)
```

This ensures no circular dependencies - lower-level modules depend on interfaces in Common, while Core provides implementations. Both Application and Worklist can depend on Core without creating circular dependencies.

## Usage

### 1. Session Context - After Login

```cpp
// In ApplicationService::authenticateUser()
if (authResult.isSuccess && m_contextManager) {
    auto sessionContext = std::make_shared<SessionContext>(
        authResult.value,  // User
        QString()          // Workstation (auto-detected)
    );
    m_contextManager->setSessionContext(sessionContext);
}
```

### 2. Workflow Context - Worklist Selection

```cpp
// In WorkListPageDelegate::onWorklistItemDoubleClicked()
if (auto ctxMgr = contextManager.lock()) {
    auto examContext = std::make_shared<ExaminationContext>(selectedEntry);
    ctxMgr->setWorkflowContext("Examination", examContext);
}
emit startExamination(entryId);
```

### 3. Consuming Context in Delegates

```cpp
// In a delegate that needs examination context
void ExaminationDelegate::initialize(const DelegateParameter& params) {
    // Option 1: Use pre-fetched context from params
    if (auto examCtx = std::dynamic_pointer_cast<IExaminationContext>(params.workflowContext)) {
        QString patientName = examCtx->patientName();
        QString bodyPart = examCtx->bodyPartExamined();
    }

    // Option 2: Access via ContextManager
    if (auto ctxMgr = params.contextManager.lock()) {
        auto ctx = ctxMgr->workflowContext("Examination");
        // ...
    }
}
```

### 4. Builder Pattern Integration

```cpp
// In a builder class
DelegateParameter params;
params.dbConnection = databaseConnection;
params.contextManager = contextManager;
params.sessionContext = contextManager->sessionContext();
params.workflowContext = contextManager->workflowContext("Examination");

auto [widget, delegate] = builder.build(params, parentWidget, parentDelegate);
```

## DelegateParameter Extension

The `DelegateParameter` struct has been extended to carry context:

```cpp
struct DelegateParameter {
    std::shared_ptr<DatabaseConnectionSetting> dbConnection;
    QMap<QString, QWeakPointer<IDelegate>> delegates;

    // Context additions
    std::weak_ptr<IContextManager> contextManager;
    std::shared_ptr<ISessionContext> sessionContext;
    std::shared_ptr<IWorkflowContext> workflowContext;
};
```

## Sequence Diagram: Worklist → Examination

```
User                WorkListPage    WorkListPageDelegate    ContextManager    ExaminationBuilder
  |                      |                   |                    |                  |
  |--double-click------->|                   |                    |                  |
  |                      |--itemDoubleClicked-->|                 |                  |
  |                      |                   |                    |                  |
  |                      |                   |--setWorkflowContext-->|              |
  |                      |                   |<---------------------|              |
  |                      |                   |                    |                  |
  |                      |                   |--startExamination------------------->|
  |                      |                   |                    |                  |
  |                      |                   |                    |<--getWorkflowCtx-|
  |                      |                   |                    |--return ctx----->|
  |                      |                   |                    |                  |
  |                      |                   |                    |   build with ctx |
  |<-----------------Examination Page Opens------------------------|-----------------|
```

## API Reference

### IContextManager

| Method | Description |
|--------|-------------|
| `setSessionContext(ctx)` | Store session context |
| `sessionContext()` | Get session context |
| `clearSessionContext()` | Clear on logout |
| `setWorkflowContext(key, ctx)` | Store workflow context by key |
| `workflowContext(key)` | Get workflow context by key |
| `clearWorkflowContext(key)` | Clear specific workflow |
| `clearAllWorkflowContexts()` | Clear all workflows |
| `hasSessionContext()` | Check if session exists |
| `hasWorkflowContext(key)` | Check if workflow exists |

### ISessionContext

| Method | Description |
|--------|-------------|
| `currentUser()` | Get logged-in user |
| `username()` | Get username string |
| `userFullName()` | Get "Name Surname" |
| `signInTime()` | Get login timestamp |
| `workstationName()` | Get machine name |
| `institution()` | Get institution entity |
| `institutionName()` | Get institution name |
| `isLoggedIn()` | Check login status |

### IExaminationContext

| Method | Description |
|--------|-------------|
| `worklistEntry()` | Get full WorklistEntry |
| `worklistEntryId()` | Get entry ID |
| `patientName()` | Get formatted patient name |
| `patientId()` | Get patient ID |
| `patientBirthDate()` | Get birth date |
| `patientGender()` | Get gender (M/F/O) |
| `accessionNumber()` | Get accession number |
| `studyDescription()` | Get study description |
| `bodyPartExamined()` | Get body part |
| `referringPhysician()` | Get referring physician |
| `attributeValue(tagName)` | Get any DICOM attribute |

## Signals

The `ContextManager` emits signals when context changes:

- `sessionContextChanged()` - Session set/cleared
- `workflowContextChanged(key)` - Workflow set/updated
- `workflowContextCleared(key)` - Workflow cleared

## Thread Safety

`ContextManager` uses `QMutex` for thread-safe access to context data. All public methods are safe to call from any thread.

## Context Audit Trail

**Since**: Version 1.1 (2026-01-24)

The `ContextAuditService` provides comprehensive audit logging for all context changes, supporting compliance and debugging requirements.

### Architecture

```
ContextManager (emits signals)
    ↓ signals: sessionContextChanged(), workflowContextChanged()
ContextAuditService (listens)
    ↓ persists to database
context_audit_log table
```

### Features

1. **Automatic Tracking**: All context changes are automatically audited
   - Session creation (login)
   - Session clearing (logout)
   - Workflow context creation (examination start)
   - Workflow context clearing (examination end)

2. **Detailed Context Data**: Extracts and stores relevant context information as JSON
   - Session: username, user_id, workstation, institution, signin time
   - Workflow: worklist_entry_id, patient info, body part, accession number

3. **Query API**: Retrieve audit history
   ```cpp
   // Get login history for a user
   auto history = auditService->getAuditHistoryForUser(userId, 10);
   
   // Get recent examination workflow events
   auto examHistory = auditService->getRecentAuditHistory(
       ContextType::WORKFLOW, 20);
   ```

4. **Database Schema**:
   ```sql
   CREATE TABLE context_audit_log (
       id INT AUTO_INCREMENT PRIMARY KEY,
       context_type ENUM('SESSION', 'WORKFLOW'),
       context_key VARCHAR(50),  -- NULL for session, workflow name for workflow
       event_type ENUM('CREATED', 'UPDATED', 'CLEARED'),
       user_id INT,
       workstation_name VARCHAR(100),
       details JSON,
       timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
       INDEX (user_id), INDEX (context_type), INDEX (timestamp)
   );
   ```

### Integration

The `ContextAuditService` is initialized in `ApplicationService` after successful authentication:

```cpp
// In ApplicationService::authenticateUser()
if (!m_contextAuditService && m_databaseConnectionSetting) {
    m_contextAuditService = std::make_shared<ContextAuditService>(
        m_databaseConnectionSetting, this);
    m_contextAuditService->connectToContextManager(m_contextManager.get());
}
```

### Use Cases

- **Compliance**: Track user login/logout times and workstation usage
- **Debugging**: Trace when examination contexts were created/cleared
- **Analytics**: Analyze workflow patterns and user activity
- **Audit Reports**: Generate reports on context changes over time

### Migration

Database migration script: `Core/Script/migration_context_audit_log.sql`

Run migration:
```bash
mysql -u root -p EtrekDb < Core/Script/migration_context_audit_log.sql
```

## Future Extensions

The system is designed to be extensible:

1. **New workflow contexts**: Create new classes implementing `IWorkflowContext`
2. **Additional session data**: Extend `SessionContext` with new fields
3. **Context persistence**: Add save/restore for session recovery
4. **Context events**: Subscribe to context changes via signals
5. **Audit retention policies**: Implement automatic cleanup of old audit records
6. **Real-time monitoring**: Subscribe to audit events for live monitoring dashboards

---

## Context Injection Audit

### Delegates WITH Context Injection

| Delegate | File | contextManager param |
|----------|------|---------------------|
| WorkListPageDelegate | Application/Delegate/WorkListPageDelegate.h:46 | `std::weak_ptr<IContextManager>` |
| ExamPageDelegate | Application/Delegate/ExamPageDelegate.h:99 | `std::weak_ptr<IContextManager>` |
| ViewSelectionDialogDelegate | Application/Delegate/ViewSelectionDialogDelegate.h:37 | `std::weak_ptr<IContextManager>` |
| ImageViewerPageDelegate | Application/Delegate/ImageViewerPageDelegate.h:77 | `std::weak_ptr<IContextManager>` |
| MainWindowDelegate | Application/Delegate/MainWindowDelegate.h:20 | Via `DelegateParameter` |
| SystemSettingPageDelegate | Application/Delegate/SystemSettingPageDelegate.h:25 | `std::weak_ptr<IContextManager>` |

### Configuration Delegates (Context via DelegateParameter)

Configuration delegates are child delegates of `SystemSettingPageDelegate` and receive context via `DelegateParameter` from `SystemSettingPageBuilder.cpp`. They access context through the builder pattern rather than direct constructor injection.

| Delegate | Module |
|----------|--------|
| WorkListConfigurationDelegate | Worklist |
| WorkflowConfigurationDelegate | Device |
| ViewConfigurationDelegate | ScanProtocol |
| ProcedureConfigurationDelegate | ScanProtocol |
| TechniqueConfigurationDelegate | ScanProtocol |
| GeneratorConfigurationDelegate | Device |
| DetectorConfigurationDelegate | Device |
| DapConfigurationDelegate | Device |
| CollimatorConfigurationDelegate | Device |
| ConnectionSetupDelegate | Device |
| PacsEntityConfigurationDelegate | Pacs |
| ImageCommentConfigurationDelegate | Dicom |
