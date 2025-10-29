-- Validation Script for admission_id Implementation
-- This script validates that the admission_id column was added correctly

-- Check if admission_id column exists in studies table
SELECT 
    COLUMN_NAME,
    DATA_TYPE,
    IS_NULLABLE,
    COLUMN_DEFAULT,
    COLUMN_COMMENT
FROM INFORMATION_SCHEMA.COLUMNS
WHERE TABLE_SCHEMA = DATABASE()
  AND TABLE_NAME = 'studies'
  AND COLUMN_NAME = 'admission_id';

-- Expected result:
-- COLUMN_NAME: admission_id
-- DATA_TYPE: varchar
-- IS_NULLABLE: YES
-- COLUMN_DEFAULT: NULL
-- COLUMN_COMMENT: (0038,0010): Admission ID from Patient Study module

-- Check if index exists on admission_id
SELECT 
    INDEX_NAME,
    COLUMN_NAME,
    SEQ_IN_INDEX,
    NON_UNIQUE
FROM INFORMATION_SCHEMA.STATISTICS
WHERE TABLE_SCHEMA = DATABASE()
  AND TABLE_NAME = 'studies'
  AND COLUMN_NAME = 'admission_id';

-- Expected result:
-- INDEX_NAME: idx_admission_id
-- COLUMN_NAME: admission_id
-- SEQ_IN_INDEX: 1
-- NON_UNIQUE: 1

-- Test that AdmissionID DICOM tag exists
SELECT 
    id,
    name,
    display_name,
    CONCAT('(', LPAD(HEX(group_hex), 4, '0'), ',', LPAD(HEX(element_hex), 4, '0'), ')') as tag,
    is_active,
    is_retired
FROM dicom_tags
WHERE name = 'AdmissionID';

-- Expected result:
-- name: AdmissionID
-- tag: (0038,0010)
-- is_active: 1
-- is_retired: 0

-- Test that AdmissionID is in worklist field configurations
SELECT 
    id,
    field_name,
    is_enabled
FROM worklist_field_configurations
WHERE field_name = 'AdmissionID';

-- Expected result:
-- field_name: AdmissionID
-- is_enabled: 0 (disabled by default, can be enabled if needed)

-- Sample test: Insert a study with admission_id
-- (This is just for validation - comment out if you don't want to modify data)
/*
INSERT INTO studies (
    study_instance_uid,
    study_id,
    accession_number,
    admission_id,
    study_date,
    study_time
) VALUES (
    'TEST.STUDY.UID.12345',
    'TEST_STUDY_001',
    'ACC001',
    'ADM-2024-001',
    '2024-10-29',
    '14:30:00'
);

-- Verify the insert
SELECT 
    id,
    study_instance_uid,
    study_id,
    accession_number,
    admission_id,
    study_date,
    study_time
FROM studies
WHERE study_instance_uid = 'TEST.STUDY.UID.12345';

-- Clean up test data
DELETE FROM studies WHERE study_instance_uid = 'TEST.STUDY.UID.12345';
*/
