-- Rollback Migration: Restore original mwl_attributes FK constraint on dicom_tag_id
-- Date: 2025-10-29
-- Issue: Bugfix - Ensure mwl_attributes does not delete dicom_tags (fix FK delete behavior)
--
-- This script rolls back the migration by restoring the original ON DELETE CASCADE behavior
-- WARNING: This should only be used if there's a critical issue with the migration

-- Step 1: Drop the new FK constraint
ALTER TABLE mwl_attributes
    DROP FOREIGN KEY fk_mwl_attributes_dicom_tag_id;

-- Step 2: Recreate the original FK constraint with ON DELETE CASCADE
ALTER TABLE mwl_attributes
    ADD FOREIGN KEY (dicom_tag_id) REFERENCES dicom_tags(id)
    ON DELETE CASCADE;
