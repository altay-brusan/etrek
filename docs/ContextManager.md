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

Application/Context/
├── ContextManager.h/.cpp   # Concrete context manager
├── SessionContext.h/.cpp   # Session context implementation
└── ExaminationContext.h/.cpp # Examination context implementation
```

### Dependency Flow

```
Common (interfaces only, no implementation)
    ↑
Application (ContextManager, concrete contexts)
    ↑
Worklist/View (uses context via injection)
```

This ensures no circular dependencies - lower-level modules depend on interfaces in Common, while Application provides implementations.

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

## Future Extensions

The system is designed to be extensible:

1. **New workflow contexts**: Create new classes implementing `IWorkflowContext`
2. **Additional session data**: Extend `SessionContext` with new fields
3. **Context persistence**: Add save/restore for session recovery
4. **Context events**: Subscribe to context changes via signals
