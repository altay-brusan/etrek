# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Configure
```bash
# Debug build
cmake --preset Qt-Debug

# Release build
cmake --preset Qt-Release
```

### Build
```bash
# Build debug
cmake --build out/build/debug

# Build release
cmake --build out/build/release
```

### Output Structure
- Executables: `out/build/[debug|release]/`
- Libraries (.lib): `out/build/[debug|release]/lib/`
- DLLs: `out/build/[debug|release]/bin/`

## Architecture Overview

Etrek is a medical imaging application built with C++17, Qt 6.5.3, and DCMTK. The architecture follows a layered service-oriented design with clear separation between UI, business logic, and data access.

### Module Dependency Hierarchy

```
Executable (main.cpp)
    └─> Application (high-level orchestration)
        ├─> View (Qt UI components: Pages, Widgets, Dialogs)
        ├─> Core (logging, security, data models, repositories)
        ├─> Worklist (DICOM worklist connectivity)
        ├─> Device (equipment configuration)
        ├─> Dicom (DICOM operations)
        ├─> Pacs (PACS node management)
        └─> ScanProtocol (scan protocol definitions)
            └─> Common (shared interfaces and entity headers)
```

**Key principle**: Lower-level modules (Core, View) do not depend on higher-level modules (Application). Common is an interface-only library providing shared types and contracts.

### Common Module Pattern

Common is an INTERFACE library (`Common/Include/`) containing:
- Entity definitions shared across modules (User, Role, WorklistEntry, etc.)
- Interface contracts (IDelegate, IPageAction, IWidgetDelegateBuilder)
- Repository interfaces (IWorklistRepository, IWorklistFieldConfigurationRepository)
- Specification types (LaunchMode, Result, DateTimeSpan)

All modules include from Common via: `${COMMON_INCLUDE_DIR}/[Module]/[Category]/[Header].h`

### Application Layer Pattern

The Application module uses the **Strategy Pattern** for different launch modes:
- `ILaunchStrategy` - Base interface
- `MainAppLaunchStrategy` - Full application with authentication
- `DemoLaunchStrategy` - Demo mode
- `DeveloperLaunchStrategy` - Developer shortcuts
- `SettingManagerLaunchStrategy` - Settings editor only
- `UserManagerLaunchStrategy` - User management only

Launch strategies are selected based on command-line arguments parsed by `ParseLaunchMode()`.

### Delegate Pattern

The codebase extensively uses the **Builder + Delegate Pattern** for UI components:
- **Builder** classes construct UI components and wire dependencies
- **Delegate** classes handle business logic and interactions

Examples:
- `MainWindowBuilder` + `MainWindowDelegate`
- `SystemSettingPageBuilder` + `SystemSettingPageDelegate`
- `WorkListConfigurationBuilder` + `WorkListConfigurationDelegate`

This pattern separates construction complexity from runtime behavior and provides clear extension points.

### Service Initialization

`ApplicationService` is the central orchestration point. It initializes services in a specific order:
1. Logger setup (spdlog)
2. Settings loading (SettingProvider)
3. Database initialization (Qt SQL with MySQL driver)
4. Authentication service
5. RIS connections (ModalityWorklistManager)
6. Device initialization
7. Main window construction

### Repository Pattern

Data access follows the **Repository Pattern**:
- Repositories in Core, Device, Dicom, Pacs, Worklist modules
- Interface definitions in Common for cross-module access
- Qt SQL-based implementations (MySQL backend)

Examples: `AuthenticationRepository`, `WorklistRepository`, `DeviceRepository`, `PacsNodeRepository`

### Namespace Conventions

