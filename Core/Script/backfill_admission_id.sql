-- Backfill Script: Populate admission_id in studies table from worklist data
-- Description: Best-effort backfill of admission_id from mwl_attributes table
-- Date: 2025-10-29
-- Author: Etrek Development Team
-- Issue: https://github.com/altay-brusan/etrek/issues/29
--
-- This script is idempotent and safe to run multiple times.
-- It will only update NULL admission_id values.
--
-- Prerequisites:
--   1. The migration_add_admission_id.sql must have been run first
--   2. Studies table must have admission_id column
--   3. Studies must be linked to worklist entries via study_instance_uid
--
-- Usage:
--   mysql -u <username> -p <database_name> < backfill_admission_id.sql
--
-- Or from MySQL client:
--   source /path/to/backfill_admission_id.sql;

-- Start transaction for safety
START TRANSACTION;

-- Log the current state
SELECT 'Starting backfill process...' AS Status;

SELECT 
    COUNT(*) AS total_studies,
    SUM(CASE WHEN admission_id IS NULL THEN 1 ELSE 0 END) AS studies_without_admission_id,
    SUM(CASE WHEN admission_id IS NOT NULL THEN 1 ELSE 0 END) AS studies_with_admission_id
FROM studies;

-- Temporary table to store the admission_id mappings from worklist
-- This query finds the admission_id for each study by:
-- 1. Joining studies to mwl_entries via study_instance_uid
-- 2. Finding the AdmissionID tag from dicom_tags
-- 3. Getting the tag value from mwl_attributes
CREATE TEMPORARY TABLE IF NOT EXISTS temp_admission_id_mappings AS
SELECT DISTINCT
    s.id AS study_id,
    s.study_instance_uid,
    TRIM(ma.tag_value) AS admission_id_value
FROM studies s
INNER JOIN mwl_entries me ON s.study_instance_uid = me.study_instance_uid
INNER JOIN mwl_attributes ma ON me.id = ma.mwl_entry_id
INNER JOIN dicom_tags dt ON ma.dicom_tag_id = dt.id
WHERE dt.name = 'AdmissionID'
  AND s.admission_id IS NULL  -- Only update studies that don't have admission_id yet
  AND ma.tag_value IS NOT NULL  -- Only use non-null values
  AND TRIM(ma.tag_value) != '';  -- Ignore empty strings

-- Show how many mappings we found
SELECT 
    COUNT(*) AS mappings_found,
    'These studies will be updated with admission_id from worklist data' AS Info
FROM temp_admission_id_mappings;

-- Update studies table with the found admission_id values
UPDATE studies s
INNER JOIN temp_admission_id_mappings t ON s.id = t.study_id
SET s.admission_id = t.admission_id_value
WHERE s.admission_id IS NULL;

-- Log the results
SELECT 
    ROW_COUNT() AS studies_updated,
    'studies were updated with admission_id from worklist data' AS Info;

-- Show final state
SELECT 
    COUNT(*) AS total_studies,
    SUM(CASE WHEN admission_id IS NULL THEN 1 ELSE 0 END) AS studies_without_admission_id,
    SUM(CASE WHEN admission_id IS NOT NULL THEN 1 ELSE 0 END) AS studies_with_admission_id
FROM studies;

-- Clean up temporary table
DROP TEMPORARY TABLE IF EXISTS temp_admission_id_mappings;

-- Commit the changes
COMMIT;

SELECT 'Backfill completed successfully!' AS Status;
