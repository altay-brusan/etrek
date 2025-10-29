-- Migration script: Add hierarchical status tracking to imaging entities
-- Description: Adds status, status_reason, and status_updated_at fields to patients, studies, series, and images tables
-- Date: 2025-10-29

-- Step 1: Create patients table with status tracking
CREATE TABLE IF NOT EXISTS patients (
    id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id VARCHAR(64) NOT NULL,  -- (0010,0020): Patient ID
    patient_name VARCHAR(255) DEFAULT NULL,  -- (0010,0010): Patient Name
    patient_birth_date DATE DEFAULT NULL,  -- (0010,0030): Patient Birth Date
    patient_sex ENUM('M', 'F', 'O', 'U') DEFAULT NULL,  -- (0010,0040): Patient Sex (M=Male, F=Female, O=Other, U=Unknown)
    issuer_of_patient_id VARCHAR(64) DEFAULT NULL,  -- (0010,0021): Issuer of Patient ID
    other_patient_ids VARCHAR(255) DEFAULT NULL,  -- (0010,1000): Other Patient IDs
    patient_comments TEXT DEFAULT NULL,  -- (0010,4000): Patient Comments
    
    -- Status tracking fields
    status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING',
    status_reason TEXT DEFAULT NULL,
    status_updated_at DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
    
    create_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    update_date DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
    
    UNIQUE KEY uk_patient_id_issuer (patient_id, issuer_of_patient_id),
    INDEX idx_patient_status (status),
    INDEX idx_patient_status_updated (status_updated_at)
);

-- Step 2: Add patient_id foreign key to studies table
ALTER TABLE studies 
ADD COLUMN IF NOT EXISTS patient_id INT DEFAULT NULL AFTER id,
ADD CONSTRAINT fk_studies_patient_id FOREIGN KEY (patient_id) REFERENCES patients(id) ON DELETE SET NULL;

-- Step 3: Add status tracking fields to studies table
ALTER TABLE studies
ADD COLUMN IF NOT EXISTS status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING' AFTER allergy,
ADD COLUMN IF NOT EXISTS status_reason TEXT DEFAULT NULL AFTER status,
ADD COLUMN IF NOT EXISTS status_updated_at DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP AFTER status_reason,
ADD INDEX IF NOT EXISTS idx_study_status (status),
ADD INDEX IF NOT EXISTS idx_study_status_updated (status_updated_at);

-- Step 4: Add status tracking fields to series table
ALTER TABLE series
ADD COLUMN IF NOT EXISTS status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING' AFTER presentation_intent_type,
ADD COLUMN IF NOT EXISTS status_reason TEXT DEFAULT NULL AFTER status,
ADD COLUMN IF NOT EXISTS status_updated_at DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP AFTER status_reason,
ADD INDEX IF NOT EXISTS idx_series_status (status),
ADD INDEX IF NOT EXISTS idx_series_status_updated (status_updated_at);

-- Step 5: Add status tracking fields to images table
ALTER TABLE images
ADD COLUMN IF NOT EXISTS status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING' AFTER kvp,
ADD COLUMN IF NOT EXISTS status_reason TEXT DEFAULT NULL AFTER status,
ADD COLUMN IF NOT EXISTS status_updated_at DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP AFTER status_reason,
ADD INDEX IF NOT EXISTS idx_image_status (status),
ADD INDEX IF NOT EXISTS idx_image_status_updated (status_updated_at);

-- Step 6: Create trigger to automatically set status_updated_at on status change for patients
DROP TRIGGER IF EXISTS trg_patients_status_update;
DELIMITER $$
CREATE TRIGGER trg_patients_status_update
BEFORE UPDATE ON patients
FOR EACH ROW
BEGIN
    IF NEW.status != OLD.status THEN
        SET NEW.status_updated_at = CURRENT_TIMESTAMP;
    END IF;
END$$
DELIMITER ;

-- Step 7: Create trigger to automatically set status_updated_at on status change for studies
DROP TRIGGER IF EXISTS trg_studies_status_update;
DELIMITER $$
CREATE TRIGGER trg_studies_status_update
BEFORE UPDATE ON studies
FOR EACH ROW
BEGIN
    IF NEW.status != OLD.status THEN
        SET NEW.status_updated_at = CURRENT_TIMESTAMP;
    END IF;
END$$
DELIMITER ;

-- Step 8: Create trigger to automatically set status_updated_at on status change for series
DROP TRIGGER IF EXISTS trg_series_status_update;
DELIMITER $$
CREATE TRIGGER trg_series_status_update
BEFORE UPDATE ON series
FOR EACH ROW
BEGIN
    IF NEW.status != OLD.status THEN
        SET NEW.status_updated_at = CURRENT_TIMESTAMP;
    END IF;
END$$
DELIMITER ;

-- Step 9: Create trigger to automatically set status_updated_at on status change for images
DROP TRIGGER IF EXISTS trg_images_status_update;
DELIMITER $$
CREATE TRIGGER trg_images_status_update
BEFORE UPDATE ON images
FOR EACH ROW
BEGIN
    IF NEW.status != OLD.status THEN
        SET NEW.status_updated_at = CURRENT_TIMESTAMP;
    END IF;
END$$
DELIMITER ;

-- Migration complete