**Current state**: The codebase is undergoing namespace standardization and header hygiene improvements. You may encounter:
- Inconsistent namespace aliases (some files use aliases, others don't)
- Mixed `using namespace` declarations
- Forward declarations that could be improved

**Target pattern** (use this for new code):
```cpp
namespace Etrek::Module::SubModule
{
    // Namespace aliases at top of file
    namespace ent = Etrek::Core::Data::Entity;
    namespace rpo = Etrek::Core::Repository;

    // Implementation
}
```

Common namespace aliases used in the codebase:
- `lg` = Etrek::Core::Log
- `set` = Etrek::Core::Setting
- `mdl` = Etrek::Core::Data::Model
- `ent` = Etrek::Core::Data::Entity
- `rpo` = Etrek::Core::Repository
- `sec` = Etrek::Core::Security
- `dlg` = Application/View Delegates
- `cnc` = Etrek::Worklist::Connectivity
- `spc` = Etrek::Specification

## Key Technologies

### Third-Party Libraries
- **Qt 6.5.3**: UI framework, requires MSVC 2019 x64 build
- **DCMTK 3.6.9**: DICOM protocol implementation
- **spdlog**: Structured logging (header-only)
- **OpenSSL**: Cryptography (libcrypto, libssl)
- **MySQL**: Database backend via Qt SQL driver (QMYSQL 6.5.3)

### Qt-Specific Features
- `CMAKE_AUTOMOC`, `CMAKE_AUTOUIC`, `CMAKE_AUTORCC` enabled
- UI forms in `.ui` files alongside source
- Resource files via `resources.qrc`
- Single-instance application using `QLockFile`

## Development Setup

### Required Environment
1. **Qt 6.5.3 with MSVC 2019 x64**: Set `QTDIR` environment variable (default: `C:/Qt/6.5.3/msvc2019_64`)
2. **MySQL Driver**: Copy `ThirdPartyLibraries/Windows/sqldrivers/*` to `%QTDIR%/plugins/sqldrivers`
3. **Fonts**: Ensure required application fonts are installed
4. **Git Large Files**: If clone/push fails, increase buffer: `git config --global http.postBuffer 524288000`

### Build Configuration
- CMake 3.19+ required
- Ninja generator (via Qt presets)
- C++17 standard
- Windows-specific: `WINDOWS_EXPORT_ALL_SYMBOLS` enabled for all shared libraries

### Output Directory Structure
CMake configures:
- Language files: `out/build/[config]/lang/` (auto-copied from `Core/Globalization/Lan/`)
- Settings files: `out/build/[config]/setting/` (auto-copied from `Core/Setting/Resources/`)
- Log directory: `out/build/[config]/log/` (created at configure time)

## Code Organization Patterns

### Module Structure
Each functional module (Application, Core, View, etc.) follows this pattern:
```
Module/
├── CMakeLists.txt          # Builds SHARED library
├── [SubModule1]/           # Grouped by responsibility
│   ├── *.h                 # Headers
│   └── *.cpp               # Implementation
├── [SubModule2]/
└── resources.qrc           # Qt resources (if needed)
```

### Header Organization
- **Public interfaces**: Defined in `Common/Include/`
- **Implementation headers**: In module source directories
- **Include directories**: Modules expose specific subdirectories, not root

### View Module Specifics
- Flat structure: all UI components in `Page/`, `Widget/`, or `Dialog/`
- `.ui` files alongside `.h`/`.cpp` files
- `CMAKE_AUTOUIC_SEARCH_PATHS` set for Qt Designer forms

## Important Implementation Notes

### Signal Handling
The application uses RAII signal guards (`SignalGuard` class) for SIGINT/SIGTERM. Current implementation calls `std::_Exit()` for immediate termination. If graceful shutdown is needed, see commented alternative pattern in `main.cpp:36-56`.

### Logger Lifecycle
Logger is initialized in `ApplicationService` and shut down in `main.cpp` via `aboutToQuit` signal. Always use `LoggerProvider::Instance()` for access.

### Authentication Flow
1. `AuthenticationService` initialized by `ApplicationService`
2. Launch strategy determines if authentication is required
3. User authentication via `authenticateUser()` returns `std::optional<User>`
4. Encrypted credentials managed by `CryptoManager`

### Launch Modes
Parse arguments with `Etrek::Specification::ParseLaunchMode()`:
- Default: Full application
- `--demo`: Demo mode
- `--dev`: Developer mode
- `--settings`: Settings manager only
- `--users`: User manager only

### DICOM Worklist Integration
- `ModalityWorklistManager` coordinates multiple RIS connections
- `WorklistQueryService` handles DCMTK C-FIND operations
- Configuration stored in `RisConnectionSetting` entities
- Mapping profiles convert DICOM tags to application entities

### Database Schema & Migrations
- MySQL database backend via Qt SQL driver
- Schema defined in `Core/Script/setup_database.sql`
- Migration scripts located in `Core/Script/` directory
- Migrations are idempotent and transaction-safe
- See `Core/Script/README.md` for migration workflow

**Recent Schema Changes**:
- **Admission ID Support** (Issue #29): Added `admission_id` column to `studies` table
  - DICOM tag (0038,0010) mapping
  - Non-unique index for query performance
  - Migration: `migration_add_admission_id.sql`
  - Backfill: `backfill_admission_id.sql`
  - See `docs/admission_id_schema.md` for details

## Current Refactoring Efforts

The branch `refactor/namespace-standardization-and-header-hygiene` is actively improving:
- Namespace consistency across modules
- Reducing unnecessary includes
- Standardizing forward declarations
- Improving header self-containment

When modifying existing code, align with these improvements where possible.
