# Etrek AI Coding Instructions

## Project Overview
Etrek is a medical imaging Digital Radiography (DR) workstation built with C++17, Qt 6.5.3, DCMTK 3.6.9, and VTK 9.5. It integrates with hospital RIS systems via DICOM Modality Worklist, controls X-ray hardware, and provides medical image viewing with PACS connectivity.

## Build & Run

### Configure and Build
```powershell
# Configure (choose preset)
cmake --preset Qt-Debug

# Build
cmake --build out/build/debug
```

### Run Application
```powershell
# Standard launch
out/build/debug/EtrekApp.exe

# Launch modes
out/build/debug/EtrekApp.exe --demo      # Demo mode (no auth)
out/build/debug/EtrekApp.exe --dev       # Developer shortcuts
out/build/debug/EtrekApp.exe --settings  # Settings manager only
out/build/debug/EtrekApp.exe --users     # User manager only
```

### Tests
Tests are **separate from main build** (different CMake project):
```powershell
cd Test
cmake -B build
cmake --build build
build/tst_WorklistRepository.exe -v2  # Run with verbose output
```

## Architecture Essentials

### Module Dependency Hierarchy
**Critical**: Lower modules cannot depend on higher modules. Respect this order:
```
Executable → Application → [View, Core, Worklist, Device, Dicom, Pacs, ScanProtocol, ImageViewer]
                                 ↓
                             Common (interfaces only)
```

**Common module** (`Common/Include/`) is an INTERFACE library containing:
- Entity definitions (User, WorklistEntry, Device, etc.)
- Interface contracts (`IDelegate`, `IPageAction`, `IContextManager`)
- Repository interfaces
- Include from Common: `${COMMON_INCLUDE_DIR}/[Module]/[Category]/[Header].h`

### Builder + Delegate Pattern (MVD)
UI follows Model-View-Delegate, not MVC. Every UI component has a Builder and Delegate:

**Builder** constructs UI and wires dependencies:
```cpp
// Example: MainWindowBuilder
auto [widget, delegate] = builder.build(params, parentWidget, parentDelegate);
```

**Delegate** handles business logic and Qt signals:
- Implements `IDelegate` (required) for identification and dependency injection
- May implement `IPageAction` for configuration pages (apply/accept/reject)
- Receives DelegateParameter with database connection and context manager

**Example locations**: `Application/Builder/MainWindowBuilder.h`, `Application/Delegate/MainWindowDelegate.h`

### Context Management
Two context types managed by `IContextManager`:

1. **SessionContext**: User session info, set after login
2. **WorkflowContext**: Transient workflow state (e.g., "Examination" context when user selects worklist item)

Access context via `DelegateParameter`:
```cpp
void MyDelegate::initialize(const DelegateParameter& params) {
    if (auto ctxMgr = params.contextManager.lock()) {
        auto examCtx = std::dynamic_pointer_cast<IExaminationContext>(
            ctxMgr->workflowContext("Examination")
        );
    }
}
```

### Launch Strategy Pattern
`ApplicationService` selects strategy based on command-line args:
- `MainAppLaunchStrategy` - Full application with authentication
- `DemoLaunchStrategy`, `DeveloperLaunchStrategy` - Special modes
- `SettingManagerLaunchStrategy`, `UserManagerLaunchStrategy` - Standalone tools

Parsed via `Etrek::Specification::ParseLaunchMode()`.

## Code Conventions

### Namespace Style (Target Pattern for New Code)
```cpp
namespace Etrek::Module::SubModule
{
    // Namespace aliases at top of file (only in .cpp, not .h)
    namespace lg = Etrek::Core::Log;
    namespace ent = Etrek::Core::Data::Entity;
    namespace rpo = Etrek::Core::Repository;
    
    // Implementation...
}
```

**Common aliases**:
- `lg` = Etrek::Core::Log
- `set` = Etrek::Core::Setting
- `mdl` = Etrek::Core::Data::Model
- `ent` = Etrek::Core::Data::Entity
- `rpo` = Etrek::Core::Repository
- `sec` = Etrek::Core::Security
- `cnc` = Etrek::Worklist::Connectivity
- `spc` = Etrek::Specification

**Note**: Codebase is undergoing namespace standardization (branch: `refactor/namespace-standardization-and-header-hygiene`). Some files have inconsistent patterns - use above for new code.

