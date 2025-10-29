# Admission ID Implementation Guide

## Overview

This document describes the implementation of `admission_id` support in the Etrek medical imaging application, aligning with DICOM tag (0038,0010) from the Patient Study module.

## Database Schema Changes

### 1. Studies Table

**File**: `Core/Script/setup_database.sql`

Added `admission_id` column to the `studies` table:

```sql
CREATE TABLE studies (
    -- ... existing columns ...
    admission_id VARCHAR(64) DEFAULT NULL,  -- (0038,0010): Admission ID from Patient Study module
    INDEX idx_admission_id (admission_id)
);
```

### 2. Migration Script

**File**: `Core/Script/add_admission_id_to_studies.sql`

Created an ALTER TABLE migration script for existing databases:

```sql
ALTER TABLE studies
ADD COLUMN IF NOT EXISTS admission_id VARCHAR(64) DEFAULT NULL COMMENT '(0038,0010): Admission ID from Patient Study module';

CREATE INDEX IF NOT EXISTS idx_admission_id ON studies(admission_id);
```

## Existing Infrastructure

The following components already support AdmissionID:

### 1. DICOM Tag Definition

AdmissionID tag is already defined in the database setup script (line 689):

```sql
('AdmissionID','Admission ID', 0x0038, 0x0010, 0x0000, 0x0000, TRUE, FALSE),
```

### 2. Worklist Field Configuration

AdmissionID is included in the worklist field configurations (line 797):

```sql
INSERT INTO worklist_field_configurations(field_name,is_enabled)
VALUES ('AdmissionID',FALSE);
```

**Note**: It's currently disabled by default. To enable it for worklist queries:

```sql
UPDATE worklist_field_configurations SET is_enabled = TRUE WHERE field_name = 'AdmissionID';
```

### 3. Mapping Profile

AdmissionID is already mapped in `Worklist/Mapping/MappingProfile.h` (lines 117-122):

```cpp
{"AdmissionID", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
     Etrek::Worklist::Data::Entity::WorklistAttribute attr;
     attr.Tag.Name = "AdmissionID";
     attr.TagValue = v;
     m.Attributes.append(attr);
 }}
```

### 4. Enum Definition

AdmissionID is defined in `Common/Include/Worklist/Specification/WorklistEnum.h`:

```cpp
enum class WorklistFieldName {
    // ... other fields ...
    AdmissionID,
    // ... other fields ...
};
```

## Implementation Requirements

### Code Changes Needed

Since the Etrek codebase doesn't appear to have explicit Study entity classes or repositories that manipulate the studies table directly, the following areas should be reviewed and updated as needed:

#### 1. Study Creation/Update Code

Search for code that performs INSERT or UPDATE operations on the `studies` table. These operations should include the `admission_id` field.

**Expected pattern**:
```cpp
QString insertQuery = R"(
    INSERT INTO studies (
        study_instance_uid,
        study_id,
        accession_number,
        admission_id,  -- Add this
        -- ... other fields ...
    ) VALUES (?, ?, ?, ?, ...)
)";
```

#### 2. Study Query Code

Any SELECT queries on the `studies` table should include `admission_id` in the column list:

```cpp
QString selectQuery = R"(
    SELECT id, study_instance_uid, study_id, accession_number, admission_id, ...
    FROM studies
    WHERE ...
)";
```

#### 3. DICOM Ingestion

When DICOM studies are ingested (via C-STORE SCP or similar), the code should:

1. Extract the Admission ID from DICOM tag (0038,0010)
2. Store it in the `admission_id` column of the `studies` table

**Example using DCMTK** (pseudo-code):

```cpp
OFString admissionId;
if (dataset->findAndGetOFString(DCM_AdmissionID, admissionId).good()) {
    // Store admissionId in database
    QString admissionIdStr = QString::fromStdString(admissionId.c_str());
    // Include in INSERT/UPDATE query
}
```

#### 4. Worklist to Study Conversion

When a worklist entry is converted to a study, the AdmissionID attribute should be extracted and stored:

```cpp
// Get AdmissionID from worklist attributes
for (const auto& attr : worklistEntry.Attributes) {
    if (attr.Tag.Name == "AdmissionID") {
        QString admissionId = attr.TagValue;
        // Store in studies table
    }
}
```

#### 5. API/DTO Updates

If there are DTOs or API responses that return study data, add `admissionId` field:

```cpp
struct StudyDTO {
    int id;
    QString studyInstanceUid;
    QString accessionNumber;
    QString admissionId;  // Add this
    // ... other fields ...
};
```

#### 6. Filtering Support

Add support for filtering studies by `admission_id`:

```cpp
QString whereClause = "WHERE 1=1";
if (!admissionId.isEmpty()) {
    whereClause += " AND admission_id = ?";
}
```

## Testing

### Unit Tests

Create tests to verify:

1. Studies can be created with `admission_id`
2. Studies can be queried by `admission_id`
3. Null `admission_id` values are handled correctly
4. The field is properly indexed for performance

### Integration Tests

1. Create a DICOM study with Admission ID tag (0038,0010)
2. Verify it's stored correctly in the database
3. Retrieve the study and verify the `admission_id` is present

### Example Test (pseudo-code)

```cpp
void test_StudyWithAdmissionId() {
    // Create a study with admission_id
    QString admissionId = "ADM-12345";
    int studyId = createStudy(studyInstanceUid, accessionNumber, admissionId);
    
    // Query the study
    auto result = getStudyById(studyId);
    QVERIFY(result.admissionId == admissionId);
    
    // Filter by admission_id
    auto filtered = getStudiesByAdmissionId(admissionId);
    QVERIFY(filtered.size() > 0);
    QVERIFY(filtered[0].admissionId == admissionId);
}
```

## Observability

### Metrics to Add

1. **admission_id_population_rate**: Percentage of studies with non-null `admission_id`
2. **admission_id_ingestion_count**: Counter for studies ingested with `admission_id`

### Logging

Add log entries when:
1. A study is created/updated with `admission_id`
2. `admission_id` is extracted from DICOM
3. Queries filter by `admission_id`

## Backward Compatibility

This is an **additive change**:

- ✅ Existing code will continue to work (field is nullable)
- ✅ Existing queries don't need to include the new field
- ✅ No breaking changes to APIs (field is optional)
- ✅ Migration script safely adds the column to existing databases

## Rollout Checklist

- [x] Database schema updated (setup_database.sql)
- [x] Migration script created (add_admission_id_to_studies.sql)
- [x] Verified DICOM tag exists
- [x] Verified mapping profile exists
- [ ] Update study creation code to include admission_id
- [ ] Update study query code to include admission_id
- [ ] Update DICOM ingestion to extract (0038,0010)
- [ ] Add API/DTO fields
- [ ] Add filtering support
- [ ] Add unit tests
- [ ] Add integration tests
- [ ] Add observability (metrics/logs)
- [ ] Update API documentation
- [ ] Update DB schema documentation
- [ ] Run migration on test database
- [ ] Run migration on production database

## References

- DICOM Standard: Part 3, Section C.7.2.2 (Patient Study Module)
- DICOM Tag (0038,0010): Admission ID - VR: LO (Long String)
- Database Schema: `Core/Script/setup_database.sql`
- Mapping Profile: `Worklist/Mapping/MappingProfile.h`
