# Hierarchical Status Tracking for Imaging Entities

## Overview

This feature adds comprehensive status tracking across all levels of the DICOM imaging hierarchy:
- **Patient** → **Study** → **Series** → **Image**

Each entity can now track its lifecycle status, enabling better workflow management, progress monitoring, and analytics.

## Status Field Specification

### Allowed Status Values
All entities use the same status enumeration from `ProcedureStepStatus`:

- `SCHEDULED` - Entity is scheduled for processing
- `PENDING` - Default state, waiting to be processed
- `IN_PROGRESS` - Currently being processed
- `COMPLETED` - Processing finished successfully
- `CANCELLED` - Cancelled by user or system
- `ABORTED` - Terminated due to error or failure

### Status Fields

Each entity (patients, studies, series, images) includes:

| Field | Type | Description |
|-------|------|-------------|
| `status` | ENUM | Current status from allowed values above |
| `status_reason` | TEXT | Optional explanation for the status (nullable) |
| `status_updated_at` | DATETIME | Timestamp when status was last changed |

### Default Behavior

- **Default Status**: `PENDING`
- **Automatic Timestamp**: `status_updated_at` is automatically updated via database trigger when `status` changes
- **Nullable Reason**: `status_reason` can be NULL or contain explanatory text

## Database Schema Changes

### New Table: `patients`

```sql
CREATE TABLE patients (
    id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id VARCHAR(64) NOT NULL,
    patient_name VARCHAR(255) DEFAULT NULL,
    patient_birth_date DATE DEFAULT NULL,
    patient_sex ENUM('M', 'F', 'O', 'U') DEFAULT NULL,
    issuer_of_patient_id VARCHAR(64) DEFAULT NULL,
    other_patient_ids VARCHAR(255) DEFAULT NULL,
    patient_comments TEXT DEFAULT NULL,
    
    -- Status tracking fields
    status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING',
    status_reason TEXT DEFAULT NULL,
    status_updated_at DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
    
    create_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    update_date DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
    
    UNIQUE KEY uk_patient_id_issuer (patient_id, issuer_of_patient_id),
    INDEX idx_patient_status (status),
    INDEX idx_patient_status_updated (status_updated_at)
);
```

### Modified Tables: `studies`, `series`, `images`

Each table now includes:
- `status` field (ENUM with same values)
- `status_reason` field (TEXT, nullable)
- `status_updated_at` field (DATETIME, auto-updated)
- Indexes on `status` and `status_updated_at` for performance

The `studies` table also includes:
- `patient_id` field (INT, foreign key to `patients.id`)

## Migration

### For Existing Databases

Run the migration script:
```sql
SOURCE Core/Script/migration_add_status_tracking.sql;
```

This script:
1. Creates the `patients` table
2. Adds status fields to existing tables
3. Creates triggers for automatic timestamp updates
4. Uses `IF NOT EXISTS` clauses for safety

### For New Installations

The main `setup_database.sql` includes all changes automatically.

## Entity Models

### C++ Entity Classes

All entity classes are located in `Common/Include/Core/Data/Entity/`:

- `Patient.h` - Patient entity with full DICOM patient module
- `Study.h` - Study entity with patient reference
- `Series.h` - Series entity
- `Image.h` - Image entity

Each entity includes:
```cpp
ProcedureStepStatus Status = ProcedureStepStatus::PENDING;
QString StatusReason;
QDateTime StatusUpdatedAt;
```

### Usage Example

```cpp
#include "Core/Data/Entity/Study.h"
#include "Worklist/Specification/WorklistEnum.h"

Etrek::Core::Data::Entity::Study study;
study.StudyInstanceUid = "1.2.3.4.5.6";
study.Status = ProcedureStepStatus::IN_PROGRESS;
study.StatusReason = "Scanning in progress";
// StatusUpdatedAt will be set automatically by the database trigger
```

## Repository Interfaces

Repository interfaces are defined in `Common/Include/Core/Repository/`:

- `IPatientRepository.h`
- `IStudyRepository.h`
- `ISeriesRepository.h`
- `IImageRepository.h`

Each interface provides:
- `updateStatus(id, status, reason)` - Update entity status
- `get{Entity}(id)` - Retrieve entity by ID

## Performance Considerations

### Indexes

Status fields are indexed for efficient queries:
```sql
INDEX idx_{table}_status (status)
INDEX idx_{table}_status_updated (status_updated_at)
```

This enables fast filtering and sorting by status:
```sql
-- Get all in-progress studies
SELECT * FROM studies WHERE status = 'IN_PROGRESS';

-- Get recently updated patients
SELECT * FROM patients ORDER BY status_updated_at DESC LIMIT 10;
```

### Triggers

Database triggers ensure `status_updated_at` is updated automatically:
```sql
CREATE TRIGGER trg_studies_status_update
BEFORE UPDATE ON studies
FOR EACH ROW
BEGIN
    IF NEW.status != OLD.status THEN
        SET NEW.status_updated_at = CURRENT_TIMESTAMP;
    END IF;
END;
```

## Hierarchical Propagation (Future Enhancement)

The current implementation provides status tracking at each level independently. Future enhancements could include:

- **Upward Propagation**: When all child series complete, automatically mark parent study as completed
- **Downward Propagation**: When a study is cancelled, cascade cancellation to all child series/images
- **Validation Rules**: Prevent marking a study as completed if any series are still in progress

These features can be implemented in application logic or via additional database triggers.

## Testing

### Unit Tests

Location: `Test/Core/tst_StatusTrackingTest.cpp`

Tests verify:
- Default status values (PENDING)
- Status updates across all entity types
- Status reason assignment
- All enum values can be assigned

Run tests:
```bash
cd out/build/debug
ctest -R StatusTrackingTest
```

### Integration Tests

Integration tests should verify:
- Database triggers update timestamps correctly
- Foreign key relationships work properly
- Indexes improve query performance
- Migration script handles existing data

## Backward Compatibility

The implementation maintains backward compatibility:

1. **Migration Script**: Uses `IF NOT EXISTS` and `ADD COLUMN IF NOT EXISTS`
2. **Default Values**: All status fields default to `PENDING`
3. **Nullable Fields**: `status_reason` and `status_updated_at` can be NULL
4. **Existing Data**: Migration adds fields without requiring data migration

## References

- [DICOM Standard - Patient Module](https://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.7.html#sect_C.7.1.1)
- [DICOM Standard - Study Module](https://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.7.html#sect_C.7.2.1)
- Existing `mwl_entries.status` implementation in `Common/Include/Worklist/`

## Migration Checklist

When deploying this feature:

- [ ] Backup production database
- [ ] Run migration script on test environment
- [ ] Verify triggers are created successfully
- [ ] Verify indexes are created successfully
- [ ] Test status updates on each entity type
- [ ] Run full test suite
- [ ] Deploy to production
- [ ] Monitor query performance
- [ ] Update application documentation
