-- Migration: Add admission_id column to studies table
-- Description: Adds DICOM Admission ID (0038,0010) support to the studies table
-- Date: 2025-10-29
-- Author: Etrek Development Team
-- Issue: https://github.com/altay-brusan/etrek/issues/29
--
-- This migration is idempotent and safe to run multiple times.
-- It will only add the column if it doesn't already exist.
--
-- Usage:
--   mysql -u <username> -p <database_name> < migration_add_admission_id.sql
--
-- Or from MySQL client:
--   source /path/to/migration_add_admission_id.sql;

-- Check if the column already exists before adding it
SET @dbname = DATABASE();
SET @tablename = 'studies';
SET @columnname = 'admission_id';
SET @preparedStatement = (SELECT IF(
  (
    SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS
    WHERE
      (TABLE_SCHEMA = @dbname)
      AND (TABLE_NAME = @tablename)
      AND (COLUMN_NAME = @columnname)
  ) > 0,
  'SELECT ''Column admission_id already exists in studies table.'' AS Info;',
  'ALTER TABLE studies ADD COLUMN admission_id VARCHAR(64) DEFAULT NULL COMMENT ''(0038,0010): Admission ID from DICOM Study-Patient module'';'
));

PREPARE alterIfNotExists FROM @preparedStatement;
EXECUTE alterIfNotExists;
DEALLOCATE PREPARE alterIfNotExists;

-- Check if the index already exists before adding it
SET @indexname = 'idx_admission_id';
SET @preparedStatement = (SELECT IF(
  (
    SELECT COUNT(*) FROM INFORMATION_SCHEMA.STATISTICS
    WHERE
      (TABLE_SCHEMA = @dbname)
      AND (TABLE_NAME = @tablename)
      AND (INDEX_NAME = @indexname)
  ) > 0,
  'SELECT ''Index idx_admission_id already exists on studies table.'' AS Info;',
  'CREATE INDEX idx_admission_id ON studies(admission_id);'
));

PREPARE createIndexIfNotExists FROM @preparedStatement;
EXECUTE createIndexIfNotExists;
DEALLOCATE PREPARE createIndexIfNotExists;

-- Verify the migration
SELECT 
    COLUMN_NAME, 
    DATA_TYPE, 
    IS_NULLABLE, 
    COLUMN_DEFAULT, 
    COLUMN_COMMENT
FROM INFORMATION_SCHEMA.COLUMNS
WHERE TABLE_SCHEMA = @dbname
  AND TABLE_NAME = @tablename
  AND COLUMN_NAME = @columnname;

SELECT 'Migration completed successfully!' AS Status;
