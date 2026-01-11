# Doxygen Documentation Standards

**Document Version:** 1.0
**Created:** January 11, 2026
**GitHub Issue:** #159

---

## Overview

This document defines the standardized Doxygen documentation templates for the Etrek codebase. All classes, methods, and significant code elements should follow these conventions to ensure consistent, maintainable documentation that can be automatically generated into HTML/PDF reference materials.

---

## Table of Contents

1. [File Header Template](#1-file-header-template)
2. [Class Documentation](#2-class-documentation)
3. [Method Documentation](#3-method-documentation)
4. [Member Variable Documentation](#4-member-variable-documentation)
5. [Enum Documentation](#5-enum-documentation)
6. [Namespace Documentation](#6-namespace-documentation)
7. [Signal and Slot Documentation (Qt)](#7-signal-and-slot-documentation-qt)
8. [Module/Group Documentation](#8-modulegroup-documentation)
9. [Common Doxygen Tags Reference](#9-common-doxygen-tags-reference)
10. [Examples by Module](#10-examples-by-module)

---

## 1. File Header Template

Every `.h` and `.cpp` file should begin with a file header block:

```cpp
/**
 * @file FileName.h
 * @brief Brief one-line description of the file's purpose.
 *
 * @details Longer description explaining the file's role in the system,
 *          what classes/functions it contains, and any important notes.
 *
 * @author Etrek Development Team
 * @date YYYY-MM-DD
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see RelatedClass.h
 * @see AnotherRelatedFile.h
 */
```

### Example:

```cpp
/**
 * @file WorkListConfigurationDelegate.h
 * @brief Delegate for WorkList configuration settings persistence.
 *
 * @details This delegate handles the connection between the WorkListConfigurationWidget
 *          and the WorklistRepository, ensuring configuration changes are persisted
 *          to the database when the user applies or accepts settings.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see WorkListConfigurationWidget
 * @see WorklistRepository
 * @see IPageAction
 */
```

---

## 2. Class Documentation

### Template:

```cpp
/**
 * @class ClassName
 * @brief Brief one-line description of the class.
 *
 * @details Detailed description of the class purpose, responsibilities,
 *          and how it fits into the overall architecture. Include:
 *          - Design patterns used (Builder, Delegate, Repository, etc.)
 *          - Key responsibilities
 *          - Lifecycle information
 *          - Thread-safety considerations
 *
 * @note Any important notes about usage or limitations.
 *
 * @warning Any critical warnings about misuse.
 *
 * @par Usage Example:
 * @code
 * auto instance = std::make_shared<ClassName>(param1, param2);
 * instance->doSomething();
 * @endcode
 *
 * @see RelatedClass
 * @see IRelatedInterface
 *
 * @ingroup ModuleName
 */
class ClassName
{
    // ...
};
```

### Example - Delegate Class:

```cpp
/**
 * @class DetectorConfigurationDelegate
 * @brief Delegate for detector device configuration persistence.
 *
 * @details Implements the Delegate pattern to handle business logic for
 *          detector configuration. Connects DetectorConfigurationWidget
 *          to DeviceRepository for CRUD operations.
 *
 *          Key responsibilities:
 *          - Receive detector settings from the UI widget
 *          - Validate configuration changes
 *          - Persist changes to database via DeviceRepository
 *          - Handle apply/accept/reject actions from settings dialog
 *
 * @note This class is part of the Model-View-Delegate (MVD) pattern used
 *       throughout the configuration system.
 *
 * @see DetectorConfigurationWidget
 * @see DetectorConfigurationBuilder
 * @see DeviceRepository
 * @see IPageAction
 *
 * @ingroup Device
 */
class DetectorConfigurationDelegate : public QObject, public IDelegate, public IPageAction
{
    // ...
};
```

### Example - Repository Class:

```cpp
/**
 * @class WorklistRepository
 * @brief Repository for worklist-related database operations.
 *
 * @details Implements the Repository pattern for worklist data access.
 *          Provides CRUD operations for worklist entries, tags, and profiles.
 *          Uses Qt SQL with MySQL backend.
 *
 *          Supported operations:
 *          - Query worklist entries with filtering
 *          - Manage worklist field configurations
 *          - CRUD operations for tags and profiles
 *
 * @note Thread-safe for read operations. Write operations should be
 *       synchronized by the caller.
 *
 * @see WorklistEntry
 * @see IWorklistRepository
 *
 * @ingroup Worklist
 */
class WorklistRepository
{
    // ...
};
```

### Example - Strategy Class:

```cpp
/**
 * @class ClinicalExaminationStrategy
 * @brief Strategy implementation for clinical examination mode.
 *
 * @details Implements IExaminationModeStrategy for standard clinical
 *          patient examinations. This strategy enables:
 *          - Full DICOM object creation and persistence
 *          - Worklist status updates
 *          - PACS transmission
 *          - Complete audit trail
 *
 *          This is the primary mode for real patient examinations.
 *
 * @par Lifecycle Hooks:
 * - onExaminationStart(): Initializes study and updates MWL status
 * - onBeforeExposure(): Prepares acquisition parameters
 * - onAfterExposure(): Creates DICOM objects
 * - onImageReceived(): Stores and processes images
 * - onExaminationComplete(): Finalizes study and sends to PACS
 *
 * @see IExaminationModeStrategy
 * @see ExaminationModeStrategyFactory
 * @see DemoExaminationStrategy
 *
 * @ingroup Examination
 */
class ClinicalExaminationStrategy : public IExaminationModeStrategy
{
    // ...
};
```

---

## 3. Method Documentation

### Template:

```cpp
/**
 * @brief Brief description of what the method does.
 *
 * @details Detailed description including algorithm explanation,
 *          side effects, and any important behavior notes.
 *
 * @param[in] paramName Description of input parameter.
 * @param[out] outParam Description of output parameter.
 * @param[in,out] inOutParam Description of in/out parameter.
 *
 * @return Description of return value.
 * @retval specificValue Meaning of specific return value.
 *
 * @throws ExceptionType Description of when this exception is thrown.
 *
 * @pre Preconditions that must be true before calling.
 * @post Postconditions guaranteed after successful return.
 *
 * @note Additional notes.
 * @warning Warnings about potential issues.
 *
 * @par Example:
 * @code
 * auto result = object->methodName(param1, param2);
 * if (result.isSuccess) {
 *     // handle success
 * }
 * @endcode
 *
 * @see relatedMethod()
 */
ReturnType methodName(ParamType paramName);
```

### Example - Simple Method:

```cpp
/**
 * @brief Returns the delegate's unique name identifier.
 *
 * @return QString containing the delegate's name.
 */
QString name() const override;
```

### Example - Repository Method:

```cpp
/**
 * @brief Retrieves all RIS procedure mappings for a specific connection.
 *
 * @details Queries the ris_procedure_mapping table filtered by connection name.
 *          Returns only active mappings by default.
 *
 * @param[in] connectionName The RIS connection name to filter by.
 * @param[in] includeInactive If true, includes inactive mappings in results.
 *
 * @return Result containing vector of RisProcedureMapping entities.
 * @retval Result::isSuccess=true Query succeeded, value contains mappings.
 * @retval Result::isSuccess=false Query failed, message contains error details.
 *
 * @pre Database connection must be open and valid.
 *
 * @see RisProcedureMapping
 * @see findByExternalCode()
 */
Result<QVector<RisProcedureMapping>> getByConnectionName(
    const QString& connectionName,
    bool includeInactive = false);
```

### Example - Apply Method (IPageAction):

```cpp
/**
 * @brief Persists current widget state to the database.
 *
 * @details Called when user clicks "Apply" in settings dialog.
 *          Retrieves current configuration from the widget and
 *          saves to database via repository. Does not close dialog.
 *
 * @pre Widget must be valid and contain current user edits.
 * @post Configuration changes are persisted to database.
 *
 * @note This method is non-transactional. Partial failures may occur
 *       if database errors happen mid-save.
 *
 * @see accept()
 * @see reject()
 */
void apply() override;
```

---

## 4. Member Variable Documentation

### Template (In-line):

```cpp
Type m_variableName; ///< Brief description of the variable.
```

### Template (Block comment for complex members):

```cpp
/**
 * @brief Brief description.
 * @details Longer explanation if needed.
 */
Type m_variableName;
```

### Example:

```cpp
private:
    WorkListConfigurationWidget* m_widget = nullptr; ///< Pointer to the associated UI widget.
    std::shared_ptr<rpo::WorklistRepository> m_repository; ///< Repository for database operations.

    /**
     * @brief Cache of modified mappings pending save.
     * @details Tracks all user modifications since last save.
     *          Cleared after successful apply() call.
     */
    QVector<RisProcedureMapping> m_pendingChanges;
```

---

## 5. Enum Documentation

### Template:

```cpp
/**
 * @enum EnumName
 * @brief Brief description of the enumeration.
 *
 * @details Detailed explanation of the enum's purpose and usage.
 */
enum class EnumName
{
    Value1,  ///< Description of Value1.
    Value2,  ///< Description of Value2.
    Value3   ///< Description of Value3.
};
```

### Example:

```cpp
/**
 * @enum ExaminationMode
 * @brief Defines the operating modes for examinations.
 *
 * @details Each mode determines the examination's behavior regarding
 *          image persistence, DICOM creation, worklist updates, and
 *          PACS transmission.
 *
 * @see IExaminationModeStrategy
 * @see ExaminationModeStrategyFactory
 */
enum class ExaminationMode
{
    CLINICAL,    ///< Standard patient examination with full DICOM workflow.
    DEMO,        ///< Demonstration mode with sample images, no persistence.
    CALIBRATION, ///< Detector/generator calibration mode.
    TEST         ///< QA and device verification mode.
};
```

---

## 6. Namespace Documentation

Document namespaces in a dedicated documentation file or at the top of primary headers:

```cpp
/**
 * @namespace Etrek::Worklist::Delegate
 * @brief Contains delegate classes for worklist-related UI components.
 *
 * @details Delegates in this namespace implement the MVD pattern for
 *          worklist configuration and management UI. Each delegate
 *          connects a widget to its corresponding repository.
 *
 * Key classes:
 * - WorkListConfigurationDelegate
 * - RisProcedureMappingConfigurationDelegate
 */
namespace Etrek::Worklist::Delegate
{
    // ...
}
```

---

## 7. Signal and Slot Documentation (Qt)

### Signals:

```cpp
signals:
    /**
     * @brief Emitted when a new mapping is added by the user.
     * @param mapping The newly created mapping entity.
     */
    void mappingAdded(const RisProcedureMapping& mapping);

    /**
     * @brief Emitted when user deletes a mapping.
     * @param mappingId The ID of the deleted mapping.
     */
    void mappingDeleted(int mappingId);
```

### Slots:

```cpp
private slots:
    /**
     * @brief Handles selection change in the mapping list.
     * @param current The newly selected item.
     * @param previous The previously selected item.
     * @details Updates the detail panel to show the selected mapping's properties.
     */
    void onMappingSelectionChanged(const QItemSelection& current, const QItemSelection& previous);

    /**
     * @brief Handles Add button click.
     * @details Creates a new mapping entry with default values and adds it to the list.
     */
    void onAddClicked();
```

---

## 8. Module/Group Documentation

Create module documentation in a dedicated `.dox` file or in a primary header:

```cpp
/**
 * @defgroup Device Device Module
 * @brief Device configuration and management components.
 *
 * @details The Device module handles configuration for all medical imaging
 *          hardware devices including:
 *          - X-ray generators
 *          - Digital detectors
 *          - Collimators
 *          - DAP meters
 *
 *          Each device type has a corresponding configuration delegate
 *          that connects the UI to the DeviceRepository.
 *
 * @{
 */

// Classes documented with @ingroup Device will appear here

/** @} */ // end of Device group
```

---

## 9. Common Doxygen Tags Reference

| Tag | Purpose | Example |
|-----|---------|---------|
| `@brief` | Short description | `@brief Saves configuration.` |
| `@details` | Extended description | `@details Longer explanation...` |
| `@param[in]` | Input parameter | `@param[in] id The entity ID.` |
| `@param[out]` | Output parameter | `@param[out] result The result.` |
| `@param[in,out]` | In/out parameter | `@param[in,out] buffer Data buffer.` |
| `@return` | Return value | `@return True on success.` |
| `@retval` | Specific return value | `@retval nullptr If not found.` |
| `@throws` | Exception thrown | `@throws std::runtime_error On failure.` |
| `@pre` | Precondition | `@pre Database must be open.` |
| `@post` | Postcondition | `@post State is updated.` |
| `@note` | Additional note | `@note Thread-safe.` |
| `@warning` | Warning | `@warning May block.` |
| `@deprecated` | Deprecated item | `@deprecated Use newMethod() instead.` |
| `@see` | Cross-reference | `@see RelatedClass` |
| `@code` | Code example start | `@code auto x = foo(); @endcode` |
| `@ingroup` | Group membership | `@ingroup Device` |
| `@file` | File documentation | `@file FileName.h` |
| `@class` | Class documentation | `@class ClassName` |
| `@enum` | Enum documentation | `@enum EnumName` |
| `@namespace` | Namespace docs | `@namespace Etrek::Core` |

---

## 10. Examples by Module

### Application Module

```cpp
/**
 * @file ApplicationService.h
 * @brief Central application orchestration service.
 * @ingroup Application
 */

/**
 * @class ApplicationService
 * @brief Orchestrates application initialization and lifecycle.
 *
 * @details Singleton service that coordinates:
 *          - Logger initialization
 *          - Database connection setup
 *          - Authentication service creation
 *          - Device initialization
 *          - Main window construction
 *
 * @par Initialization Order:
 * 1. Logger (spdlog)
 * 2. Settings (SettingProvider)
 * 3. Database (Qt SQL/MySQL)
 * 4. Authentication
 * 5. RIS connections
 * 6. Devices
 * 7. Main window
 *
 * @see LaunchMode
 * @see ILaunchStrategy
 *
 * @ingroup Application
 */
class ApplicationService
{
    // ...
};
```

### Core Module

```cpp
/**
 * @class AuthenticationRepository
 * @brief Repository for user authentication data access.
 *
 * @details Handles user credential verification, role retrieval,
 *          and password management. Uses CryptoManager for
 *          password hashing.
 *
 * @see User
 * @see Role
 * @see CryptoManager
 *
 * @ingroup Core
 */
class AuthenticationRepository
{
public:
    /**
     * @brief Authenticates a user with username and password.
     *
     * @param[in] username The user's login name.
     * @param[in] password The user's plaintext password.
     *
     * @return Optional containing User if authentication succeeds.
     * @retval std::nullopt If authentication fails.
     *
     * @note Password is hashed before comparison.
     */
    std::optional<User> authenticateUser(
        const QString& username,
        const QString& password);
};
```

### View Module

```cpp
/**
 * @class RisProcedureMappingConfigurationWidget
 * @brief Widget for managing RIS procedure code mappings.
 *
 * @details Provides a master-detail interface for viewing and editing
 *          RIS procedure code to internal view mappings. Features:
 *          - Connection filter dropdown
 *          - Mapping list with sorting
 *          - Detail editor panel
 *          - Add/Delete/Update buttons
 *
 * @par UI Layout:
 * ```
 * +--------------------------------------------------+
 * | [Connection Filter v]                            |
 * +------------------+-------------------------------+
 * | Mapping List     | Detail Editor                 |
 * | - Item 1         | External Code: [____]         |
 * | - Item 2         | Coding Scheme: [____]         |
 * | - Item 3 (sel)   | Internal View: [____]         |
 * |                  | Notes: [__________]           |
 * |                  | [Add] [Delete] [Update]       |
 * +------------------+-------------------------------+
 * ```
 *
 * @see RisProcedureMappingConfigurationDelegate
 * @see RisProcedureMapping
 *
 * @ingroup View
 */
class RisProcedureMappingConfigurationWidget : public QWidget
{
    // ...
};
```

---

## Documentation Generation

### Doxyfile Configuration

Key settings for Etrek Doxyfile:

```ini
PROJECT_NAME           = "Etrek Medical Imaging"
PROJECT_NUMBER         = 1.0
OUTPUT_DIRECTORY       = ./docs/api
INPUT                  = ./Application ./Core ./View ./Device ./Dicom ./Pacs ./Worklist ./ScanProtocol ./Common
FILE_PATTERNS          = *.h *.cpp
RECURSIVE              = YES
EXTRACT_ALL            = NO
EXTRACT_PRIVATE        = YES
EXTRACT_STATIC         = YES
GENERATE_HTML          = YES
GENERATE_LATEX         = NO
USE_MDFILE_AS_MAINPAGE = ./README.md
```

### Generation Command

```bash
# From project root
doxygen Doxyfile
```

---

## Checklist for Documentation

When documenting a class, ensure:

- [ ] File header with @file, @brief, @author, @date
- [ ] Class documentation with @class, @brief, @details
- [ ] All public methods documented with @brief, @param, @return
- [ ] All signals and slots documented
- [ ] Member variables have inline documentation
- [ ] Cross-references added with @see
- [ ] Module group assigned with @ingroup
- [ ] Example code provided for complex usage

---

*Document maintained by Etrek Development Team*
*Last updated: January 11, 2026*
