# Acceptance Criteria Validation

This document maps the implemented changes to the original acceptance criteria from the issue.

## AC1 — Schema ✅ COMPLETE

**Requirement**: 
> **GIVEN** the database migration is applied
> **WHEN** I inspect the `studies` table schema
> **THEN** I see a new column `admission_id` (string/text), nullable, indexed (non-unique).

**Implementation**:
- ✅ Added `admission_id VARCHAR(64) DEFAULT NULL` to `studies` table
- ✅ Column is nullable (DEFAULT NULL)
- ✅ Added non-unique index: `INDEX idx_admission_id (admission_id)`
- ✅ Properly commented with DICOM tag reference: `(0038,0010): Admission ID from Patient Study module`

**Files**:
- `Core/Script/setup_database.sql` (lines 385-386)
- `Core/Script/add_admission_id_to_studies.sql` (migration)
- `Core/Script/validate_admission_id.sql` (validation queries)

**Validation**:
```sql
-- Run this to verify:
SELECT COLUMN_NAME, DATA_TYPE, IS_NULLABLE 
FROM INFORMATION_SCHEMA.COLUMNS
WHERE TABLE_NAME = 'studies' AND COLUMN_NAME = 'admission_id';

SELECT INDEX_NAME, COLUMN_NAME
FROM INFORMATION_SCHEMA.STATISTICS
WHERE TABLE_NAME = 'studies' AND COLUMN_NAME = 'admission_id';
```

---

## AC2 — Ingestion ⚠️ PARTIALLY COMPLETE

**Requirement**:
> **GIVEN** a DICOM Study with tag `(0038,0010)` present
> **WHEN** the study is ingested
> **THEN** `admission_id` is persisted with the exact value (no normalization beyond our standard trimming).

**Status**: Database schema ready; ingestion code needs to be implemented.

**What's Done**:
- ✅ DICOM tag (0038,0010) AdmissionID already defined in `dicom_tags` table (line 689 in setup_database.sql)
- ✅ AdmissionID already mapped in `Worklist/Mapping/MappingProfile.h` (lines 117-122)
- ✅ Database column ready to receive data

**What's Needed**:
- [ ] Locate DICOM C-STORE ingestion code or study creation code
- [ ] Extract DICOM tag (0038,0010) value
- [ ] Insert/update `admission_id` in `studies` table

**Example Code Needed** (pseudo-code):
```cpp
OFString admissionId;
if (dataset->findAndGetOFString(DCM_AdmissionID, admissionId).good()) {
    QString admissionIdStr = QString::fromStdString(admissionId.c_str()).trimmed();
    // Include in INSERT/UPDATE query for studies table
}
```

**Note**: This requires access to the DICOM ingestion code, which was not found in the current codebase review. It may be in development or in a module not yet reviewed.

---

## AC3 — APIs/DTO/UI ⚠️ PARTIALLY COMPLETE

**Requirement**:
> **GIVEN** a study fetch via repository/API
> **WHEN** the study is returned
> **THEN** payloads include `admissionId` field; existing clients remain backward compatible (field is optional).

**Status**: Database schema ready; API/DTO code needs to be updated.

**What's Done**:
- ✅ Database column is nullable (backward compatible)
- ✅ Documentation provided for DTO updates

**What's Needed**:
- [ ] Find Study DTOs/entities (if they exist)
- [ ] Add `admissionId` field to DTOs
- [ ] Update SELECT queries to include `admission_id`
- [ ] Ensure JSON/XML serialization includes the field

**Note**: No explicit Study entity classes were found in the codebase review. Studies may be accessed via raw SQL queries, in which case those queries need to be updated to include the `admission_id` column.

---

## AC4 — Queryability ⚠️ PARTIALLY COMPLETE

**Requirement**:
> **GIVEN** there are studies with `admission_id` set
> **WHEN** I filter by `admission_id`
> **THEN** matching studies are returned.

**Status**: Database indexed for filtering; query code needs to be implemented.

**What's Done**:
- ✅ Non-unique index created on `admission_id` for efficient filtering
- ✅ Index name: `idx_admission_id`

