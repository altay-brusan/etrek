# Database Migration: Fix mwl_attributes FK Constraint

## Overview

This migration fixes the foreign key constraint on `mwl_attributes.dicom_tag_id` to prevent deletion of globally-managed `dicom_tags` records while they are referenced.

## Migration Files

- **001_fix_mwl_attributes_fk_constraint.sql** - Forward migration
- **001_fix_mwl_attributes_fk_constraint_rollback.sql** - Rollback migration

## Changes

### Before
```sql
FOREIGN KEY (dicom_tag_id) REFERENCES dicom_tags(id) ON DELETE CASCADE
```

### After
```sql
FOREIGN KEY (dicom_tag_id) REFERENCES dicom_tags(id) ON DELETE RESTRICT
```

## Rationale

The `dicom_tags` table contains global, admin-managed DICOM tag definitions that should never be deleted while referenced by instance data in `mwl_attributes`. While `ON DELETE CASCADE` only flows parent → child (so deleting a child `mwl_attributes` record never deletes the parent `dicom_tags`), using `ON DELETE RESTRICT` provides an additional safety guarantee: it prevents accidental deletion of tag definitions that are actively in use.

## Expected Behavior

After applying this migration:

1. **AC1 - Child → Parent**: Deleting `mwl_attributes` records does NOT delete `dicom_tags` (this was already true, but remains correct)

2. **AC2 - Protection**: Attempting to delete a `dicom_tags` record that is referenced by any `mwl_attributes` record will be **blocked** with a foreign key constraint error

3. **AC3 - Cascade unchanged**: Deleting `mwl_entries` records still cascades to delete their associated `mwl_attributes` records (the `mwl_entry_id` FK remains `ON DELETE CASCADE`)

## How to Apply

### Forward Migration
```bash
mysql -u root -p etrekdb < Core/Script/migrations/001_fix_mwl_attributes_fk_constraint.sql
```

### Rollback (if needed)
```bash
mysql -u root -p etrekdb < Core/Script/migrations/001_fix_mwl_attributes_fk_constraint_rollback.sql
```

## Testing

The test suite `tst_MwlAttributesForeignKey` validates all three acceptance criteria:
- `testDeletingMwlAttributesDoesNotDeleteDicomTags` (AC1)
- `testDeletingReferencedDicomTagIsBlocked` (AC2)
- `testDeletingMwlEntryCascadesToAttributes` (AC3)

Run tests:
```bash
cd out/build/debug
ctest -R tst_MwlAttributesForeignKey -V
```

## Database Schema Update

The base schema file `Core/Script/setup_database.sql` has been updated to reflect the corrected FK constraint for new database installations.
