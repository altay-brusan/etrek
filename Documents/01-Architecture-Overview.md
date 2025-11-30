# E-TREK Project Architecture Overview

## Project Structure

E-TREK is a medical imaging Digital Radiography (DR) system built with Qt 6.5.3 and C++17, organized into modular components:

```
etrek/
├── Executable/          # Application entry point
├── Application/         # Business logic & orchestration
├── Core/                # Foundation services & utilities
├── View/                # User interface components
├── Common/              # Shared interfaces & specifications
├── Worklist/            # DICOM Modality Worklist (MWL) management
├── Device/              # X-ray hardware control (generators, detectors, tubes)
├── Dicom/               # DICOM protocol implementation
├── Pacs/                # Picture Archiving & Communication System
├── ScanProtocol/        # Imaging procedures & technique parameters
├── ImageViewer/         # Image visualization (VTK-based)
└── ThirdPartyLibraries/ # External dependencies
```

## Architectural Patterns

### Builder/Delegate Pattern
- **Builders** construct delegates with dependencies from `DelegateParameter`
- **Delegates** handle business logic and coordinate between repositories/services
- **Separation of concerns**: Construction logic separate from business logic

### Dependency Injection
- `DelegateParameter` struct passes database connection and context to builders
- Each builder creates its own repository instances
- Delegates receive constructed repositories (not raw database connections)

### Repository Pattern
- Database access abstracted through repository classes
- One repository per domain entity (Patient, Study, Device, etc.)
- Repositories handle SQL operations and transaction management

## Application Startup Flow

### 1. Executable (main.cpp)
Entry point that:
- Enables memory leak detection (Windows debug builds)
- Creates `QApplication` instance
- Implements single-instance check using `QLockFile`
- Sets up OS signal handlers (SIGINT, SIGTERM)
- Parses launch mode from command-line arguments
- Creates and initializes `ApplicationService`
- Handles fatal errors with user-friendly messages

**Launch Modes:**
```
--user-manager     # User account management
--setting-manager  # System settings configuration  
--demo             # Demo/testing mode
--developer        # Developer diagnostic mode
(default)          # Normal operation mode
```

### 2. Application Package

#### ApplicationService (Orchestrator)
Central service that:
- Selects appropriate launch strategy based on mode
- Initializes core services in correct order
- Registers Qt meta-types for signal/slot communication
- Manages application lifecycle

**Initialization Sequence:**
```cpp
1. Register Qt meta-types (User, Role, etc.)
2. Create launch strategy based on mode
3. Strategy executes initialization:
   a. Configure logging system
   b. Load settings (database, RIS, file paths)
   c. Establish database connection
   d. Initialize authentication service
   e. Create context managers (session, workflow)
   f. Build main window delegate
   g. Show main window
```

#### Launch Strategies
Each strategy implements `ILaunchStrategy` interface:

**MainAppLaunchStrategy** (Default):
- Full application initialization
- Database connection required
- Authentication required
- All modules loaded

**UserManagerLaunchStrategy**:
- Minimal initialization
- User account CRUD operations
- Password management
- Role assignment

**SettingManagerLaunchStrategy**:
- System configuration interface
- Database settings
- Device configurations
- PACS/RIS connections

**DemoLaunchStrategy**:
- Testing/demo mode
- Mock data
- Limited functionality

**DeveloperLaunchStrategy**:
- Diagnostic tools
- Debug interfaces
- System monitoring

### 3. Core Package

Provides foundational services used by all modules:

#### Logging System
- **LoggerProvider**: Singleton managing logger instances
- **AppLogger**: File-based logging (spdlog backend)
- **AppLoggerFactory**: Creates configured logger instances
- Supports multiple log files per component
- Automatic log rotation and cleanup

#### Security
- **CryptoManager**: Password hashing (bcrypt-style)
- **AuthenticationService**: User login/logout
- **AuthenticationRepository**: User credential storage
- Session token management

#### Settings Management
- **SettingProvider**: Centralized configuration
- Settings stored in JSON files
- Database connection parameters
- File paths and system preferences
- Device configurations

#### Globalization
- **TranslationProvider**: Multi-language support
- Loads Qt translation files (.qm)
- Runtime language switching
- Fallback to English

#### Context Management
- **ContextManager**: Application state coordination
- **SessionContext**: User session data (logged-in user, roles)
- **WorkflowContext**: Current imaging workflow state
- Thread-safe state access

#### Data Models
Domain entities and specifications:
- **Patient**: Patient demographics
- **Study/Series/Image**: DICOM hierarchy
- **Device**: X-ray equipment metadata
- **Procedure/View**: Imaging protocols

## Database Design

See [02-Database-Schema.md](./02-Database-Schema.md) for detailed schema documentation.

## Key Design Principles

### 1. Single Responsibility
Each module has one well-defined purpose:
- **Core**: Foundation services only
- **Application**: Business logic orchestration only
- **View**: UI rendering only
- **Device**: Hardware control only

### 2. Dependency Direction
```
View → Application → Core
  ↓         ↓          ↓
Common (Interfaces)
```
- Lower layers don't depend on upper layers
- Dependencies point inward toward Core
- Common package provides shared abstractions

### 3. Memory Safety
- Smart pointers (`std::shared_ptr`, `std::unique_ptr`) for heap objects
- Qt parent-child ownership for QObject-derived classes
- RAII for resource management
- No raw `new`/`delete` in business logic

### 4. Qt Best Practices
- Signals/slots for inter-component communication
- Qt's meta-object system for type registration
- Event loop for asynchronous operations
- Qt containers where appropriate (QString, QVector)

## Third-Party Dependencies

- **Qt 6.5.3**: Application framework (Core, Widgets, Network, SQL, OpenGL)
- **VTK 9.5**: Medical image visualization
- **DCMTK**: DICOM protocol implementation
- **OpenSSL**: Cryptography and secure connections
- **spdlog**: High-performance logging (header-only)
- **MySQL**: Relational database

All dependencies organized in `ThirdPartyLibraries/` with centralized CMake configuration.

## Build System

CMake-based build:
- Modular structure (each package is a shared library)
- Automatic DLL copying for Windows
- Precompiled headers for faster builds
- Debug and Release configurations

---

**Next**: See [02-Database-Schema.md](./02-Database-Schema.md) for database design documentation.
