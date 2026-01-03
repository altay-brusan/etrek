# Settings and Configuration Guide

This document describes the settings and configuration system used in the Etrek application.

## Overview

Etrek uses a two-tier configuration system:
1. **JSON File Settings** - Loaded at application startup from `Settings.json`
2. **Database Settings** - Stored in the `environment_settings` table for runtime configuration

## JSON Configuration File

### Location
- **Source:** `Core/Setting/Resources/Settings.json`
- **Runtime:** `out/build/[config]/setting/Settings.json` (auto-copied during build)

### Structure

```json
{
  "FileLogger": {
    "LogDirectory": "./log",
    "FileSizeMB": 10,
    "MaxFileCount": 5
  },
  "DatabaseConnection": {
    "HostName": "localhost",
    "DatabaseName": "EtrekDb",
    "UserName": "root",
    "Password": "admin",
    "IsPasswordEncrypted": false
  },
  "ModalityWorklistConnection": [
    {
      "IsActive": true,
      "ConnectionName": "DxWorklist1",
      "CallingAETitle": "DVTK_MW_SCU1",
      "CalledAETitle": "DVTK_MW_SCP1",
      "HostIP": "127.0.0.1",
      "Port": 107,
      "State": 1,
      "Modality": "DX",
      "WorklistDestination": 0,
      "EchoFailProcess": 0,
      "NameSeparator": "^",
      "NameDirection": 0,
      "ProtocolCode": 101
    }
  ]
}
```

### Settings Sections

#### FileLogger
Controls application logging behavior.

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `LogDirectory` | string | `"./log"` | Directory where log files are written |
| `FileSizeMB` | int | `10` | Maximum size of each log file in MB (minimum: 1) |
| `MaxFileCount` | int | `5` | Number of log files to retain (minimum: 5) |

#### DatabaseConnection
MySQL database connection parameters.

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `HostName` | string | `"localhost"` | Database server hostname or IP |
| `DatabaseName` | string | `"EtrekDb"` | Name of the database |
| `UserName` | string | `"root"` | Database username |
| `Password` | string | - | Database password (plain or encrypted) |
| `IsPasswordEncrypted` | bool | `false` | If true, password is AES encrypted (Base64) |

#### ModalityWorklistConnection
Array of DICOM Modality Worklist (MWL) connections for RIS integration.

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `IsActive` | bool | `true` | Enable/disable this connection |
| `ConnectionName` | string | - | User-friendly name for the connection |
| `CallingAETitle` | string | - | DICOM Application Entity title of this system |
| `CalledAETitle` | string | - | DICOM AE title of the remote RIS |
| `HostIP` | string | - | IP address of the RIS server |
| `Port` | int | `104` | DICOM port number |
| `State` | int | `1` | Connection state flag |
| `Modality` | string | `"DX"` | DICOM modality type (DX, CT, MR, etc.) |
| `WorklistDestination` | int | `0` | Worklist routing configuration |
| `EchoFailProcess` | int | `0` | How to handle DICOM echo failures |
| `NameSeparator` | string | `"^"` | DICOM patient name component separator |
| `NameDirection` | int | `0` | Name order: 0=First^Last, 1=Last^First |
| `ProtocolCode` | int | - | Protocol identifier for this connection |

## Database Settings (environment_settings)

Runtime environment settings stored in MySQL.

### Table Schema

```sql
CREATE TABLE environment_settings (
  id INT AUTO_INCREMENT PRIMARY KEY,
  study_level ENUM('Multi-Series Study','Single-Series Study') NOT NULL DEFAULT 'Multi-Series Study',
  lookup_table ENUM('VOI LUT','None') NOT NULL DEFAULT 'None',
  worklist_clear_period_days INT NOT NULL DEFAULT 30,
  worklist_refresh_period_seconds INT NOT NULL DEFAULT 60,
  delete_log_period_days INT NOT NULL DEFAULT 90,
  auto_refresh_worklist BOOL NOT NULL DEFAULT TRUE,
  auto_clear_disk_space BOOL NOT NULL DEFAULT TRUE,
  enable_mpps BOOL NOT NULL DEFAULT TRUE,
  continue_on_echo_fail BOOL NOT NULL DEFAULT TRUE,
  create_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  update_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);
```

### Configuration Options

