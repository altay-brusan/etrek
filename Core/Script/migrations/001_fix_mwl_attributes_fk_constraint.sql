-- Migration: Fix mwl_attributes FK constraint on dicom_tag_id
-- Date: 2025-10-29
-- Issue: Bugfix - Ensure mwl_attributes does not delete dicom_tags (fix FK delete behavior)
--
-- Problem:
--   The current FK constraint on dicom_tag_id uses ON DELETE CASCADE, which allows
--   deletion of parent dicom_tags records when they are referenced by mwl_attributes.
--   This is semantically incorrect since dicom_tags are global/admin-managed and should
--   not be deletable while referenced.
--
-- Solution:
--   Replace ON DELETE CASCADE with ON DELETE RESTRICT to prevent deletion of referenced
--   dicom_tags records.
--
-- Expected Behavior After Migration:
--   1. Deleting mwl_attributes rows does NOT delete dicom_tags (child → parent never deletes)
--   2. Deleting dicom_tags that are referenced by mwl_attributes is BLOCKED (FK restriction)
--   3. Deleting mwl_entries cascades to mwl_attributes (this FK remains unchanged)

-- Step 1: Get the current FK constraint name for dicom_tag_id
-- Note: MySQL auto-generates FK names like 'mwl_attributes_ibfk_2' if not explicitly named
-- We need to drop it first before recreating with the correct delete behavior

-- First, we need to identify the constraint name
-- In a production migration, you might query INFORMATION_SCHEMA.KEY_COLUMN_USAGE
-- For this migration, we'll use the typical auto-generated name pattern

-- Drop the existing FK constraint on dicom_tag_id
-- The constraint name is typically auto-generated as 'mwl_attributes_ibfk_N'
-- We'll use a dynamic approach to be safe
SET @constraint_name = (
    SELECT CONSTRAINT_NAME 
    FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE 
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'mwl_attributes'
      AND COLUMN_NAME = 'dicom_tag_id'
      AND REFERENCED_TABLE_NAME = 'dicom_tags'
    LIMIT 1
);

-- Prepare and execute the DROP FOREIGN KEY statement
SET @drop_fk_sql = CONCAT('ALTER TABLE mwl_attributes DROP FOREIGN KEY ', @constraint_name);
PREPARE drop_fk_stmt FROM @drop_fk_sql;
EXECUTE drop_fk_stmt;
DEALLOCATE PREPARE drop_fk_stmt;

-- Step 2: Recreate the FK constraint with ON DELETE RESTRICT
ALTER TABLE mwl_attributes
    ADD CONSTRAINT fk_mwl_attributes_dicom_tag_id
    FOREIGN KEY (dicom_tag_id) REFERENCES dicom_tags(id)
    ON DELETE RESTRICT;

-- Note: The FK on mwl_entry_id remains unchanged (ON DELETE CASCADE is correct for that relationship)
