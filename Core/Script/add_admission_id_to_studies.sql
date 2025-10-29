-- Migration: Add admission_id to studies table
-- DICOM Tag: (0038,0010) Admission ID from Patient Study module
-- Date: 2025-10-29
-- Description: Adds admission_id field to studies table to align with DICOM Study/Patient module

-- Add the admission_id column if it doesn't exist
ALTER TABLE studies
ADD COLUMN IF NOT EXISTS admission_id VARCHAR(64) DEFAULT NULL COMMENT '(0038,0010): Admission ID from Patient Study module';

-- Add index for admission_id to support filtering
CREATE INDEX IF NOT EXISTS idx_admission_id ON studies(admission_id);
