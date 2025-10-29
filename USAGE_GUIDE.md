# Status Tracking Implementation Summary

## Quick Reference

### Status Values
```cpp
enum class ProcedureStepStatus {
    SCHEDULED,     // Scheduled for processing
    PENDING,       // Default - waiting to be processed
    IN_PROGRESS,   // Currently being processed
    COMPLETED,     // Finished successfully
    CANCELLED,     // Cancelled by user/system
    ABORTED        // Terminated due to error
};
```

### Entity Usage

```cpp
#include "Core/Data/Entity/Patient.h"
#include "Core/Data/Entity/Study.h"
#include "Core/Data/Entity/Series.h"
#include "Core/Data/Entity/Image.h"

// Create a new patient with default PENDING status
Etrek::Core::Data::Entity::Patient patient;
patient.PatientId = "PAT12345";
patient.PatientName = "John Doe";
patient.Status = ProcedureStepStatus::SCHEDULED;
patient.StatusReason = "Patient check-in completed";

// Create a study linked to patient
Etrek::Core::Data::Entity::Study study;
study.PatientId = patient.Id;
study.StudyInstanceUid = "1.2.840.113619.2.55.3.1234567890";
study.Status = ProcedureStepStatus::IN_PROGRESS;
study.StatusReason = "Acquiring images";

// Create a series
Etrek::Core::Data::Entity::Series series;
series.StudyId = study.Id;
series.SeriesInstanceUid = "1.2.840.113619.2.55.3.1234567890.1";
series.Status = ProcedureStepStatus::COMPLETED;

// Create an image
Etrek::Core::Data::Entity::Image image;
image.StudyId = study.Id;
image.SeriesId = series.Id;
image.Status = ProcedureStepStatus::COMPLETED;
```

### Repository Interface Usage

```cpp
#include "Core/Repository/IStudyRepository.h"

// Update study status (repository handles timestamp automatically)
IStudyRepository* studyRepo = getStudyRepository();
auto result = studyRepo->updateStatus(
    studyId,
    ProcedureStepStatus::COMPLETED,
    "All images acquired successfully"
);

if (result.IsSuccess()) {
    // Status updated successfully
    // Database trigger has automatically set status_updated_at
}
```

### SQL Queries

```sql
-- Find all in-progress studies
SELECT * FROM studies WHERE status = 'IN_PROGRESS';

-- Find studies by patient
SELECT s.* FROM studies s
JOIN patients p ON s.patient_id = p.id
WHERE p.patient_id = 'PAT12345';

-- Get recently updated entities
SELECT * FROM images 
WHERE status_updated_at > DATE_SUB(NOW(), INTERVAL 1 HOUR)
ORDER BY status_updated_at DESC;

-- Count entities by status
SELECT status, COUNT(*) as count
FROM series
GROUP BY status;

-- Find completed studies with all series completed
SELECT st.*
FROM studies st
WHERE st.status = 'COMPLETED'
AND NOT EXISTS (
    SELECT 1 FROM series se
    WHERE se.study_id = st.id
    AND se.status != 'COMPLETED'
);
```

## Database Hierarchy

```
patients (patient_id, status, status_reason, status_updated_at)
    └── studies (patient_id FK, status, status_reason, status_updated_at)
        └── series (study_id FK, status, status_reason, status_updated_at)
            └── images (series_id FK, study_id FK, status, status_reason, status_updated_at)
```

## Key Features

1. **Automatic Timestamps**: Database triggers update `status_updated_at` when status changes
2. **Default Status**: All entities default to `PENDING` status
3. **Indexed Fields**: `status` and `status_updated_at` are indexed for fast queries
4. **Nullable Reason**: `status_reason` can be NULL or contain explanatory text
5. **Backward Compatible**: Migration script safely adds fields to existing databases

## Common Workflows

### Workflow 1: Schedule a new patient study
```cpp
// 1. Create patient
Patient patient;
patient.Status = ProcedureStepStatus::SCHEDULED;

// 2. Create study
Study study;
study.PatientId = patient.Id;
study.Status = ProcedureStepStatus::SCHEDULED;
```

### Workflow 2: Mark series as in progress
```cpp
series.Status = ProcedureStepStatus::IN_PROGRESS;
series.StatusReason = "Technician acquiring images";
// Update via repository
```

### Workflow 3: Complete an image acquisition
```cpp
image.Status = ProcedureStepStatus::COMPLETED;
// Trigger automatically sets status_updated_at
```

### Workflow 4: Cancel a study
```cpp
study.Status = ProcedureStepStatus::CANCELLED;
study.StatusReason = "Patient requested cancellation";
// Optional: Cascade cancellation to child series/images
```

## Testing

Unit tests are in `Test/Core/tst_StatusTrackingTest.cpp`:

```bash
# Build and run tests
cd out/build/debug
cmake --build .
ctest -R StatusTrackingTest -V
```

## Migration for Existing Databases

```bash
# Connect to MySQL
mysql -u root -p etrek_db

# Run migration
source Core/Script/migration_add_status_tracking.sql;

# Verify changes
SHOW COLUMNS FROM patients;
SHOW COLUMNS FROM studies;
SHOW TRIGGERS;
```

## Performance Tips

1. Use indexed status field for filtering:
   ```sql
   SELECT * FROM studies WHERE status = 'IN_PROGRESS';  -- Fast (uses index)
   ```

2. Use status_updated_at for time-based queries:
   ```sql
   SELECT * FROM series 
   WHERE status_updated_at > '2025-10-01'  -- Fast (uses index)
   ORDER BY status_updated_at DESC;
   ```

3. Avoid full table scans when possible:
   ```sql
   -- Good: Uses index
   SELECT COUNT(*) FROM images WHERE status = 'COMPLETED';
   
   -- Also good: Uses index
   SELECT * FROM studies 
   WHERE status IN ('IN_PROGRESS', 'SCHEDULED')
   ORDER BY status_updated_at DESC;
   ```

## Troubleshooting

### Issue: status_updated_at not updating
**Solution**: Check that the trigger exists:
```sql
SHOW TRIGGERS WHERE `Trigger` LIKE '%status_update%';
```

### Issue: Status not defaulting to PENDING
**Solution**: Check table definition:
```sql
SHOW CREATE TABLE studies;
-- Should show: status ... DEFAULT 'PENDING'
```

### Issue: Foreign key constraint fails
**Solution**: Ensure parent entity exists before creating child:
```cpp
// Always create patient before study
patient = patientRepo->create(patient);
study.PatientId = patient.Id;  // Use actual ID, not -1
```

## Next Steps

For hierarchical status propagation (future enhancement):
1. Implement application logic to cascade status changes
2. Add validation rules (e.g., can't complete study if series are in progress)
3. Create stored procedures for complex status workflows
4. Add event listeners for status change notifications

## References

- Full documentation: `Core/Script/STATUS_TRACKING_README.md`
- Migration script: `Core/Script/migration_add_status_tracking.sql`
- Setup script: `Core/Script/setup_database.sql`
- Entity definitions: `Common/Include/Core/Data/Entity/`
- Repository interfaces: `Common/Include/Core/Repository/`
- Tests: `Test/Core/tst_StatusTrackingTest.cpp`