**What's Needed**:
- [ ] Add filter parameter to study query APIs
- [ ] Implement WHERE clause for `admission_id`

**Example Code Needed** (pseudo-code):
```cpp
QString whereClause = "WHERE 1=1";
if (!admissionId.isEmpty()) {
    whereClause += " AND admission_id = ?";
    query.bindValue(admissionId);
}
```

---

## AC5 — Backfill (best-effort) ⚠️ NOT STARTED

**Requirement**:
> **GIVEN** legacy rows where `admission_id` can be derived from prior staging or metadata
> **WHEN** the backfill job runs
> **THEN** those rows are populated; rows without reliable source remain `NULL`.

**Status**: Not implemented (may not be needed).

**What's Done**:
- ✅ Column is nullable, so legacy rows will have NULL values
- ✅ This is acceptable per the requirement

**What's Needed** (optional):
- [ ] Determine if there's a source for backfilling (e.g., worklist attributes)
- [ ] Write backfill script if source exists

**Possible Backfill Query**:
```sql
-- If admission_id can be derived from worklist attributes:
UPDATE studies s
JOIN mwl_task_mapping m ON s.id = m.study_id
JOIN mwl_attributes a ON m.mwl_entry_id = a.mwl_entry_id
JOIN dicom_tags t ON a.dicom_tag_id = t.id
SET s.admission_id = a.tag_value
WHERE t.name = 'AdmissionID'
  AND s.admission_id IS NULL;
```

---

## AC6 — Observability ❌ NOT IMPLEMENTED

**Requirement**:
> **GIVEN** migrations and ingestions run
> **WHEN** I check logs/metrics
> **THEN** there are no new errors; ingestion counters include a gauge for `% studies with admission_id`.

**Status**: Not implemented (requires application code).

**What's Done**:
- ✅ Documentation provided for observability requirements

**What's Needed**:
- [ ] Add logging when `admission_id` is ingested
- [ ] Add metric: `admission_id_population_rate`
- [ ] Add metric: `admission_id_ingestion_count`

**Example Metrics**:
```cpp
// Log when admission_id is extracted
if (!admissionId.isEmpty()) {
    logger->LogInfo(QString("Study ingested with admission_id: %1").arg(admissionId));
    metrics->IncrementCounter("admission_id_ingestion_count");
}

// Periodic calculation of population rate
double rate = (studiesWithAdmissionId / totalStudies) * 100;
metrics->SetGauge("admission_id_population_rate", rate);
```

---

## Definition of Done Status

### Repository/entities/DTOs updated
- ✅ Database schema updated
- ⚠️ Code entities/DTOs not yet updated (if they exist)

### E2E test creates a Study with `admission_id` and retrieves it via API
- ❌ Not implemented (requires build environment and code integration)

### Docs updated: DB schema diagram, DICOM→DB mapping table
- ✅ Schema updated with inline comments
- ✅ Implementation guide created (`docs/ADMISSION_ID_IMPLEMENTATION.md`)
- ✅ Migration guide created (`Core/Script/README_ADMISSION_ID.md`)
- ⚠️ Schema diagram not updated (if one exists)

---

## Summary

### Completed ✅
1. Database schema changes (AC1)
2. Migration scripts (idempotent)
3. Validation scripts
4. Comprehensive documentation
5. Verified existing infrastructure (DICOM tag, mapping, enum)

### Requires Windows Build Environment ⚠️
1. DICOM ingestion code updates (AC2)
2. Study creation/update code
3. API/DTO updates (AC3)
4. Query filtering (AC4)
5. Observability (AC6)
6. E2E testing

### Optional/Low Priority
1. Backfill script (AC5) - nullable column makes this non-critical
2. Schema diagram update

---

## Deployment Recommendation

The database schema changes are **ready for deployment** and are **backward compatible**. They can be deployed independently of code changes:

1. **Immediate**: Deploy migration script to test database
2. **Immediate**: Validate with validation queries
3. **Next**: Implement code integration (requires Windows build)
4. **After code**: Deploy to production with full functionality

The system will continue to work with `admission_id` as NULL until the ingestion code is updated.
