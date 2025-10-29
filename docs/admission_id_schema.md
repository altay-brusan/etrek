# Database Schema: Admission ID Support

## Overview

This document describes the Admission ID support added to the Etrek medical imaging application database schema.

## DICOM Background

- **Tag**: (0038,0010) - Admission ID
- **Module**: Study-Patient Module (Patient Relationship)  
- **VR**: LO (Long String)
- **VM**: 1
- **Description**: Identifier of the admission event associated with the study

The Admission ID is used to identify the hospital admission or visit associated with a medical imaging study. It helps correlate imaging studies with patient admission records in hospital information systems (HIS).

## Database Schema Changes

### studies Table

Added column to the `studies` table:

```sql
admission_id VARCHAR(64) DEFAULT NULL  -- (0038,0010)
```

**Column Details:**
- **Name**: `admission_id`
- **Type**: VARCHAR(64)
- **Nullable**: YES (NULL allowed)
- **Default**: NULL
- **Index**: Non-unique index `idx_admission_id` for query performance
- **DICOM Tag**: (0038,0010)
- **Source**: DICOM Study-Patient module

### Index

```sql
INDEX idx_admission_id (admission_id)
```

The index supports efficient querying of studies by Admission ID.

## DICOM Tag Mapping

The AdmissionID DICOM tag is already defined in the `dicom_tags` table:

```sql
INSERT INTO dicom_tags (...) VALUES
('AdmissionID','Admission ID', 0x0038, 0x0010, 0x0000, 0x0000, TRUE, FALSE);
```

And is included in the DxWorklist profile:

```sql
INSERT INTO profile_tag_association (profile_id, tag_id, is_identifier, is_mandatory)
VALUES
(1, (SELECT id FROM dicom_tags WHERE name = 'AdmissionID'), FALSE, TRUE);
```

## Data Flow

1. **Ingestion**: DICOM tag (0038,0010) is captured from incoming DICOM files
2. **Worklist**: Tag value is stored in `mwl_attributes` table linked to worklist entries
3. **Studies**: Value should be mapped to `studies.admission_id` during study creation
4. **Query**: Applications can filter studies by `admission_id` using the indexed column

## Migration

For existing databases, use the migration script:

```bash
mysql -u username -p database_name < Core/Script/migration_add_admission_id.sql
```

The migration script is idempotent and can be safely run multiple times.

## Backfill

To populate `admission_id` from existing worklist data:

```bash
mysql -u username -p database_name < Core/Script/backfill_admission_id.sql
```

The backfill script:
- Only updates studies where `admission_id` IS NULL
- Joins with worklist data via `study_instance_uid`
- Retrieves AdmissionID tag values from `mwl_attributes`
- Is idempotent and transaction-safe

## Usage Examples

### Query studies by Admission ID

```sql
SELECT * FROM studies 
WHERE admission_id = 'ADM123456';
```

### Query studies with Admission ID present

```sql
SELECT * FROM studies 
WHERE admission_id IS NOT NULL;
```

### Statistics

```sql
SELECT 
    COUNT(*) AS total_studies,
    SUM(CASE WHEN admission_id IS NULL THEN 1 ELSE 0 END) AS without_admission_id,
    SUM(CASE WHEN admission_id IS NOT NULL THEN 1 ELSE 0 END) AS with_admission_id,
    ROUND(100.0 * SUM(CASE WHEN admission_id IS NOT NULL THEN 1 ELSE 0 END) / COUNT(*), 2) AS percentage_populated
FROM studies;
```

## Integration Points

### C++ Code Integration (Future)

When implementing study creation/update in C++:

1. **Reading DICOM**: Extract tag (0038,0010) using DCMTK
   ```cpp
   OFString admissionId;
   dataset.findAndGetOFString(DCM_AdmissionID, admissionId);
   ```

2. **Database Insertion**: Include admission_id in INSERT/UPDATE statements
   ```cpp
   query.prepare("INSERT INTO studies (..., admission_id) VALUES (..., :admission_id)");
   query.bindValue(":admission_id", QString::fromStdString(admissionId.c_str()).trimmed());
   ```

3. **Query Support**: Add admission_id filter to repository methods
   ```cpp
   query.prepare("SELECT * FROM studies WHERE admission_id = :admission_id");
   ```

### Worklist Module Integration

The MappingProfile.h already includes AdmissionID mapping (lines 117-122):

```cpp
{"AdmissionID", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
     Etrek::Worklist::Data::Entity::WorklistAttribute attr;
     attr.Tag.Name = "AdmissionID";
     attr.TagValue = v;
     m.Attributes.append(attr);
 }}
```

## Testing

### Manual Testing

1. Insert a study with admission_id:
```sql
INSERT INTO studies (study_instance_uid, study_id, admission_id) 
VALUES ('1.2.3.4.5', 'ST001', 'ADM123456');
```

2. Query by admission_id:
```sql
SELECT * FROM studies WHERE admission_id = 'ADM123456';
```

3. Verify index usage:
```sql
EXPLAIN SELECT * FROM studies WHERE admission_id = 'ADM123456';
```

## Compatibility

- **Backward Compatible**: YES - Column is nullable with DEFAULT NULL
- **Existing Queries**: Unaffected - New column doesn't break existing SELECT/INSERT/UPDATE
- **Schema Version**: Added in version corresponding to this PR
- **DICOM Compliance**: Fully compliant with DICOM PS3.3 Study-Patient module

## References

- [DICOM PS3.3 Section C.7.2.2 - Patient Study Module](http://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.7.2.2.html)
- [DICOM Tag (0038,0010) - Admission ID](http://dicom.nema.org/medical/dicom/current/output/chtml/part06/chapter_6.html)
- GitHub Issue: https://github.com/altay-brusan/etrek/issues/29
