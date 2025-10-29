# Database Schema Documentation: Foreign Key Constraints

## Overview

This document details the foreign key constraints in the Etrek database schema, with particular focus on the relationship between `mwl_attributes`, `dicom_tags`, and `mwl_entries`.

## Key Tables and Relationships

### dicom_tags (Global Tag Dictionary)

The `dicom_tags` table contains the global dictionary of DICOM tags used throughout the application.

**Characteristics:**
- **Global/Admin-managed**: These tags are defined at the application level
- **Shared resource**: Referenced by multiple tables across the schema
- **Should not be deleted**: While referenced by instance data

**Foreign Keys Referencing This Table:**
- `mwl_attributes.dicom_tag_id` → `dicom_tags.id` (ON DELETE RESTRICT)
- `profile_tag_association.tag_id` → `dicom_tags.id` (ON DELETE CASCADE)

### mwl_entries (Worklist Entries)

The `mwl_entries` table stores worklist entries received from RIS or created locally.

**Foreign Keys:**
- `profile_id` → `mwl_profiles.id` (ON DELETE SET NULL)

**Tables Referencing This Table:**
- `mwl_attributes.mwl_entry_id` → `mwl_entries.id` (ON DELETE CASCADE)
- `mwl_task_mapping.mwl_entry_id` → `mwl_entries.id` (ON DELETE CASCADE)

### mwl_attributes (Tag Values for Worklist Entries)

The `mwl_attributes` table stores actual DICOM tag values for each worklist entry.

**Relationship Model:**
```
dicom_tags (1) ←--[RESTRICT]--→ (N) mwl_attributes
mwl_entries (1) ←--[CASCADE]--→ (N) mwl_attributes
```

**Foreign Keys:**
1. `mwl_entry_id` → `mwl_entries.id` (ON DELETE CASCADE)
   - **Purpose**: When a worklist entry is deleted, all its attributes should be deleted
   - **Behavior**: Parent deletion cascades to children
   
2. `dicom_tag_id` → `dicom_tags.id` (ON DELETE RESTRICT)
   - **Purpose**: Prevent deletion of tag definitions that are in use
   - **Behavior**: Parent deletion is blocked while children exist

## Delete Behavior Matrix

| Action | Effect on mwl_attributes | Effect on dicom_tags | Effect on mwl_entries |
|--------|--------------------------|----------------------|----------------------|
| Delete `mwl_attributes` row | Row deleted | No effect (child → parent never cascades) | No effect (child → parent never cascades) |
| Delete `dicom_tags` row (referenced) | **BLOCKED** (FK RESTRICT) | N/A | No effect |
| Delete `dicom_tags` row (unreferenced) | No effect | Row deleted | No effect |
| Delete `mwl_entries` row | All child rows CASCADE deleted | No effect | Row deleted |

## Migration History

### Migration 001: Fix mwl_attributes FK Constraint (2025-10-29)

**Issue:** The original schema used `ON DELETE CASCADE` for `dicom_tag_id`, which didn't provide protection against accidental deletion of globally-managed tags.

**Change:**
```sql
-- Before
FOREIGN KEY (dicom_tag_id) REFERENCES dicom_tags(id) ON DELETE CASCADE

-- After
FOREIGN KEY (dicom_tag_id) REFERENCES dicom_tags(id) ON DELETE RESTRICT
```

**Files:**
- Migration: `Core/Script/migrations/001_fix_mwl_attributes_fk_constraint.sql`
- Rollback: `Core/Script/migrations/001_fix_mwl_attributes_fk_constraint_rollback.sql`
- Tests: `Test/tst_MwlAttributesForeignKey.cpp`

## Best Practices

### When to Use ON DELETE CASCADE
Use `ON DELETE CASCADE` when:
- The child records are **owned by** the parent
- Child records have no independent meaning without the parent
- Deleting the parent should automatically clean up all children

**Examples in our schema:**
- `mwl_entries` → `mwl_attributes` (attributes are owned by entries)
- `mwl_entries` → `mwl_task_mapping` (task mappings are owned by entries)
- `studies` → `series` → `images` (DICOM hierarchy)

### When to Use ON DELETE RESTRICT
Use `ON DELETE RESTRICT` when:
- The parent record is a **reference/lookup** table
- The parent record should not be deletable while in use
- You want to prevent accidental data loss

**Examples in our schema:**
- `dicom_tags` → `mwl_attributes` (tags are global reference data)
- `views` → `view_techniques` (views should not be deleted while in use)

### When to Use ON DELETE SET NULL
Use `ON DELETE SET NULL` when:
- The relationship is **optional**
- The child can continue to exist without the parent reference
- You want to preserve the child record

**Examples in our schema:**
- `mwl_profiles` → `mwl_entries` (entries can exist without a profile)

## Testing

All foreign key behaviors are validated by automated tests. See:
- `Test/tst_MwlAttributesForeignKey.cpp` - Validates AC1, AC2, AC3

To run FK constraint tests:
```bash
cd out/build/debug
ctest -R tst_MwlAttributesForeignKey -V
```

## References

- MySQL Foreign Key Constraints: https://dev.mysql.com/doc/refman/8.0/en/create-table-foreign-keys.html
- DICOM Standard: https://dicom.nema.org/medical/dicom/current/output/chtml/part03/