### Header Organization
- Headers include minimal dependencies
- Use forward declarations in headers
- Includes in .cpp files only where needed
- Qt MOC features require `Q_OBJECT` macro in headers

### Repository Pattern
Data access via repositories with interfaces in Common:
- `AuthenticationRepository` - User authentication
- `WorklistRepository` - DICOM worklist entries
- `DeviceRepository` - Hardware configuration
- All use Qt SQL (MySQL backend via QMYSQL driver)

## Configuration & Settings

### Settings.json Location
- **Source**: `Core/Setting/Resources/Settings.json`
- **Runtime**: `out/build/[config]/setting/Settings.json` (auto-copied by CMake)

Contains:
- `FileLogger` - Log directory, file size, rotation
- `DatabaseConnection` - MySQL connection (localhost:3306 by default)
- `ModalityWorklistConnection[]` - Multiple RIS connections with DICOM parameters

**Important**: Multiple MWL connections supported. Each has independent AE titles, mappings, and protocols.

### Runtime Configuration
Additional settings in MySQL `environment_settings` table. Database schema in [Documents/02-Database-Schema.md](Documents/02-Database-Schema.md).

## Key Technologies

### Qt 6.5.3 Specifics
- `CMAKE_AUTOMOC`, `CMAKE_AUTOUIC`, `CMAKE_AUTORCC` enabled
- `.ui` files alongside `.h/.cpp` files (no separate ui directory)
- Resource files: `resources.qrc` in module roots
- Single-instance application via `QLockFile` (see `Executable/main.cpp`)

### Third-Party Libraries
Located in `ThirdPartyLibraries/`:
- **DCMTK 3.6.9**: DICOM operations in `Worklist/Connectivity/WorklistQueryService`
- **VTK 9.5**: Medical image rendering in `ImageViewer/Rendering/`
- **spdlog**: Logging via `LoggerProvider::Instance()`
- **OpenSSL**: Password encryption in `Core/Security/CryptoManager`

MySQL driver: Copy `ThirdPartyLibraries/Windows/sqldrivers/*` to `%QTDIR%/plugins/sqldrivers` during setup.

## Common Tasks

### Adding a New Configuration Page
1. Create interface in `Common/Include/` if cross-module access needed
2. Create Widget/Page in `View/Page/` with `.ui` file
3. Create Delegate in `Application/Delegate/` implementing `IDelegate` + `IPageAction`
4. Create Builder in `Application/Builder/`
5. Wire in parent delegate's `attachDelegates()` method

### DICOM Worklist Integration
- Multiple RIS connections managed by `ModalityWorklistManager`
- Query via `WorklistQueryService::performQuery()` using DCMTK C-FIND
- Mapping profiles convert DICOM tags to application entities (see `Worklist/Mapping/`)
- Configuration in `Settings.json` → ModalityWorklistConnection array

### Entity Status Tracking
Workflow states managed by `EntityStatusService`:
- SCHEDULED → IN_PROGRESS (on double-click) → COMPLETED/CANCELLED
- Tracks user, timestamp, audit trail
- See Epic #2 in `docs/EPIC_STATUS_SUMMARY.md`

## Critical Notes

- **Logger lifecycle**: Initialize in `ApplicationService`, shutdown via `aboutToQuit` signal in `main.cpp`
- **Signal handling**: Uses RAII `SignalGuard` for SIGINT/SIGTERM. Currently calls `std::_Exit()` for immediate termination (see `main.cpp:36-56` for graceful shutdown alternative)
- **Qt MOC**: Always add `Q_OBJECT` macro to classes using signals/slots
- **CMake output**: Executables in build root, libraries in `lib/`, DLLs in `bin/`
- **Git LFS**: If clone/push fails, increase buffer: `git config --global http.postBuffer 524288000`

## Documentation
- Architecture: `Documents/01-Architecture-Overview.md` (1437 lines, 4+1 View Model)
- Database schema: `Documents/02-Database-Schema.md`
- MVD Pattern: `Documents/03-Model-View-Delegate-Pattern.md`
- Settings: `Documents/05-Settings-Configuration.md`
- Context: `Documents/09-ContextManager.md`
- Epic status: `docs/EPIC_STATUS_SUMMARY.md` (7/8 complete)

---
*Generated for AI coding agents. For human-readable docs, see Documents/ directory.*
