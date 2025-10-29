# Database Schema Changes: Admission ID

## Overview

This directory contains SQL scripts for adding `admission_id` support to the Etrek database schema.

## Files

1. **setup_database.sql** (Modified)
   - Main database setup script
   - Contains the complete schema including the new `admission_id` column

2. **add_admission_id_to_studies.sql** (New)
   - Migration script for existing databases
   - Safely adds `admission_id` column and index
   - Uses `IF NOT EXISTS` for idempotency

3. **validate_admission_id.sql** (New)
   - Validation queries to verify the schema changes
   - Checks column, index, and DICOM tag configuration

## Running the Migration

### For New Installations

Simply run the main setup script:

```bash
mysql -u <username> -p <database_name> < setup_database.sql
```

### For Existing Databases

Run the migration script:

```bash
mysql -u <username> -p <database_name> < add_admission_id_to_studies.sql
```

The script is safe to run multiple times (idempotent).

### Validation

After running the migration, validate the changes:

```bash
mysql -u <username> -p <database_name> < validate_admission_id.sql
```

## Schema Changes Details

### Studies Table

Added column:
```sql
admission_id VARCHAR(64) DEFAULT NULL
```

Added index:
```sql
INDEX idx_admission_id (admission_id)
```

### DICOM Mapping

The AdmissionID DICOM tag (0038,0010) is already configured:
- Defined in `dicom_tags` table
- Included in `worklist_field_configurations` (disabled by default)
- Mapped in the application's MappingProfile

## Enabling AdmissionID in Worklists

By default, AdmissionID is disabled in worklist queries. To enable it:

```sql
UPDATE worklist_field_configurations 
SET is_enabled = TRUE 
WHERE field_name = 'AdmissionID';
```

## Backward Compatibility

This is a **backward-compatible change**:

- ✅ Column is nullable (no default value required)
- ✅ Existing queries work without modification
- ✅ Migration is safe for production
- ✅ No application code changes required for basic operation

## Next Steps

After applying the migration:

1. **Verify** the schema using `validate_admission_id.sql`
2. **Update** application code to populate `admission_id` during study creation
3. **Enable** AdmissionID in worklist queries if needed (see above)
4. **Test** with DICOM data containing Admission ID tag (0038,0010)
5. **Monitor** admission_id population rate
6. **Update** any APIs/DTOs to include the new field

## Rollback

If needed, the migration can be rolled back:

```sql
-- Remove index
DROP INDEX IF EXISTS idx_admission_id ON studies;

-- Remove column
ALTER TABLE studies DROP COLUMN IF EXISTS admission_id;
```

**Warning**: This will permanently delete any stored admission_id data.

## References

- DICOM Standard Part 3: C.7.2.2 (Patient Study Module)
- DICOM Tag (0038,0010): Admission ID (VR: LO)
- Implementation Guide: `docs/ADMISSION_ID_IMPLEMENTATION.md`