| Setting | Type | Default | Description |
|---------|------|---------|-------------|
| `study_level` | enum | `Multi-Series Study` | Study organization mode |
| `lookup_table` | enum | `None` | Image display lookup table (VOI LUT or None) |
| `worklist_clear_period_days` | int | `30` | Days before clearing old worklist entries |
| `worklist_refresh_period_seconds` | int | `60` | Worklist auto-refresh interval |
| `delete_log_period_days` | int | `90` | Days before deleting old log files |
| `auto_refresh_worklist` | bool | `true` | Enable automatic worklist refresh |
| `auto_clear_disk_space` | bool | `true` | Enable automatic disk space cleanup |
| `enable_mpps` | bool | `true` | Enable DICOM Modality Performed Procedure Step |
| `continue_on_echo_fail` | bool | `true` | Continue operation if DICOM echo fails |

### Study Level Options
- **Multi-Series Study** - Each study can contain multiple series (e.g., AP and Lateral views)
- **Single-Series Study** - Each study contains exactly one series

### Lookup Table Options
- **VOI LUT** - Apply Value of Interest Lookup Table for display
- **None** - No lookup table applied

## Architecture

### Key Classes

| Class | Location | Purpose |
|-------|----------|---------|
| `SettingProvider` | `Core/Setting/` | Loads and parses Settings.json |
| `DatabaseConnectionSetting` | `Core/Data/Model/` | Database connection parameters |
| `FileLoggerSetting` | `Core/Data/Model/` | Logger configuration |
| `RisConnectionSetting` | `Core/Data/Model/` | RIS/Worklist connection config |
| `EnvironmentSetting` | `Common/Include/Device/Data/Entity/` | Database environment settings |
| `DeviceRepository` | `Device/Repository/` | Database access for settings |

### Loading Flow

```
Application Startup
       │
       ▼
┌──────────────────────┐
│ ApplicationService   │
│ loadSettings()       │
└──────────────────────┘
       │
       ▼
┌──────────────────────┐
│ SettingProvider      │
│ loadSettingsFile()   │◄── Settings.json
└──────────────────────┘
       │
       ▼
┌──────────────────────────────────────────┐
│ Extract settings into model objects:     │
│ - DatabaseConnectionSetting              │
│ - FileLoggerSetting                      │
│ - RisConnectionSetting[]                 │
└──────────────────────────────────────────┘
       │
       ▼
┌──────────────────────┐
│ Initialize Services: │
│ - Logger             │
│ - Database           │
│ - RIS Connections    │
└──────────────────────┘
```

### Password Encryption

Database passwords can be stored encrypted in Settings.json:

1. Set `IsPasswordEncrypted` to `true`
2. Password value should be AES encrypted and Base64 encoded
3. `CryptoManager::decryptPassword()` handles decryption at load time

## UI Configuration

Settings are managed through the **System Settings Page** accessible from the main toolbar.

### Settings Tabs

| Tab | Description |
|-----|-------------|
| Workflow | Environment settings (study level, auto-refresh, etc.) |
| Image Comment | Predefined image comments and reject reasons |
| Connection | RIS/Worklist connection configuration |
| Collimator | Collimator hardware settings |
| DAP | Dose Area Product meter settings |
| PACS Entity | PACS server configurations |
| Worklist | Worklist field mappings and display |
| Procedure | Procedure code definitions |
| View | Imaging view definitions |
| Technique Setting | Default technique parameters |
| Image Process | Image processing parameters |
| Generator | X-ray generator settings |
| Detector | Digital detector configuration |

## File Locations

```
etrek/
├── Core/
│   ├── Setting/
│   │   ├── SettingProvider.h/.cpp    # Settings loader
│   │   └── Resources/
│   │       └── Settings.json          # Source config file
│   ├── Data/Model/
│   │   ├── DatabaseConnectionSetting.h
│   │   ├── FileloggerSetting.h
│   │   └── RisConnectionSetting.h
│   └── Script/
│       └── setup_database.sql         # Database schema
├── Device/
│   └── Repository/
│       └── DeviceRepository.cpp       # environment_settings access
├── Common/Include/Device/Data/Entity/
│   └── EnvironmentSetting.h           # Environment settings entity
└── out/build/[config]/
    ├── setting/
    │   └── Settings.json              # Runtime copy
    └── log/
        └── *.log                      # Log files
```

## Best Practices

1. **Never commit real passwords** - Use encrypted passwords or environment-specific config files
2. **Use default ports** - DICOM standard port is 104, common alternatives are 107, 11112
3. **Test RIS connections** - Use DICOM echo to verify connectivity before going live
4. **Monitor log files** - Adjust `FileSizeMB` and `MaxFileCount` based on disk space
5. **Set appropriate refresh intervals** - Balance between freshness and network load
