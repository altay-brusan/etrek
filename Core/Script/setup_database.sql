-- ******************[section: authntication and autherization tables]******************

-- Stores user login and audit information.
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_name VARCHAR(255) NOT NULL UNIQUE,
    name VARCHAR(255) NOT NULL,
    surname VARCHAR(255) NOT NULL,
    is_active BOOLEAN NOT NULL DEFAULT TRUE,
    is_deleted BOOLEAN DEFAULT NULL,
    create_date DATETIME(6) DEFAULT CURRENT_TIMESTAMP(6),
    update_date DATETIME(6) DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP(6),
    password_hash VARCHAR(255) NOT NULL
);

-- Stores available user roles (e.g., admin, technician).
CREATE TABLE roles (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE
);

-- Associates users with roles (many-to-many).
CREATE TABLE user_roles (
    user_id INT,
    role_id INT,
    PRIMARY KEY(user_id, role_id),
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(role_id) REFERENCES roles(id)
);


-- ******************[section: device, setup and configurations]******************

CREATE TABLE environment_settings (
  id INT AUTO_INCREMENT PRIMARY KEY,
  study_level ENUM('Multi-Series Study','Single-Series Study') NOT NULL DEFAULT 'Multi-Series Study',
  lookup_table ENUM('VOI LUT','None') NOT NULL DEFAULT 'None',

  worklist_clear_period_days INT NOT NULL DEFAULT 30,
  worklist_refresh_period_seconds INT NOT NULL DEFAULT 60,
  delete_log_period_days INT NOT NULL DEFAULT 90,
  auto_refresh_worklist BOOL NOT NULL DEFAULT TRUE,
  auto_clear_disk_space BOOL NOT NULL DEFAULT TRUE,
  enable_mpps          BOOL NOT NULL DEFAULT TRUE,
  continue_on_echo_fail BOOL NOT NULL DEFAULT TRUE,

  create_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  update_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);


CREATE TABLE image_comments(
    id INT AUTO_INCREMENT PRIMARY KEY,
    is_reject BOOL DEFAULT FALSE,
    heading VARCHAR(128) NOT NULL,
    comment VARCHAR(512) NOT NULL UNIQUE,
    create_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

CREATE TRIGGER trim_image_comments_before_insert
BEFORE INSERT ON image_comments
FOR EACH ROW
SET NEW.`comment` = TRIM(NEW.`comment`);


-- stores metadata about medical or research institutions where equipment is installed
CREATE TABLE institutions (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(128) NOT NULL UNIQUE,         -- (0008,0080): Institution where the equipment is located
    institution_type VARCHAR(128) NULL,             -- Additional optional attribute, you may want to specify if it's a hospital, clinic, etc.
    institution_id VARCHAR(128) NULL,
    department_name VARCHAR(128) NULL,              -- (0008,1040): Department in the institution where the equipment is located
    address VARCHAR(512) NULL,                 -- (0008,0081): Mailing address of the institution
    contact_information VARCHAR(500),          -- Optional: Contact info for the institution
    is_active BOOLEAN DEFAULT TRUE             -- Indicates whether the institution is active or not
);

-- details of supported X-ray detectors characteristics
CREATE TABLE detectors (
    id INT AUTO_INCREMENT PRIMARY KEY,    
    manufacturer VARCHAR(255) NOT NULL,
    model_name VARCHAR(128) NULL,                  -- (0008,1090): Manufacturer's model name
    resolution VARCHAR(64) NULL,                   -- (0054,0220): Resolution in terms of pixel size
    size VARCHAR(64) NULL,                         -- (0054,0222): Physical size (e.g., 14" x 17")
    pixel_width DECIMAL(6, 3) NULL,                -- (0054,0224): Pixel width in mm (e.g., 0.1 mm)
    pixel_height DECIMAL(6, 3) NULL,               -- (0054,0225): Pixel height in mm (e.g., 0.1 mm)
    sensitivity DECIMAL(6, 3) NULL,                -- (0054,0226): Sensitivity of the detector
    imager_pixel_spacing VARCHAR(64) NULL,         -- (0018,1164): Imager Pixel Spacing (row_spacing,column_spacing in mm) "0.1,0.1"
    identifier VARCHAR(128) NULL,                  -- (0018,700A): The ID or serial number of the detector used to acquire this image.
    space_lut_reference VARCHAR(255) NULL,         -- (0054,0228): Reference to LUT (e.g., file path, ID in LUT table)
    spatial_resolution DECIMAL(6, 3) NULL,         -- (0018,1050): Spatial resolution in mm
    scan_options VARCHAR(128) NULL,                -- (0018,0022): Scan parameters (e.g., scanning technique)
    width INT NULL,
    height INT NULL,
    width_shift INT NULL,
    height_shift INT NULL,
    saturation_value int,    
    is_active bool DEFAULT FALSE,
    create_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);



-- Details of supported X-ray tube characteristics
-- This table is also intentionally denormalized for easier read/write operations.
-- Relation between generator and X-ray tube is one-to-many (1 generator → 1 or 2 tubes).
-- The supported tube list is fixed. End users cannot add or remove records.
-- Users can only specify installation-specific parameters like date, serial number, or technical parameters.
-- All other records should be managed by the software provider after validation.

CREATE TABLE xray_tubes (
    id INT AUTO_INCREMENT PRIMARY KEY,
    manufacturer VARCHAR(255) NOT NULL,
    model_number VARCHAR(100) NULL,
    part_number VARCHAR(100) NULL,
    serial_number VARCHAR(100) NOT NULL UNIQUE,
    type_number VARCHAR(100) NULL,
    focal_spot VARCHAR(100) NULL,
    anode_heat_capacity INT NULL,    -- in HU
    cooling_rate INT NULL,           -- in HU/min
    max_voltage INT NULL,            -- in kV
    max_current INT NULL,            -- in mA
    power_range VARCHAR(100) NULL,   -- in kW
    tube_filter VARCHAR(100) DEFAULT NULL,
    is_active BOOLEAN DEFAULT FALSE,
    manufacture_date DATE NULL,
    installation_date DATE NULL,    
    create_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);


-- Details of supported X-ray generator characteristics
-- This table is intentionally denormalized: it includes manufacturer, installation, and configuration data.
-- Denormalization is intentional to simplify read/write operations.
-- Each generator can be connected to one or more X-ray tubes (currently 1:2 relation).
-- The supported generator list is fixed. End users are not allowed to add or remove records.
-- Only the software provider can add records after testing, validation, and integration.
-- Users can specify certain parameters such as installation date, technical specs, or serial number of purchased devices.
-- In the future, if more outputs are needed, this table can be split into separate Generator and Generator_Installation tables.

CREATE TABLE generators (
    id INT AUTO_INCREMENT PRIMARY KEY,
    manufacturer VARCHAR(255) NOT NULL,
    model_number VARCHAR(100) NULL,
    part_number VARCHAR(100) NULL,
    serial_number VARCHAR(100) NULL,
    type_number VARCHAR(100) NULL,
    technical_specifications VARCHAR(1024) NULL,
    manufacture_date DATE NULL,
    installation_date DATE NULL,
    calibration_date DATE NULL,
    output1 INT NULL,
    output2 INT NULL,
    is_active BOOLEAN DEFAULT FALSE,
    is_output1_active BOOLEAN DEFAULT FALSE,
    is_output2_active BOOLEAN DEFAULT FALSE,
    create_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);


-- equipment setup details installed in healthcare institutions
CREATE TABLE general_equipments (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_serial_number VARCHAR(64) NOT NULL UNIQUE, -- (0018,1000)
    device_uid VARCHAR(64) DEFAULT NULL,                           -- (0018,1002): Unique identifier for the equipment
    manufacturer VARCHAR(128) DEFAULT NULL,                        -- (0008,0070)
    model_name VARCHAR(128) DEFAULT NULL,            -- (0008,1090)
    station_name VARCHAR(64) DEFAULT NULL,                         -- (0008,1010)
    institution_id INT,                               -- (0008,0080): Foreign key referring to the institution
    department_name VARCHAR(128) DEFAULT NULL,                     -- (0008,1040)
    date_of_last_calibration DATE DEFAULT NULL,                    -- (0018,1200): Date of the last calibration for the equipment
    time_of_last_calibration TIME DEFAULT NULL,                    -- (0018,1201): Time of the last calibration for the equipment
    date_of_manufacture DATE DEFAULT NULL,                         -- (0018,1204): Date when the equipment was manufactured or re-manufactured
    software_versions VARCHAR(100) DEFAULT NULL,                           -- (0018,1020): Manufacturer's software version(s)
    gantry_id VARCHAR(255) DEFAULT NULL,                           -- (0018,1008): Identifier for the gantry or positioner
    create_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,                   -- Indicates whether the device is active or not
    FOREIGN KEY (institution_id) REFERENCES institutions(id) -- Foreign key reference to the institution table
);

CREATE TABLE detector_setup (
        id INT AUTO_INCREMENT PRIMARY KEY,
        device_id INT,
        equipment_id INT,
        horizontal_flip bit DEFAULT false, -- Indicates if the detector is horizontally flipped
        crop_mod ENUM('None' ,'Software','Hardware'),
        save_raw_data ENUM('No save','Before calibration','After calibration','All'),
        detector_order ENUM ('Detector1','Detector2') DEFAULT 'Detector1',
        position ENUM('Stand','Table','Motorized','Portable') NULL,
        calibration_date DATE NULL,              -- (0018,1200): Date of last calibration
        is_active BIT DEFAULT b'0',

        -- Generated column to enforce uniqueness only when active
        active_key VARCHAR(255) GENERATED ALWAYS AS (
            CASE 
                WHEN is_active = b'1' 
                THEN CONCAT(equipment_id, '_', detector_order, '_', position) 
            END
        ) STORED,
        -- Unique constraint on active rows
        create_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

        UNIQUE KEY uq_active_tube (active_key),    
        FOREIGN KEY (device_id) REFERENCES detectors(id),
        FOREIGN KEY (equipment_id) REFERENCES general_equipments(id)
);

CREATE TABLE xray_tube_setup (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id INT,
    equipment_id INT,
    tube_order ENUM ('Tube1','Tube2') DEFAULT 'Tube1',
    position ENUM('Stand','Table','Motorized','Portable') DEFAULT 'Stand',
    tube_name VARCHAR(100) NULL, -- e.g., "Ceiling Tube", "Table Tube"
    is_active BIT DEFAULT b'0',
    calibration_date DATE NULL,     
    create_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

    -- Generated column to enforce uniqueness only when active
    active_key VARCHAR(255) GENERATED ALWAYS AS (
        CASE 
            WHEN is_active = b'1' 
            THEN CONCAT(equipment_id, '_', tube_order, '_', position) 
        END
    ) STORED,

    -- Unique constraint on active rows
    UNIQUE KEY uq_active_tube (active_key),

    FOREIGN KEY (device_id) REFERENCES xray_tubes(id),
    FOREIGN KEY (equipment_id) REFERENCES general_equipments(id)
);


-- details of supported positions and movements
CREATE TABLE positioners (
    id INT AUTO_INCREMENT PRIMARY KEY,
    position_name VARCHAR(100) NOT NULL UNIQUE, -- unique position name
    description VARCHAR(255) NULL,
    create_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);


CREATE TABLE positioner_steps (
    id INT AUTO_INCREMENT PRIMARY KEY,
    positioner_id INT NOT NULL,          -- FK to positioners
    role ENUM('TableDetector','StandDetector','PortableDetector',
              'PrimaryTube','SecondaryTube') NOT NULL,
    motion_code VARCHAR(64) NOT NULL,    -- e.g., "X-2331" or "D-1234"
    step_order INT DEFAULT 1,            -- if multiple moves required
    FOREIGN KEY (positioner_id) REFERENCES positioners(id)
);

-- details of connection channels
CREATE TABLE device_connections (
    id INT AUTO_INCREMENT PRIMARY KEY,
    generator_id INT NULL,
    detector_id INT NULL,
    tube_id INT NULL,
    positioner_id INT NULL,
    connector ENUM('SyncBox','Direct') NULL,    
    protocol ENUM('RS_232','RS_485','CAN','LAN','MODBUS_RTU','MODBUS_TCP','WIFI','USB','ANALOG') NOT NULL,
    interface_name VARCHAR(100) NULL,
    parameters JSON,
    create_date DATETIME(6) DEFAULT CURRENT_TIMESTAMP(6),
    update_date DATETIME(6) DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP(6),

    FOREIGN KEY (generator_id) REFERENCES generators(id) ON DELETE CASCADE,
    FOREIGN KEY (detector_id) REFERENCES detectors(id) ON DELETE CASCADE,
    FOREIGN KEY (tube_id) REFERENCES xray_tubes(id) ON DELETE CASCADE,
    FOREIGN KEY (positioner_id) REFERENCES positioners(id) ON DELETE CASCADE
);



-- ****************[section: pacs ]***********************

-- details of connection for pcas nodes
CREATE TABLE pacs_nodes (
    id INT AUTO_INCREMENT PRIMARY KEY,
    entity_type ENUM('Archive','MPPS') NOT NULL,
    host_name VARCHAR(128) NOT NULL,
    host_ip VARCHAR(45) NOT NULL,
    host_port INT NOT NULL,
    called_aet VARCHAR(20) NOT NULL,
    calling_aet VARCHAR(20) NOT NULL,
    is_default BOOLEAN DEFAULT FALSE,    
    create_date DATETIME(6) DEFAULT CURRENT_TIMESTAMP(6),
    update_date DATETIME(6) DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP(6)    
);


-- ****************[section: modality worklist ]***********************

-- primary key fields used by worklist.
CREATE TABLE worklist_field_configurations(
    id INT AUTO_INCREMENT PRIMARY KEY,
    field_name ENUM('PatientID', 'AccessionNumber','OtherPatientID','AdmissionID','ScheduledProcedureStepID','RequestedProcedureID','RequestedProcedureCodeValue','StudyInstanceUID') NOT NULL UNIQUE,
    is_enabled bool NOT NULL
);

-- MWL profiles.
CREATE TABLE mwl_profiles (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE       -- Profile name (e.g. 'DefaultMWLQuery', 'UltrasoundTags')
);

-- Transfer syntaxes and SOP classes associated with a MWL profile.
CREATE TABLE mwl_presentation_contexts (
    id INT AUTO_INCREMENT PRIMARY KEY,
    profile_id INT NOT NULL,                -- Foreign key to mwl_profiles.id
    transfer_syntax_uid VARCHAR(128) NOT NULL, -- Transfer Syntax UID (e.g. LittleEndianExplicit)
    FOREIGN KEY (profile_id) REFERENCES mwl_profiles(id) ON DELETE CASCADE
);

-- DICOM tag dictionary with tag identifiers and metadata.
CREATE TABLE dicom_tags (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE,     -- Tag standard name (e.g. 'AccessionNumber')
    display_name VARCHAR(255) NOT NULL,    -- Tag display name (e.g. 'AccessionNumber')
    group_hex INT UNSIGNED NOT NULL,       -- Group part of DICOM tag (e.g. 0x0008)
    element_hex INT UNSIGNED NOT NULL,     -- Element part of DICOM tag (e.g. 0x0050)
    pgroup_hex INT UNSIGNED NOT NULL,      -- Parent group for sequences (or 0 if none)
    pelement_hex INT UNSIGNED NOT NULL,    -- Parent element for sequences (or 0 if none)
    is_active BOOLEAN DEFAULT TRUE,         -- Indicates if tag is currently enabled globally
    is_retired BOOLEAN DEFAULT FALSE       -- Indicates if tag is retired/deprecated
);

-- Links MWL profiles to DICOM tags, marking identifiers/mandatory.
CREATE TABLE profile_tag_association (
    profile_id INT NOT NULL,                -- Foreign key to mwl_profiles.id
    tag_id INT NOT NULL,                    -- Foreign key to dicom_tags.id
    is_identifier BOOLEAN DEFAULT FALSE,    -- This tag is being used for identification (active)
    is_mandatory BOOLEAN DEFAULT FALSE,     -- If true, must be in is_identifier = true
    PRIMARY KEY (profile_id, tag_id),
    FOREIGN KEY (profile_id) REFERENCES mwl_profiles(id) ON DELETE CASCADE,
    FOREIGN KEY (tag_id) REFERENCES dicom_tags(id) ON DELETE CASCADE
);

-- Stores each Modality Worklist (MWL) entry received (queue).
CREATE TABLE mwl_entries (
    id INT AUTO_INCREMENT PRIMARY KEY,                  -- Unique worklist entry ID
    source ENUM('LOCAL', 'RIS') NOT NULL,               -- Source system of the worklist entry
    profile_id INT NULL,                                 -- Foreign key to the profile used for this entry
    status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING', -- Status of the scheduled procedure
    study_instance_uid VARCHAR(64),
    created_at DATETIME DEFAULT NULL,                    -- Entry creation time, default is NULL
    updated_at DATETIME DEFAULT NULL,                    -- Entry update time, default is NULL, will be updated explicitly
    FOREIGN KEY (profile_id) REFERENCES mwl_profiles(id) ON DELETE SET NULL
);

-- Stores actual tag values from DICOM MWL responses for each entry.
CREATE TABLE mwl_attributes (
    id INT AUTO_INCREMENT PRIMARY KEY,
    mwl_entry_id INT NOT NULL,               -- Foreign key to mwl_entries.id (worklist entry)
    dicom_tag_id INT NOT NULL,               -- Foreign key to dicom_tags.id (the actual DICOM tag)
    tag_value VARCHAR(512) DEFAULT NULL,       -- Value of the tag (as a string)
    FOREIGN KEY (mwl_entry_id) REFERENCES mwl_entries(id) ON DELETE CASCADE,  -- Cascade deletes with worklist entry
    FOREIGN KEY (dicom_tag_id) REFERENCES dicom_tags(id) ON DELETE CASCADE   -- Cascade deletes with dicom tag
);

-- section DICOM: https://dicom.nema.org/medical/dicom/2023c/output/chtml/part03/sect_A.26.3.html

-- Stores Patient-level DICOM metadata with hierarchical status tracking.
CREATE TABLE patients (
    id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id VARCHAR(64) NOT NULL,  -- (0010,0020): Patient ID
    patient_name VARCHAR(255) DEFAULT NULL,  -- (0010,0010): Patient Name
    patient_birth_date DATE DEFAULT NULL,  -- (0010,0030): Patient Birth Date
    patient_sex ENUM('M', 'F', 'O', 'U') DEFAULT NULL,  -- (0010,0040): Patient Sex (M=Male, F=Female, O=Other, U=Unknown)
    issuer_of_patient_id VARCHAR(64) DEFAULT NULL,  -- (0010,0021): Issuer of Patient ID
    other_patient_ids VARCHAR(255) DEFAULT NULL,  -- (0010,1000): Other Patient IDs
    patient_comments TEXT DEFAULT NULL,  -- (0010,4000): Patient Comments
    status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING',
    status_reason TEXT DEFAULT NULL,
    status_updated_at DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
    create_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    update_date DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uk_patient_id_issuer (patient_id, issuer_of_patient_id),
    INDEX idx_patient_status (status),
    INDEX idx_patient_status_updated (status_updated_at)
);

-- Stores Study-level DICOM metadata and links to MWL entries.
CREATE TABLE studies (
    id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id INT DEFAULT NULL,
    study_instance_uid VARCHAR(64) NOT NULL,
    study_id VARCHAR(64) DEFAULT NULL,  -- (0020,0010): User or equipment generated Study identifier.
    accession_number VARCHAR(64) DEFAULT NULL,  -- (0008,0050)
    issuer_of_accession_number VARCHAR(128) DEFAULT NULL,  -- (0008,0051)
    referring_physician_name VARCHAR(128) DEFAULT NULL,  -- (0008,0090)
    study_date DATE DEFAULT NULL,  -- (0008,0020)
    study_time TIME DEFAULT NULL,  -- (0008,0030)
    study_description VARCHAR(255) DEFAULT NULL,  -- (0008,1030)
    patient_age INT DEFAULT NULL,  -- (0010,1010)
    patient_size INT DEFAULT NULL,  -- (0010,1020)
    allergy VARCHAR(255) DEFAULT NULL,  -- (0010,2110)
    status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING',
    status_reason TEXT DEFAULT NULL,
    status_updated_at DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (patient_id) REFERENCES patients(id) ON DELETE SET NULL,
    INDEX idx_study_status (status),
    INDEX idx_study_status_updated (status_updated_at)
);

-- Stores Series-level metadata linked to a Study.
CREATE TABLE series (
    id INT AUTO_INCREMENT PRIMARY KEY,
    study_id INT NOT NULL,
    series_instance_uid VARCHAR(64) NOT NULL,
    series_number INT DEFAULT NULL,  -- Optional: (0008,1030)
    modality VARCHAR(10) DEFAULT NULL,  -- (0008,0060)
    series_description VARCHAR(255) DEFAULT NULL,  -- (0008,103E)
    operator_name VARCHAR(128) DEFAULT NULL,  -- (0008,1070)
    body_part_examined VARCHAR(64) DEFAULT NULL,  -- (0018,0015)
    patient_position VARCHAR(64) DEFAULT NULL,  -- (0018,5100)
    view_position VARCHAR(64) DEFAULT NULL,  -- (0018,5101)
    image_laterality ENUM('R', 'L', 'B', 'U') DEFAULT NULL,  -- (0020,0062)
    acquisition_device_id INT DEFAULT NULL,
    presentation_intent_type ENUM('FOR PRESENTATION', 'FOR PROCESSING') DEFAULT NULL,  -- (0008,0060)
    status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING',
    status_reason TEXT DEFAULT NULL,
    status_updated_at DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (acquisition_device_id) REFERENCES general_equipments(id),
    FOREIGN KEY (study_id) REFERENCES studies(id) ON DELETE CASCADE,
    INDEX idx_series_status (status),
    INDEX idx_series_status_updated (status_updated_at)
);

-- Stores dynamic settings and processing information for each image, such as patient orientation, contrast agents, image compression, and processing steps.
CREATE TABLE images (
    id INT AUTO_INCREMENT PRIMARY KEY,
    study_id INT NOT NULL,
    series_id INT NOT NULL,
    instance_number VARCHAR(64) DEFAULT NULL,  -- (0020,0013)
    content_date DATE DEFAULT NULL,  -- (0008,0023)
    content_time TIME DEFAULT NULL,  -- (0008,0033)
    image_type VARCHAR(255) DEFAULT NULL,  -- (0008,0008)
    image_comments TEXT DEFAULT NULL,  -- (0020,4000)
    quality_control_image ENUM('YES', 'NO', 'BOTH') DEFAULT NULL,  -- (0028,0300)
    samples_per_pixel VARCHAR(128), -- (0028,0002)
    photometric_interpretation VARCHAR(128), -- (0028,0004)
    row_count INT, -- (0028,0010)
    columns_count INT, -- (0028,0011)
    bits_allocated INT, -- (0028,0100)
    bits_stored INT, -- (0028,0101)
    high_bit INT, -- (0028,0102)
    pixel_representation BIT, -- (0028,0103) 0: (unsigned integer), 1:  (2's complement)
    pixel_aspect_ratio VARCHAR(64),
    contrast_agent VARCHAR(64) DEFAULT NULL,  -- (0018,0010)
    image_laterality ENUM('R', 'L', 'B', 'U') DEFAULT NULL,  -- (0020,0062)
    pixel_intensity_relationship VARCHAR(10) DEFAULT NULL,  -- (0028,1040)
    pixel_intensity_sign ENUM('+1', '-1') DEFAULT NULL,  -- (0028,1041)
    rescale_intercept DECIMAL(10, 2) DEFAULT NULL,  -- (0028,1052)
    rescale_slope DECIMAL(10, 2) DEFAULT NULL,  -- (0028,1053)
    rescale_type VARCHAR(10) DEFAULT NULL,  -- (0028,1054)
    presentation_lut_shape ENUM('IDENTITY', 'INVERSE') DEFAULT NULL,  -- (2050,0020)
    lossy_image_compression ENUM('00', '01') DEFAULT NULL,  -- (0028,2110)
    lossy_image_compression_ratio DECIMAL(5, 2) DEFAULT NULL,  -- (0028,2112)
    derivation_description TEXT DEFAULT NULL,  -- (0008,2111)
    acquisition_device_processing_description TEXT DEFAULT NULL,  -- (0018,1400)
    acquisition_device_processing_code VARCHAR(128) DEFAULT NULL,  -- (0018,1401)
    patient_orientation VARCHAR(128) DEFAULT NULL,  -- (0020,0020)
    calibration_image ENUM('YES', 'NO') DEFAULT NULL,  -- (0050,0004)
    burned_in_annotation ENUM('YES', 'NO') DEFAULT NULL,  -- (0028,0301)
    detector_binning VARCHAR(64) DEFAULT NULL,  -- (0018,701A)
    detector_mode VARCHAR(64) DEFAULT NULL,  -- (0018,7008)
    frame_increment_pointer VARCHAR(64) DEFAULT NULL,  -- (0028,0009)
    frame_label_vector TEXT DEFAULT NULL,  -- (0018,2002)
    kvp INT DEFAULT NULL, -- (0018,0060)
    status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING',
    status_reason TEXT DEFAULT NULL,
    status_updated_at DATETIME DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (study_id) REFERENCES studies(id) ON DELETE CASCADE,
    FOREIGN KEY (series_id) REFERENCES series(id) ON DELETE CASCADE,
    INDEX idx_image_status (status),
    INDEX idx_image_status_updated (status_updated_at)
);

-- SOP common module
CREATE TABLE sop_commons (
    id INT AUTO_INCREMENT PRIMARY KEY,
    image_id INT,  -- Foreign key to the Image table
    sop_class_uid VARCHAR(64) NOT NULL,  -- (0008,0016)
    sop_instance_uid VARCHAR(64) NOT NULL,  -- (0008,0018)
    specific_character_set VARCHAR(64) DEFAULT NULL,  -- (0008,0005)
    instance_creation_date DATE DEFAULT NULL,  -- (0008,0012)
    instance_creation_time TIME DEFAULT NULL,  -- (0008,0013)
    instance_coercion_datetime DATETIME DEFAULT NULL,  -- (0008,0015)
    instance_creator_uid VARCHAR(64) DEFAULT NULL,  -- (0008,0014)
    related_general_sop_class_uid VARCHAR(64) DEFAULT NULL,  -- (0008,001A)
    original_specialized_sop_class_uid VARCHAR(64) DEFAULT NULL,  -- (0008,001B)
    coding_scheme_identification_sequence TEXT DEFAULT NULL,  -- (0008,0110)
    contributing_equipment_sequence TEXT DEFAULT NULL,  -- (0018,A001)
    FOREIGN KEY (image_id) REFERENCES images(id) ON DELETE CASCADE
);

-- Acquisition module
CREATE TABLE acquisitions (
    id INT AUTO_INCREMENT PRIMARY KEY,
    study_id INT NOT NULL,  -- Link to the Study
    series_id INT NOT NULL,  -- Link to the Series
    acquisition_uid VARCHAR(64) NOT NULL,  -- (0008,0017)
    acquisition_date DATE DEFAULT NULL,  -- (0008,0022)
    acquisition_time TIME DEFAULT NULL,  -- (0008,0032)
    acquisition_duration INT DEFAULT NULL,  -- (0018,9073)
    acquisition_device_id INT DEFAULT NULL,  -- Reference to acquisition device used
    radiation_dose DOUBLE DEFAULT NULL,  -- Optional: Radiation dose used during acquisition
    aec_position INT DEFAULT NULL,  -- (0028,0301): Add AEC Density here as it relates to exposure control
    FOREIGN KEY (study_id) REFERENCES studies(id) ON DELETE CASCADE,
    FOREIGN KEY (series_id) REFERENCES series(id) ON DELETE CASCADE,
    FOREIGN KEY (acquisition_device_id) REFERENCES general_equipments(id)
);

-- section Anatomic Region Codes and Body Part Examined Defined Terms
CREATE TABLE anatomic_regions (
    id                 INT AUTO_INCREMENT PRIMARY KEY,
    name               VARCHAR(64)  NOT NULL UNIQUE,      -- your UI/bucket name (e.g., 'HEAD')
    code_value         VARCHAR(32)  NOT NULL,             -- SNOMED code value used by DICOM
    coding_scheme      VARCHAR(16)  NOT NULL DEFAULT 'SRT', -- 'SRT' (SNOMED RT) widely used, some systems use 'SCT'
    code_meaning       VARCHAR(128) NOT NULL,             -- human meaning from the standard
    description        VARCHAR(255) NULL,
    display_order      INT          NOT NULL DEFAULT 0,
    UNIQUE KEY uq_region_code (code_value, coding_scheme)
);

-- Standardized list of anatomical body parts (DICOM-compliant).
CREATE TABLE body_parts (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE,
    code_value  VARCHAR(50),
    coding_scheme VARCHAR(20) DEFAULT 'SRT',
    description VARCHAR(255),
    anatomic_region_id INT NOT NULL,
    FOREIGN KEY (anatomic_region_id) REFERENCES anatomic_regions(id),
    is_active BOOLEAN DEFAULT TRUE
);

-- Exposure parameters associated with a view and patient size.
CREATE TABLE technique_parameters (
    id INT AUTO_INCREMENT PRIMARY KEY,
    body_part_id INT NOT NULL, -- may not required, but could speed up the retrival process
    size ENUM('Fat', 'Medium', 'Thin', 'Paediatric') NOT NULL,
    technique_profile ENUM('AP|PA','LAT','OBL','AXIAL') DEFAULT 'AP|PA',
    kvp INT,
    ma INT,
    ms INT,
    fkvp INT,
    fma DECIMAL(4,2),
    focal_spot INT DEFAULT NULL,
    sid_min INT,
    sid_max INT,
    grid_type ENUM('Parallel','Focused','Crossed','Moving','Virtual') DEFAULT NULL,
    grid_ratio VARCHAR(50) DEFAULT NULL,
    grid_speed VARCHAR(50) DEFAULT NULL,
    exposure_style ENUM('Mas Mode','Time Mode','AEC Mode','Manual') DEFAULT NULL,
    aec_field VARCHAR(10),
    aec_density INT,
    FOREIGN KEY (body_part_id) REFERENCES body_parts(id));

-- Defines anatomical imaging views or orientations (e.g., AP, LAT).
-- AP (Anterior → Posterior): X-ray enters anterior side, exits posterior.
-- PA (Posterior → Anterior): X-ray enters posterior side, exits anterior.
-- LL (Left Lateral): Beam passes left → right, detector on right side.
-- RL (Right Lateral): Beam passes right → left, detector on left side.
-- RLD (Right Lateral Decubitus): Patient lying on right side, beam horizontal.
-- LLD (Left Lateral Decubitus): Patient lying on left side, beam horizontal.
-- RLO (Right Lateral Oblique): Beam enters right anterior oblique side.
-- LLO (Left Lateral Oblique): Beam enters left anterior oblique side.

CREATE TABLE views (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(20) NOT NULL,  -- e.g., "AP", "LAT", "OBL"
    description VARCHAR(255) DEFAULT NULL,  -- Optional description (e.g., "Anteroposterior view")
    body_part_id INT NOT NULL,

    -- DICOM tag (0018,5100): Patient Position
    patient_position ENUM('Supine','Prone','Erect','Lateral') DEFAULT NULL,

    -- Custom imaging orientation
    projection_profile ENUM('AP|PA','LAT','OBL','AXIAL','DUAL') DEFAULT 'AP|PA',

    -- DICOM-derived patient orientation directions (0020,0020)
    patient_orientation_row ENUM(
        'L','R','A','P','H','F',
        'LA','LP','LH','LF',
        'RA','RP','RH','RF',
        'AL','AR','AH','AF',
        'PL','PR','PH','PF',
        'HL','HR','HA','HP',
        'FL','FR','FA','FP'
    ) DEFAULT NULL,

    patient_orientation_col ENUM(
        'L','R','A','P','H','F',
        'LA','LP','LH','LF',
        'RA','RP','RH','RF',
        'AL','AR','AH','AF',
        'PL','PR','PH','PF',
        'HL','HR','HA','HP',
        'FL','FR','FA','FP'
    ) DEFAULT NULL,
    view_position ENUM('AP','PA','LL','RL','RLD','LLD','RLO','LLO'), -- (0018,5101) View Position
    image_laterality ENUM('L','R','B') DEFAULT NULL, -- (0020,0062) Image Laterality Attribute
    image_rotate TINYINT DEFAULT NULL,               -- (0070,0042) Image Rotation Attribute
    icon_file_location VARCHAR(1024) DEFAULT NULL,
    collimator_size varchar(64) DEFAULT NULL,
    image_processing_algorithm varchar(64) DEFAULT NULL,
    image_horizontal_flip bit default false,
    label_mark ENUM('R','L','PA','AP','LAT','OBL','FLAT','UPRT') DEFAULT NULL,
    label_position ENUM('LEFT TOP','RIGHT TOP','LEFT BOTTOM','RIGHT BOTTOM') DEFAULT NULL,
    position_name VARCHAR(100) DEFAULT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    FOREIGN KEY (body_part_id) REFERENCES body_parts(id), 
    FOREIGN KEY (position_name) REFERENCES positioners(position_name)
);


CREATE TABLE view_techniques (
    view_id INT NOT NULL,
    technique_parameter_id INT NOT NULL,

    -- execution order for exposures in this view (1,2,...)
    seq TINYINT NOT NULL DEFAULT 1,

    -- role of the attached exposure in this view
    role ENUM('PRIMARY','LOW','HIGH') NOT NULL DEFAULT 'PRIMARY',

    PRIMARY KEY (view_id, seq),
    UNIQUE KEY uq_view_role (view_id, technique_parameter_id),

    FOREIGN KEY (view_id)
        REFERENCES views(id)
        ON DELETE CASCADE,

    FOREIGN KEY (technique_parameter_id)
        REFERENCES technique_parameters(id)
        ON DELETE RESTRICT
);


-- Catalog of imaging procedures, including standard and custom.
-- for custom procedures we can have views from different body-pars and anatomic-regions.
-- So, the relation between procedure and body-part/anatomic-region is many-to-many.
-- the body-part-id and anatomic-region-id are optional and should be filled for standard procedures.
CREATE TABLE procedures (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,           -- e.g., "CSpine AP"
    code_value VARCHAR(50),               -- e.g., "T-11100" (DICOM Code)
    coding_scheme VARCHAR(10) DEFAULT 'SRT',  -- Typically 'SRT' for SNOMED CT
    code_meaning VARCHAR(255),            -- Human-readable meaning
    is_true_size BIT DEFAULT FALSE,
	print_orientation ENUM('Landscape','Portrait') DEFAULT  NULL,
    print_format ENUM('STANDARD\1,1','STANDARD\1,2','STANDARD\2,1','STANDARD\2,2') DEFAULT NULL,
	anatomic_region_id INT DEFAULT NULL,  -- no need to assign anatomic-region
    body_part_id INT DEFAULT NULL,        
    is_active BOOLEAN DEFAULT TRUE,
    created_at DATETIME(6) DEFAULT CURRENT_TIMESTAMP(6),
    updated_at DATETIME(6) DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP(6)
);

-- Associates procedures with their required views (many-to-many).
CREATE TABLE procedure_views (
    procedure_id INT NOT NULL,
    view_id INT NOT NULL,
    PRIMARY KEY (procedure_id, view_id),
    FOREIGN KEY (procedure_id) REFERENCES procedures(id) ON DELETE CASCADE,
    FOREIGN KEY (view_id) REFERENCES views(id) ON DELETE CASCADE
);

-- Links MWL entries to internal procedure definitions.
CREATE TABLE mwl_task_mapping (
    mwl_entry_id INT NOT NULL,
    procedure_id INT NOT NULL,
    study_id INT NOT NULL,
    series_id INT NOT NULL,
    images_id INT NOT NULL,
    sop_common_id INT NOT NULL,
    acquisition_id INT NOT NULL,
    PRIMARY KEY (mwl_entry_id, procedure_id),
    FOREIGN KEY (mwl_entry_id) REFERENCES mwl_entries(id) ON DELETE CASCADE,
    FOREIGN KEY (study_id) REFERENCES studies(id) ON DELETE CASCADE,
    FOREIGN KEY (series_id) REFERENCES series(id) ON DELETE CASCADE,
    FOREIGN KEY (images_id) REFERENCES images(id) ON DELETE CASCADE,
    FOREIGN KEY (sop_common_id) REFERENCES sop_commons(id) ON DELETE CASCADE,
    FOREIGN KEY (acquisition_id) REFERENCES acquisitions(id) ON DELETE CASCADE
);

-- **************[Insert DICOM tags into dicom_tags table]************************

-- roles using system
INSERT INTO roles (Name)
VALUES 
  ('Admin'),
  ('Radiographer'),
  ('Engineer');


INSERT INTO dicom_tags (name, display_name, group_hex, element_hex, pgroup_hex, pelement_hex, is_active, is_retired) VALUES

('RequestAttributesSequence','Request Attributes Sequence', 0x0040, 0x0275, 0x0000, 0x0000, TRUE, FALSE),
-- patient module
('PatientName','Patient Name', 0x0010, 0x0010, 0x0000, 0x0000, TRUE, FALSE),
('PatientID','Patient ID', 0x0010, 0x0020, 0x0000, 0x0000, TRUE, FALSE),
('IssuerOfPatientID','Issuer of Patient ID', 0x0010, 0x0021, 0x0000, 0x0000, TRUE, FALSE),
('TypeOfPatientID','Type of Patient ID', 0x0010, 0x0022, 0x0000, 0x0000, TRUE, FALSE), -- Defined Terms: TEXT, RFID, BARCODE
('IssuerOfPatientIDQualifiersSequence','Issuer of Patient ID Qualifiers Sequence', 0x0010, 0x0024, 0x0000, 0x0000, TRUE, FALSE),
('OtherPatientID','Other Patient ID', 0x0010, 0x1000, 0x0000, 0x0000, TRUE, TRUE), -- replaced by (0010,1002): Other Patient IDs Sequence
('PatientSex', 'Patient Sex', 0x0010, 0x0040, 0x0000, 0x0000, TRUE, FALSE),
('PatientBirthDate','Patient Birth Date', 0x0010, 0x0030, 0x0000, 0x0000, TRUE, FALSE),
('PatientComments','Patient Comments', 0x0010, 0x4000, 0x0000, 0x0000, TRUE, FALSE),
('PatientAllergies','Patient Allergies', 0x0010, 0x2110, 0x0000, 0x0000, TRUE, FALSE), -- renamed
('RequestingPhysician','Requesting Physician', 0x0032, 0x1032, 0x0000, 0x0000, TRUE, FALSE),
-- study modules
('PatientAge', 'Patient Age', 0x0010, 0x1010, 0x0000, 0x0000, TRUE, FALSE), 
('AccessionNumber','Accession Number', 0x0008, 0x0050, 0x0000, 0x0000, TRUE, FALSE),
('AdmissionID','Admission ID', 0x0038, 0x0010, 0x0000, 0x0000, TRUE, FALSE),
('StudyDescription','Study Description', 0x0008, 0x1030, 0x0000, 0x0000, TRUE, FALSE),
('OperatorName','Operators Name', 0x0008, 0x1070, 0x0000, 0x0000, TRUE, FALSE),
-- series module
('Modality','Modality', 0x0008, 0x0060, 0x0000, 0x0000, TRUE, FALSE),
('SeriesInstanceUID','Series Instance UID', 0x0020, 0x000E, 0x0000, 0x0000, TRUE, FALSE),
('SeriesNumber','Series Number', 0x0020, 0x0011, 0x0000, 0x0000, TRUE, FALSE),
('ImageLaterality','Image Laterality', 0x0020, 0x0062, 0x0000, 0x0000, TRUE, FALSE), -- maybe renamed to laterality
('SeriesDescription','Series Description', 0x0008, 0x103E, 0x0000, 0x0000, TRUE, FALSE),
('BodyPartExamined','Body Part Examined', 0x0008, 0x0015, 0x0000, 0x0000, TRUE, FALSE),
('PatientPosition','Patient Position', 0x0018, 0x5100, 0x0000, 0x0000, TRUE, FALSE),
('ViewPosition','View Position', 0x0018, 0x5101, 0x0000, 0x0000, TRUE, FALSE), -- FOR PRESENTATION, FOR PROCESSING
-- Modality Worklist Information Model.
('ScheduledProcedureStepSequence','Scheduled Procedure Step Sequence', 0x0040, 0x0100, 0x0000, 0x0000, TRUE, FALSE),
    ('ScheduledStationAETitle','Scheduled Station AE Title', 0x0040, 0x0001, 0x0040, 0x0100, TRUE, FALSE), -- renamed
    ('ScheduledProcedureStepStartDate', 'Scheduled Procedure Step Start Date', 0x0040, 0x0002, 0x0040, 0x0100, TRUE, FALSE),
    ('ScheduledProcedureStepStartTime','Scheduled Procedure Step Start Time', 0x0040, 0x0003, 0x0040, 0x0100, TRUE, FALSE),
    ('ScheduledProcedureStepModality','Modality', 0x0008, 0x0060, 0x0040, 0x0100, TRUE, FALSE),
    ('ScheduledPerformingPhysicianName','Scheduled Performing Physician Name', 0x0040, 0x0006, 0x0040, 0x0100, TRUE, FALSE),
    ('ScheduledProcedureStepDescription','Scheduled Procedure Step Description', 0x0040, 0x0007, 0x0040, 0x0100, TRUE, FALSE),
    ('ScheduledStationName','Scheduled Station Name', 0x0040, 0x0010, 0x0040, 0x0100, TRUE, FALSE),
    ('ScheduledProcedureStepLocation','Scheduled Procedure Step Location', 0x0040, 0x0011, 0x0040, 0x0100, TRUE, FALSE),
    ('ScheduledProtocolCodeSequence','Scheduled Protocol Code Sequence',0x0040, 0x0008, 0x0040, 0x0100, TRUE, FALSE),
        ('CodeValue','Code Value', 0x0008, 0x0100, 0x0040, 0x0008, TRUE,FALSE),
    ('ScheduledProcedureStepID','Scheduled Procedure Step ID', 0x0040, 0x0009, 0x0040, 0x0100, TRUE, FALSE),
    ('ScheduledProcedureStepStatus','Scheduled Procedure Step Status', 0x0040, 0x0008, 0x0040, 0x0100, TRUE,FALSE),
    ('ReferencedDefinedProtocolSequence','Referenced Defined Protocol Sequence', 0x0018, 0x990C, 0x0040, 0x0100, TRUE, FALSE),
        ('ReferencedSOPClassUID','Referenced SOP Class UID', 0x0008, 0x1150, 0x0018, 0x990C, TRUE, FALSE),
        ('ReferencedSOPInstanceUID','Referenced SOP Instance UID', 0x0008, 0x1155, 0x0018, 0x990C, TRUE, FALSE),
    ('ReferencedPerformedProtocolSequence','Referenced Performed Protocol Sequence', 0x0018, 0x990D, 0x0040, 0x0100, TRUE, FALSE),
        ('Referenced SOP Class UID','Referenced SOP Class UID', 0x0008, 0x1150, 0x0018, 0x990D, TRUE, FALSE),
        ('Referenced SOP Instance UID','ReferencedSOPInstanceUID', 0x0008, 0x1155, 0x0018, 0x990D, TRUE, FALSE),
('RequestedProcedureID','Requested Procedure ID', 0x0040, 0x1001, 0x0000, 0x0000, TRUE, FALSE),
('RequestedProcedureDescription','Requested Procedure Description', 0x0032, 0x1060, 0x0000, 0x0000, TRUE, FALSE),
('RequestedProcedureCodeSequence','Requested Procedure Code Sequence', 0x0032, 0x1064, 0x0000, 0x0000, TRUE, FALSE),
    ('RequestedProcedureCodeValue','Code Value', 0x0008, 0x0100, 0x0032, 0x1064, TRUE, FALSE), 
    ('RequestedProcedureCodingSchemeDesignator','Coding Scheme Designator', 0x0008, 0x0102, 0x0032, 0x1064, TRUE, FALSE),
    ('RequestedProcedureCodingSchemeVersion','Coding Scheme Version', 0x0008, 0x0103, 0x0032, 0x1064, TRUE, FALSE),
    ('RequestedProcedureCodeMeaning','Code Meaning', 0x0008, 0x0104, 0x0032, 0x1064, TRUE, FALSE),
    ('RequestedProcedureLongCodeValue','Long Code Value', 0x0008, 0x0119, 0x0032, 0x1064, TRUE, FALSE),
    ('RequestedProcedureURNCodeValue','URN Code Value', 0x0008, 0x0120, 0x0032, 0x1064, TRUE, FALSE),
('RequestedLateralityCodeSequence','Requested Laterality Code Sequence', 0x0032, 0x1065, 0x0000, 0x0000, TRUE, FALSE),
    ('RequestedLateralityCodeValue','Code Value', 0x0008, 0x0100, 0x0032, 0x1065, TRUE, FALSE), 
    ('RequestedLateralityCodingSchemeDesignator','Coding Scheme Designator', 0x0008, 0x0102, 0x0032, 0x1065, TRUE, FALSE),
    ('RequestedLateralityCodingSchemeVersion','Coding Scheme Version', 0x0008, 0x0103, 0x0032, 0x1065, TRUE, FALSE),
    ('RequestedLateralityCodeMeaning','Code Meaning', 0x0008, 0x0104, 0x0032, 0x1065, TRUE, FALSE),
    ('RequestedLateralityLongCodeValue','Long Code Value', 0x0008, 0x0119, 0x0032, 0x1065, TRUE, FALSE),
    ('RequestedLateralityURNCodeValue','URN Code Value', 0x0008, 0x0120, 0x0032, 0x1065, TRUE, FALSE), 
('StudyInstanceUID', 'Study Instance UID', 0x0020, 0x000D, 0x0000, 0x0000, TRUE, FALSE),
('StudyDate','Study Date', 0x0008, 0x0020, 0x0000, 0x0000, TRUE, FALSE),
('StudyTime','Study Time', 0x0008, 0x0030, 0x0000, 0x0000, TRUE, FALSE),
('RequestedProcedurePriority','Requested Procedure Priority', 0x0040, 0x1003, 0x0000, 0x0000, TRUE, FALSE),
-- already inserted: ('AccessionNumber','Accession Number', 0x0008, 0x0050, 0x0000, 0x0000, TRUE, FALSE),
('IssuerOfAccessionNumberSequence','Issuer of Accession Number Sequence', 0x0008, 0x0051, 0x0000, 0x0000, TRUE, FALSE),
-- already inserted: ('RequestingPhysician','Requesting Physician', 0x0032, 0x1032, 0x0000, 0x0000, TRUE, FALSE),
-- already inserted: ('AdmissionID','Admission ID', 0x0038, 0x0010, 0x0000, 0x0000, TRUE, FALSE),
-- already inserted: ('PatientName','Patient Name', 0x0010, 0x0010, 0x0000, 0x0000, TRUE, FALSE),
-- already inserted: ('PatientID','Patient ID', 0x0010, 0x0020, 0x0000, 0x0000, TRUE, FALSE),
-- already inserred: ('IssuerOfPatientID','Issuer of Patient ID', 0x0010, 0x0021, 0x0000, 0x0000, TRUE, FALSE),
-- already inserred: ('PatientSex', 'Patient Sex', 0x0010, 0x0040, 0x0000, 0x0000, TRUE, FALSE),
-- already inserred: ('PatientBirthDate','Patient Birth Date', 0x0010, 0x0030, 0x0000, 0x0000, TRUE, FALSE),
('SexParametersforClinicalUseCategorySequence','Sex Parameters for Clinical Use Category Sequence', 0x0010, 0x0043, 0x0000, 0x0000, TRUE, FALSE),
    ('SexParametersforClinicalUseCategoryCodeSequence','Sex Parameters for Clinical Use Category Code Sequence', 0x0010, 0x0046, 0x0010, 0x0043, TRUE, FALSE),
        ('SexParametersforClinicalUseCategoryCodeValue','Code Value', 0x0008, 0x0100, 0x0010, 0x0046, TRUE, FALSE), 
        ('SexParametersforClinicalUseCategoryCodingSchemeDesignator','Coding Scheme Designator', 0x0008, 0x0102, 0x0010, 0x0046, TRUE, FALSE),
        ('SexParametersforClinicalUseCategoryCodingSchemeVersion','Coding Scheme Version', 0x0008, 0x0103, 0x0010, 0x0046, TRUE, FALSE),
        ('SexParametersforClinicalUseCategoryCodeMeaning','Code Meaning', 0x0008, 0x0104, 0x0010, 0x0046, TRUE, FALSE),
        ('SexParametersforClinicalUseCategoryLongCodeValue','Long Code Value', 0x0008, 0x0119, 0x0010, 0x0046, TRUE, FALSE),
        ('SexParametersforClinicalUseCategoryURNCodeValue','URN Code Value', 0x0008, 0x0120, 0x0010, 0x0046, TRUE, FALSE),
-- general equipment
('Manufacturer','Manufacturer', 0x0008, 0x0070, 0x0000, 0x0000, TRUE, FALSE),
('ManufacturerModelName','Manufacturers Model Name', 0x0008, 0x1090, 0x0000, 0x0000, TRUE, FALSE),
('InstitutionName','Institution Name', 0x0008, 0x0080, 0x0000, 0x0000, TRUE, FALSE),
('DeviceSerialNumber','Device Serial Number', 0x0018, 0x1000, 0x0000, 0x0000, TRUE, FALSE),
('SoftwareVersions','Software Versions', 0x0018, 0x1020, 0x0000, 0x0000, TRUE, FALSE),
('StationName','Station Name', 0x0008, 0x1010, 0x0000, 0x0000, TRUE, FALSE),
('InstitutionalDepartmentName','Institutional Department Name', 0x0008, 0x1040, 0x0000, 0x0000, TRUE, FALSE),
-- image
('ImageType','Image Type', 0x0008, 0x0008, 0x0000, 0x0000, TRUE, FALSE), -- ORIGINAL\PRIMARY
('KVP','KVP', 0x0018, 0x0060, 0x0000, 0x0000, TRUE, FALSE),
('XRayTubeCurrent','X-Ray Tube Current', 0x0018, 0x1151, 0x0000, 0x0000, TRUE, FALSE),
('ExposureTime','Exposure Time', 0x0018, 0x1150, 0x0000, 0x0000, TRUE, FALSE),
('Exposure','Exposure', 0x0018, 0x1152, 0x0000, 0x0000, TRUE, FALSE),
('ExposureControlMode','Exposure Control Mode', 0x0018, 0x7060, 0x0000, 0x0000, TRUE, FALSE), -- MANUAL, AUTOMATIC
('ExposureStatus','Exposure Status', 0x0018, 0x7064,  0x0000, 0x0000, TRUE, FALSE),
('FocalSpot','Focal Spot', 0x0018, 0x1190,  0x0000, 0x0000, TRUE, FALSE), -- focal spot size in mm
('DistanceSourcetoPatient','Distance Source to Patient', 0x0018, 0x1111, 0x0000, 0x0000, TRUE, FALSE),
('DistanceSourceToDetector','Distance Source to Detector', 0x0018, 0x1110, 0x0000, 0x0000, TRUE, FALSE),
('BodyPartThickness','Body Part Thickness', 0x0018, 0x11A0, 0x0000, 0x0000, TRUE, FALSE), -- The average thickness in mm of the body part examined when compressed, if compression has been applied during exposure.
('FilterType','Filter Type', 0x0018, 0x1160, 0x0000, 0x0000, TRUE, FALSE),
('FilterMaterial','Filter Material', 0x0018, 0x7050, 0x0000, 0x0000, TRUE, FALSE), -- The X-Ray absorbing material used in the filter. May be multi-valued: MOLYBDENUM, ALUMINUM, COPPER, RHODIUM, NIOBIUM, EUROPIUM, LEAD, SILVER
-- ('ViewCodeSequence','View Code Sequence', 0x0054, 0x0220, 0x0000, 0x0000, TRUE, FALSE), TODO: too complex try to add this tag in future versions
('SamplesperPixel','Samples per Pixel', 0x0028, 0x0002, 0x0000, 0x0000, TRUE, FALSE),
('PhotometricInterpretation','Photometric Interpretation', 0x0028, 0x0004, 0x0000, 0x0000, TRUE, FALSE),
('Rows','Rows', 0x0028, 0x0010, 0x0000, 0x0000, TRUE, FALSE),
('Columns','Columns', 0x0028, 0x0011, 0x0000, 0x0000, TRUE, FALSE),
('BitsAllocated','Bits Allocated', 0x0028, 0x0100, 0x0000, 0x0000, TRUE, FALSE),
('BitsStored','Bits Stored', 0x0028, 0x0101, 0x0000, 0x0000, TRUE, FALSE),
('HighBit','High Bit', 0x0028, 0x0102, 0x0000, 0x0000, TRUE, FALSE),
('PixelRepresentation','Pixel Representation', 0x0028, 0x0103, 0x0000, 0x0000, TRUE, FALSE), -- 0: (unsigned integer), 1:  (2's complement)
('PixelAspectRatio','Pixel Aspect Ratio', 0x0028, 0x0034, 0x0000, 0x0000, TRUE, FALSE);

 
INSERT INTO worklist_field_configurations(field_name,is_enabled)
VALUES
('PatientID',TRUE),
('AccessionNumber',TRUE),
('OtherPatientID',FALSE),
('AdmissionID',FALSE),
('ScheduledProcedureStepID',FALSE),
('RequestedProcedureID',FALSE),
('RequestedProcedureCodeValue',FALSE),
('StudyInstanceUID',FALSE);


-- Insert a sample DICOM tag group for 'DxWorklist'
INSERT INTO mwl_profiles (name) 
VALUES ('DxWorklist')
ON DUPLICATE KEY UPDATE name = mwl_profiles.name;  -- Ensure no duplicates of the 'DxWorklist'

INSERT INTO mwl_presentation_contexts (profile_id, transfer_syntax_uid) VALUES
(
  (SELECT id FROM mwl_profiles WHERE name = 'DxWorklist'), 
  '1.2.840.10008.1.2'
); -- Little Endian Implicit
-- (
--  (SELECT id FROM mwl_profiles WHERE name = 'DxWorklist'), 
--  '1.2.840.10008.1.2.1'
-- ); -- Little Endian Explicit


-- SET SQL_SAFE_UPDATES = 0;
-- DELETE FROM profile_tag_association;
-- SET SQL_SAFE_UPDATES = 1;


-- Insert all active tags into the profile_tag_association for 'DxWorklist'
INSERT INTO profile_tag_association (profile_id, tag_id, is_identifier, is_mandatory)
VALUES
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestAttributesSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PatientName'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PatientID'), TRUE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'IssuerOfPatientID'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'TypeOfPatientID'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'IssuerOfPatientIDQualifiersSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'OtherPatientID'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PatientSex'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PatientBirthDate'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PatientComments'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PatientAllergies'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestingPhysician'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PatientAge'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'AccessionNumber'), TRUE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'AdmissionID'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'StudyDescription'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'OperatorName'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'Modality'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SeriesInstanceUID'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SeriesNumber'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ImageLaterality'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SeriesDescription'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'BodyPartExamined'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PatientPosition'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ViewPosition'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledProcedureStepSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledStationAETitle'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledProcedureStepStartDate'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledProcedureStepStartTime'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledProcedureStepModality'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledPerformingPhysicianName'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledProcedureStepDescription'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledStationName'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledProcedureStepLocation'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledProtocolCodeSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'CodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledProcedureStepID'), TRUE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ScheduledProcedureStepStatus'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ReferencedDefinedProtocolSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ReferencedSOPClassUID'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ReferencedSOPInstanceUID'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ReferencedPerformedProtocolSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedureID'), TRUE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedureDescription'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedureCodeSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedureCodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedureCodingSchemeDesignator'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedureCodingSchemeVersion'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedureCodeMeaning'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedureLongCodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedureURNCodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedLateralityCodeSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedLateralityCodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedLateralityCodingSchemeDesignator'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedLateralityCodingSchemeVersion'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedLateralityCodeMeaning'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedLateralityLongCodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedLateralityURNCodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'StudyInstanceUID'), TRUE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'StudyDate'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'StudyTime'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'RequestedProcedurePriority'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'IssuerOfAccessionNumberSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SexParametersforClinicalUseCategorySequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SexParametersforClinicalUseCategoryCodeSequence'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SexParametersforClinicalUseCategoryCodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SexParametersforClinicalUseCategoryCodingSchemeDesignator'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SexParametersforClinicalUseCategoryCodingSchemeVersion'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SexParametersforClinicalUseCategoryCodeMeaning'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SexParametersforClinicalUseCategoryLongCodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SexParametersforClinicalUseCategoryURNCodeValue'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'Manufacturer'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ManufacturerModelName'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'InstitutionName'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'DeviceSerialNumber'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SoftwareVersions'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'StationName'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'InstitutionalDepartmentName'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ImageType'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'KVP'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'XRayTubeCurrent'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ExposureTime'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'Exposure'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ExposureControlMode'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'ExposureStatus'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'FocalSpot'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'DistanceSourcetoPatient'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'DistanceSourceToDetector'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'BodyPartThickness'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'FilterType'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'FilterMaterial'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'SamplesperPixel'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PhotometricInterpretation'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'Rows'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'Columns'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'BitsAllocated'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'BitsStored'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'HighBit'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PixelRepresentation'), FALSE, TRUE),
(1, (SELECT id FROM dicom_tags WHERE name = 'PixelAspectRatio'), FALSE, TRUE);



-- Table: institutions(name, institution_type, institution_id, department_name, address, contact_information, is_active)
INSERT INTO institutions
(name, institution_type, institution_id, department_name, address, contact_information, is_active)
VALUES
-- İstanbul
('Green Valley Hospital', 'Hospital', 'TR-IST-GVH-001', 'Cardiology',
 'Halkalı Cd. No:123, Küçükçekmece, 34303 İstanbul, Türkiye',
 'info@gvhospital.com | +90 212 555 23 45', 1),

-- Ankara
('Techno University', 'University', 'TR-ANK-TU-001', 'Computer Science',
 'Üniversiteler Mah., Dumlupınar Blv. No:1, 06800 Çankaya, Ankara, Türkiye',
 'contact@techno.edu | +90 312 987 65 43', 1),

-- İzmir
('Sunrise Clinic', 'Clinic', 'TR-IZM-SRC-001', 'Pediatrics',
 'Mustafa Kemal Sahil Blv. No:789, Karşıyaka, 35550 İzmir, Türkiye',
 'sunrise@clinic.org | +90 232 321 11 11', 1),

-- Ankara
('Global Research Institute', 'Research Institute', 'TR-ANK-GRI-001', 'Biotechnology',
 'Teknoloji Mah., İnovasyon Blv. No:101, 06510 Çankaya, Ankara, Türkiye',
 'office@gri.org | +90 312 777 88 88', 1),

-- Antalya
('BlueSky School', 'School', 'TR-ANT-BSS-001', 'Mathematics',
 'Atatürk Cd. No:202, Muratpaşa, 07010 Antalya, Türkiye',
 'info@blueskyschool.edu | +90 242 444 22 22', 0);

INSERT INTO general_equipments 
    (device_serial_number, device_uid, manufacturer, model_name, station_name, institution_id, department_name, date_of_last_calibration, time_of_last_calibration, date_of_manufacture, software_versions, gantry_id, is_active)
VALUES
    ('SN-AXD-20250826001', 'UID-9f3c12d7', 'Etrek AI Health', 'Smart Ray', 'DX-Station-01', 1, 'Radiology', '2025-06-10', '14:35:00', '2022-03-15', 'syngo DX VB20, Patch 2.1', 'Positioner-DX-01', TRUE);


INSERT INTO generators (
  manufacturer, model_number,
  part_number, serial_number, type_number, technical_specifications,
  manufacture_date, installation_date, calibration_date,
  is_active, is_output1_active, is_output2_active
)
VALUES
  ('Gulmay',  NULL,
   NULL, '0073-0313', NULL, 'voltage 220V 3.0A',
   '2010-01-01', '2012-01-01', '2016-08-14',
   TRUE, FALSE, FALSE),
  ('Sedecal', 'SHF-535',
   NULL, 'G-36421', NULL, '50KW 63KVA',
   '2010-01-01', '2010-08-16', '2016-08-14',
   TRUE, FALSE, FALSE);

INSERT INTO xray_tubes(
     manufacturer, model_number, part_number, serial_number, type_number, 
     focal_spot,
     anode_heat_capacity, cooling_rate , max_voltage,  max_current,
     power_range,
	 tube_filter,
     is_active,
     manufacture_date,
     installation_date
)
values
('Varian',NULL,'1547','24 876-z1','HPX-160-11',
 '04mm/1mm',
 NULL, NULL, 160, NULL,
 '800W 1800W',
 '0.8 Be',
 true,
 '2012-08-01',
 NULL
 ),
 ('Toshiba','E7869X',NULL,'09K148',NULL,
 '0.6mm/1.2mm',
 NULL, NULL, 150, NULL,
 NULL,
 '1.1 AL /75',
 true,
 NULL,
 NULL
 );

INSERT INTO detectors (
    manufacturer, model_name, resolution, size, pixel_width, pixel_height, sensitivity, imager_pixel_spacing,
    identifier, space_lut_reference, spatial_resolution, scan_options, width, height, width_shift,
    height_shift, saturation_value, is_active
)
VALUES
    (
    'Teledyne DALSA','DM-20-08K10-00-R', '8160 × 256','221 × 6.9 mm', 0.027, 0.027, 80.00,'0.027,0.027','DM-20-08K10-00-R-XXXXXX', 'Standard LUT', 7.0,'TDI, Area', 8160, 256, 0, 0, 65535, TRUE),
    ('Toshiba','FDX3543RP','2448 × 2984','35 × 43 cm', 0.143, 0.143, 70.00,'0.143,0.143','FDX3543RP-XXXXXX','Standard LUT',3.7,'Fixed, Pulsed',2448, 2984, 0, 0, 65535, TRUE);



INSERT INTO image_comments (is_reject, heading, comment) VALUES
-- ACCEPTED
(FALSE, 'Chest PA - Diagnostic quality',
'Well-centered PA chest: full inspiration (10 posterior ribs visible), no rotation (spinous processes midline), scapulae cleared from lung fields, sharp costophrenic angles, adequate penetration (vertebral bodies faintly visible behind heart), no motion blur diagnostic image.'),
(FALSE, 'Knee AP - Diagnostic quality',
'AP knee correctly centered to joint space patella near midline, femorotibial joint margins symmetrical, tibial spines well seen, exposure adequate to visualize trabecular detail collimation includes distal femur and proximal tibia/fibula no motion.'),

-- REJECTED
(TRUE, 'Pelvis AP - Reject: rotation/collimation',
'Pelvis AP rejected: patient rotated (asymmetric obturator foramina, iliac wings unequal), left hip joint partially clipped repeat with equal ASIS-to-table distances, 15-degree internal leg rotation, center midway between ASIS and pubic symphysis, widen collimation to include both hip joints.'),
(TRUE, 'Cervical Spine Lateral - Reject: exposure/coverage',
'C-spine lateral rejected: C7/T1 not demonstrated inadequate shoulder depression and underexposure with mild motion blur. Repeat using swimmer''s lateral or gentle traction, increase mAs/shorten exposure time, and ensure immobilization to include C7/T1 junction.');

-- The following table content data for development
INSERT INTO pacs_nodes
  (entity_type, host_name, host_ip, host_port, called_aet, calling_aet, is_default)
VALUES
  -- ARCHIVE nodes
  ('Archive', 'Main Archive',   '192.168.1.10',   104,   'ARCHIVE_AE', 'ETREK_WS',  TRUE),
  ('Archive', 'Backup Archive', '192.168.1.11',   11112, 'ARCHIVE_BK', 'ETREK_WS',  FALSE),
  ('Archive', 'Cloud Archive',  '203.0.113.10',   11112, 'CLOUD_ARC',  'ETREK_WS',  FALSE),
  ('Archive', 'Staging Archive','10.10.0.15',     11112, 'STAGE_ARC',  'ETREK_WS',  FALSE),

  -- MPPS nodes
  ('MPPS',    'Local MPPS',     '192.168.1.20',   104,   'MPPS_SCP',   'ETREK_WS',  TRUE),
  ('MPPS',    'Vendor MPPS',    '198.51.100.50',  11112, 'VEND_MPPS',  'ETREK_WS',  FALSE),
  ('MPPS',    'Test MPPS',      '10.0.0.20',      11112, 'MPPS_TEST',  'ETREK_WS2', FALSE);



  -- anatomic regions: Primary Anatomic Structure Sequence (0008,2228) 
INSERT INTO anatomic_regions (name, code_value, coding_scheme, code_meaning, description, display_order) VALUES
  ('HEAD',              'T-D1100', 'SRT', 'Head',          'Head region',                                      10),
  ('NECK',              'T-D1600', 'SRT', 'Neck',          'Neck region',                                      20),
  ('THORAX',            'T-D3000', 'SRT', 'Chest',         'Thorax / chest',                                   30),
  ('ABDOMEN',           'T-D4000', 'SRT', 'Abdomen',       'Abdominal region',                                 40),
  ('PELVIS',            'T-D6000', 'SRT', 'Pelvis',        'Pelvic region',                                    50),
  ('SPINE',             'T-D0146', 'SRT', 'Spine',         'Entire spine / vertebral column',                  60),
  ('UPPER EXTREMITY',   'T-D8000', 'SRT', 'Upper limb',    'Upper extremity bucket (arm/forearm/hand)',        70),
  ('LOWER EXTREMITY',   'T-D9400', 'SRT', 'Leg',           'Lower extremity bucket (thigh/leg/ankle/foot)',    80);




  -- -------- HEAD --------

INSERT INTO body_parts (name, code_value, coding_scheme, description, anatomic_region_id, is_active) VALUES
('HEAD',         'E1-00001', 'ETRK', 'Head (generic)',            (SELECT id FROM anatomic_regions WHERE name='HEAD'), TRUE),
('SKULL',        'E1-00002', 'ETRK', 'Skull',                      (SELECT id FROM anatomic_regions WHERE name='HEAD'), TRUE),
('SINUSES',      'E1-00003', 'ETRK', 'Paranasal sinuses',          (SELECT id FROM anatomic_regions WHERE name='HEAD'), TRUE),
('ORBITS',       'E1-00004', 'ETRK', 'Orbits',                     (SELECT id FROM anatomic_regions WHERE name='HEAD'), TRUE),
('NASALBONES',   'E1-00005', 'ETRK', 'Nasal bones',                (SELECT id FROM anatomic_regions WHERE name='HEAD'), TRUE),
('FACIALBONES',  'E1-00006', 'ETRK', 'Facial bones',               (SELECT id FROM anatomic_regions WHERE name='HEAD'), TRUE),
('MANDIBLE',     'E1-00007', 'ETRK', 'Mandible',                   (SELECT id FROM anatomic_regions WHERE name='HEAD'), TRUE),
('TMJ',          'E1-00008', 'ETRK', 'Temporomandibular joints',   (SELECT id FROM anatomic_regions WHERE name='HEAD'), TRUE);

-- -------- NECK --------
INSERT INTO body_parts (name, code_value, coding_scheme, description, anatomic_region_id, is_active) VALUES
('NECK',             'E1-00009', 'ETRK', 'Neck (generic)',        (SELECT id FROM anatomic_regions WHERE name='NECK'), TRUE),
('SOFTTISSUENECK',   'E1-00010', 'ETRK', 'Soft tissue neck',      (SELECT id FROM anatomic_regions WHERE name='NECK'), TRUE);

-- -------- SPINE --------
INSERT INTO body_parts (name, code_value, coding_scheme, description, anatomic_region_id, is_active) VALUES
('CSPINE',       'E1-00011', 'ETRK', 'Cervical spine',            (SELECT id FROM anatomic_regions WHERE name='SPINE'), TRUE),
('TSPINE',       'E1-00012', 'ETRK', 'Thoracic spine',            (SELECT id FROM anatomic_regions WHERE name='SPINE'), TRUE),
('LSPINE',       'E1-00013', 'ETRK', 'Lumbar spine',              (SELECT id FROM anatomic_regions WHERE name='SPINE'), TRUE),
('SSPINE',       'E1-00014', 'ETRK', 'Sacrum (sacral spine)',     (SELECT id FROM anatomic_regions WHERE name='SPINE'), TRUE),
('COCCYX',       'E1-00015', 'ETRK', 'Coccyx',                    (SELECT id FROM anatomic_regions WHERE name='SPINE'), TRUE),
('SCOLIOSIS',    'E1-00016', 'ETRK', 'Whole-spine scoliosis',     (SELECT id FROM anatomic_regions WHERE name='SPINE'), TRUE);

-- -------- THORAX (CHEST) --------
INSERT INTO body_parts (name, code_value, coding_scheme, description, anatomic_region_id, is_active) VALUES
('CHEST',        'E1-00017', 'ETRK', 'Thorax / chest',            (SELECT id FROM anatomic_regions WHERE name='THORAX'), TRUE),
('RIBS',         'E1-00018', 'ETRK', 'Ribs',                      (SELECT id FROM anatomic_regions WHERE name='THORAX'), TRUE),
('STERNUM',      'E1-00019', 'ETRK', 'Sternum',                   (SELECT id FROM anatomic_regions WHERE name='THORAX'), TRUE),
('CLAVICLE',     'E1-00020', 'ETRK', 'Clavicle',                  (SELECT id FROM anatomic_regions WHERE name='THORAX'), TRUE),
('SCAPULA',      'E1-00021', 'ETRK', 'Scapula',                   (SELECT id FROM anatomic_regions WHERE name='THORAX'), TRUE),
('ACJOINTS',     'E1-00022', 'ETRK', 'Acromioclavicular joints',  (SELECT id FROM anatomic_regions WHERE name='THORAX'), TRUE);

-- -------- ABDOMEN --------
INSERT INTO body_parts (name, code_value, coding_scheme, description, anatomic_region_id, is_active) VALUES
('ABDOMEN',      'E1-00023', 'ETRK', 'Abdomen (e.g. KUB)',        (SELECT id FROM anatomic_regions WHERE name='ABDOMEN'), TRUE);

-- -------- PELVIS --------
INSERT INTO body_parts (name, code_value, coding_scheme, description, anatomic_region_id, is_active) VALUES
('PELVIS',            'E1-00024', 'ETRK', 'Pelvis',               (SELECT id FROM anatomic_regions WHERE name='PELVIS'), TRUE),
('HIP',               'E1-00025', 'ETRK', 'Hip',                  (SELECT id FROM anatomic_regions WHERE name='PELVIS'), TRUE),
('SACROILIAC JOINTS',  'E1-00026', 'ETRK', 'Sacroiliac joints',    (SELECT id FROM anatomic_regions WHERE name='PELVIS'), TRUE);

-- -------- UPPER EXTREMITY --------
INSERT INTO body_parts (name, code_value, coding_scheme, description, anatomic_region_id, is_active) VALUES
('SHOULDER',     'E1-00027', 'ETRK', 'Shoulder',                  (SELECT id FROM anatomic_regions WHERE name='UPPER EXTREMITY'), TRUE),
('HUMERUS',      'E1-00028', 'ETRK', 'Humerus',                   (SELECT id FROM anatomic_regions WHERE name='UPPER EXTREMITY'), TRUE),
('ELBOW',        'E1-00029', 'ETRK', 'Elbow',                     (SELECT id FROM anatomic_regions WHERE name='UPPER EXTREMITY'), TRUE),
('FOREARM',      'E1-00030', 'ETRK', 'Forearm (radius/ulna)',     (SELECT id FROM anatomic_regions WHERE name='UPPER EXTREMITY'), TRUE),
('WRIST',        'E1-00031', 'ETRK', 'Wrist',                     (SELECT id FROM anatomic_regions WHERE name='UPPER EXTREMITY'), TRUE),
('HAND',         'E1-00032', 'ETRK', 'Hand',                      (SELECT id FROM anatomic_regions WHERE name='UPPER EXTREMITY'), TRUE),
('FINGERS',      'E1-00033', 'ETRK', 'Fingers / digits',          (SELECT id FROM anatomic_regions WHERE name='UPPER EXTREMITY'), TRUE),
('THUMB',        'E1-00034', 'ETRK', 'Thumb',                     (SELECT id FROM anatomic_regions WHERE name='UPPER EXTREMITY'), TRUE);

-- -------- LOWER EXTREMITY --------
INSERT INTO body_parts (name, code_value, coding_scheme, description, anatomic_region_id, is_active) VALUES
('FEMUR',        'E1-00035', 'ETRK', 'Femur',                     (SELECT id FROM anatomic_regions WHERE name='LOWER EXTREMITY'), TRUE),
('KNEE',         'E1-00036', 'ETRK', 'Knee',                      (SELECT id FROM anatomic_regions WHERE name='LOWER EXTREMITY'), TRUE),
('PATELLA',      'E1-00037', 'ETRK', 'Patella',                   (SELECT id FROM anatomic_regions WHERE name='LOWER EXTREMITY'), TRUE),
('TIBIAFIBULA',  'E1-00038', 'ETRK', 'Tibia/Fibula',              (SELECT id FROM anatomic_regions WHERE name='LOWER EXTREMITY'), TRUE),
('ANKLE',        'E1-00039', 'ETRK', 'Ankle',                     (SELECT id FROM anatomic_regions WHERE name='LOWER EXTREMITY'), TRUE),
('FOOT',         'E1-00040', 'ETRK', 'Foot',                      (SELECT id FROM anatomic_regions WHERE name='LOWER EXTREMITY'), TRUE),
('TOES',         'E1-00041', 'ETRK', 'Toes / digits',             (SELECT id FROM anatomic_regions WHERE name='LOWER EXTREMITY'), TRUE),
('CALCANEUS',    'E1-00042', 'ETRK', 'Calcaneus (heel)',          (SELECT id FROM anatomic_regions WHERE name='LOWER EXTREMITY'), TRUE);


-- ------------------- TECHNIQUE PARAMETERS -------------------
-- ============ HEAD ============
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  ((SELECT id FROM body_parts WHERE name='HEAD'), 'Thin',  'AP|PA', 70, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HEAD'), 'Medium','AP|PA', 75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HEAD'), 'Fat',   'AP|PA', 80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='HEAD'), 'Thin',  'LAT',   80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HEAD'), 'Medium','LAT',   85, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HEAD'), 'Fat',   'LAT',   90, 200, 320, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='HEAD'), 'Paediatric','AP|PA', 65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HEAD'), 'Paediatric','LAT',   70, 200, 80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ============ SKULL ============
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Thin',   'AP|PA', 70, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Medium', 'AP|PA', 75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Fat',    'AP|PA', 80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Thin',   'LAT',   80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Medium', 'LAT',   85, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Fat',    'LAT',   90, 200, 320, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Thin',   'AXIAL', 80, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Medium', 'AXIAL', 85, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Fat',    'AXIAL', 90, 200, 320, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Paediatric','AP|PA', 65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SKULL'), 'Paediatric','LAT',   70, 200, 80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ============ SINUSES ============
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  ((SELECT id FROM body_parts WHERE name='SINUSES'), 'Thin',   'AP|PA', 70, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SINUSES'), 'Medium', 'AP|PA', 75, 200, 50, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SINUSES'), 'Fat',    'AP|PA', 80, 200, 80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='SINUSES'), 'Thin',   'LAT',   70, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SINUSES'), 'Medium', 'LAT',   75, 200, 60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SINUSES'), 'Fat',    'LAT',   80, 200, 80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='SINUSES'), 'Paediatric','AP|PA', 60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SINUSES'), 'Paediatric','LAT',   65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ============ ORBITS ============
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  ((SELECT id FROM body_parts WHERE name='ORBITS'), 'Thin',   'AP|PA', 65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ORBITS'), 'Medium', 'AP|PA', 70, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ORBITS'), 'Fat',    'AP|PA', 75, 200, 60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='ORBITS'), 'Thin',   'LAT',   65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ORBITS'), 'Medium', 'LAT',   70, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ORBITS'), 'Fat',    'LAT',   75, 200, 50, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='ORBITS'), 'Paediatric','AP|PA', 60, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ORBITS'), 'Paediatric','LAT',   60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ============ NASAL BONES ============
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  ((SELECT id FROM body_parts WHERE name='NASALBONES'), 'Thin',   'AP|PA', 60, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NASALBONES'), 'Medium', 'AP|PA', 60, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NASALBONES'), 'Fat',    'AP|PA', 60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='NASALBONES'), 'Thin',   'LAT',   60, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NASALBONES'), 'Medium', 'LAT',   60, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NASALBONES'), 'Fat',    'LAT',   60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='NASALBONES'), 'Paediatric','AP|PA', 55, 200, 8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NASALBONES'), 'Paediatric','LAT',   55, 200, 8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ============ FACIAL BONES ============
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  ((SELECT id FROM body_parts WHERE name='FACIALBONES'), 'Thin',   'AP|PA', 70, 200,  60, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FACIALBONES'), 'Medium', 'AP|PA', 75, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FACIALBONES'), 'Fat',    'AP|PA', 80, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='FACIALBONES'), 'Thin',   'LAT',   70, 200,  50, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FACIALBONES'), 'Medium', 'LAT',   75, 200,  60, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FACIALBONES'), 'Fat',    'LAT',   80, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='FACIALBONES'), 'Paediatric','AP|PA', 65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FACIALBONES'), 'Paediatric','LAT',   65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ============ MANDIBLE ============
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Thin',   'AP|PA', 70, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Medium', 'AP|PA', 75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Fat',    'AP|PA', 80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- OBL (Axiolateral oblique)
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Thin',   'OBL',   70, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Medium', 'OBL',   75, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Fat',    'OBL',   80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- AXIAL (Towne / SMV variations)
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Thin',   'AXIAL', 75, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Medium', 'AXIAL', 80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Fat',    'AXIAL', 85, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Paediatric','AP|PA', 65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='MANDIBLE'), 'Paediatric','OBL',   65, 200, 50, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ============ TMJ ============
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA (açık/kapalı ağız)
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Thin',   'AP|PA', 70, 200, 50, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Medium', 'AP|PA', 75, 200, 60, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Fat',    'AP|PA', 80, 200, 80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- OBL (transkranial/oblik)
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Thin',   'OBL',   70, 200, 60, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Medium', 'OBL',   75, 200, 80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Fat',    'OBL',   80, 200,100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Thin',   'LAT', 70, 200,  60, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Medium', 'LAT', 75, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Fat',    'LAT', 80, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Paediatric','LAT', 60, 200,  25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Paediatric','AP|PA', 60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TMJ'), 'Paediatric','OBL',   65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== NECK (generic) ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='NECK'), 'Thin',   'AP|PA', 70, 200,  40, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NECK'), 'Medium', 'AP|PA', 75, 200,  50, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NECK'), 'Fat',    'AP|PA', 80, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='NECK'), 'Thin',   'LAT',   75, 200,  60, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NECK'), 'Medium', 'LAT',   80, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NECK'), 'Fat',    'LAT',   85, 200, 120, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='NECK'), 'Paediatric','AP|PA', 60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='NECK'), 'Paediatric','LAT',   65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== SOFTTISSUENECK ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA (gridsiz tercih)
  ((SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'), 'Thin',   'AP|PA', 60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'), 'Medium', 'AP|PA', 65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'), 'Fat',    'AP|PA', 70, 200, 60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT (gridsiz)
  ((SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'), 'Thin',   'LAT',   70, 200, 50, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'), 'Medium', 'LAT',   75, 200, 60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'), 'Fat',    'LAT',   80, 200, 80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'), 'Paediatric','AP|PA', 55, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'), 'Paediatric','LAT',   60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== CSPINE ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Thin',   'AP|PA', 70, 200,  40, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Medium', 'AP|PA', 75, 200,  50, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Fat',    'AP|PA', 80, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT (72" / ~180 cm önerilir)
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Thin',   'LAT',   80, 200,  80, NULL, NULL, NULL, 150, 180, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Medium', 'LAT',   85, 200, 120, NULL, NULL, NULL, 150, 180, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Fat',    'LAT',   90, 200, 200, NULL, NULL, NULL, 150, 180, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- OBL (RAO/LAO) ≈ AP +5 kVp, mAs +~25%
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Thin',   'OBL',   75, 200,  50, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Medium', 'OBL',   80, 200,  65, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Fat',    'OBL',   85, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Paediatric','AP|PA', 60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CSPINE'), 'Paediatric','LAT',   70, 200, 40, NULL, NULL, NULL, 150, 180, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== TSPINE ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='TSPINE'), 'Thin',   'AP|PA', 75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TSPINE'), 'Medium', 'AP|PA', 80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TSPINE'), 'Fat',    'AP|PA', 85, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT (uzun kalınlık → daha yüksek mAs, 150–180 cm uygun)
  ((SELECT id FROM body_parts WHERE name='TSPINE'), 'Thin',   'LAT',   85, 200, 200, NULL, NULL, NULL, 150, 180, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TSPINE'), 'Medium', 'LAT',   90, 200, 300, NULL, NULL, NULL, 150, 180, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TSPINE'), 'Fat',    'LAT',   95, 200, 400, NULL, NULL, NULL, 150, 180, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='TSPINE'), 'Paediatric','AP|PA', 65, 200,  50, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TSPINE'), 'Paediatric','LAT',   75, 200, 100, NULL, NULL, NULL, 150, 180, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);



-- ========== LSPINE ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Thin',   'AP|PA', 80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Medium', 'AP|PA', 85, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Fat',    'AP|PA', 90, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT (100–110 cm tipik)
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Thin',   'LAT',   90, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Medium', 'LAT',   95, 200, 400, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Fat',    'LAT',  100, 200, 600, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- OBL (RPO/LPO) ≈ AP +5 kVp, mAs +~25%
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Thin',   'OBL',   85, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Medium', 'OBL',   90, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Fat',    'OBL',   95, 200, 320, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- AXIAL (L5-S1 spot)
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Thin',   'AXIAL',  95, 200, 400, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Medium', 'AXIAL', 100, 200, 600, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Fat',    'AXIAL', 105, 200, 800, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Paediatric','AP|PA', 70, 200,  80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='LSPINE'), 'Paediatric','LAT',   80, 200, 160, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== SSPINE (Sacrum) ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AXIAL (AP axial)
  ((SELECT id FROM body_parts WHERE name='SSPINE'), 'Thin',   'AXIAL',  80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SSPINE'), 'Medium', 'AXIAL',  85, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SSPINE'), 'Fat',    'AXIAL',  90, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='SSPINE'), 'Thin',   'LAT',    85, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SSPINE'), 'Medium', 'LAT',    90, 200, 300, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SSPINE'), 'Fat',    'LAT',    95, 200, 400, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='SSPINE'), 'Paediatric','AXIAL', 70, 200,  80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SSPINE'), 'Paediatric','LAT',   75, 200, 120, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== COCCYX ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AXIAL (AP axial; çoğunlukla gridsiz)
  ((SELECT id FROM body_parts WHERE name='COCCYX'), 'Thin',   'AXIAL', 70, 200,  60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='COCCYX'), 'Medium', 'AXIAL', 75, 200,  80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='COCCYX'), 'Fat',    'AXIAL', 80, 200, 125, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT (gridsiz veya düşük grid)
  ((SELECT id FROM body_parts WHERE name='COCCYX'), 'Thin',   'LAT',   80, 200, 160, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='COCCYX'), 'Medium', 'LAT',   85, 200, 200, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='COCCYX'), 'Fat',    'LAT',   90, 200, 250, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='COCCYX'), 'Paediatric','AXIAL', 60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='COCCYX'), 'Paediatric','LAT',   65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== SCOLIOSIS (Whole-spine) ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA (PA tercih; stitch/LLI; genelde gridsiz; yüksek SID)
  ((SELECT id FROM body_parts WHERE name='SCOLIOSIS'), 'Thin',   'AP|PA', 85, 200,  30, NULL, NULL, NULL, 180, 200, NULL,      NULL,  NULL, 'AEC Mode', 'C',  0),
  ((SELECT id FROM body_parts WHERE name='SCOLIOSIS'), 'Medium', 'AP|PA', 90, 200,  40, NULL, NULL, NULL, 180, 200, NULL,      NULL,  NULL, 'AEC Mode', 'C',  0),
  ((SELECT id FROM body_parts WHERE name='SCOLIOSIS'), 'Fat',    'AP|PA', 95, 200,  60, NULL, NULL, NULL, 180, 200, NULL,      NULL,  NULL, 'AEC Mode', 'C',  0),

  -- LAT (daha yüksek mAs; grid kullanımı yaygın)
  ((SELECT id FROM body_parts WHERE name='SCOLIOSIS'), 'Thin',   'LAT',   95, 200, 125, NULL, NULL, NULL, 180, 200, 'Parallel', '10:1', NULL, 'AEC Mode', 'C',  0),
  ((SELECT id FROM body_parts WHERE name='SCOLIOSIS'), 'Medium', 'LAT',  100, 200, 160, NULL, NULL, NULL, 180, 200, 'Parallel', '10:1', NULL, 'AEC Mode', 'C',  0),
  ((SELECT id FROM body_parts WHERE name='SCOLIOSIS'), 'Fat',    'LAT',  105, 200, 250, NULL, NULL, NULL, 180, 200, 'Parallel', '10:1', NULL, 'AEC Mode', 'C',  0),

  -- Paediatric (düşük doz; PA; gridsiz)
  ((SELECT id FROM body_parts WHERE name='SCOLIOSIS'), 'Paediatric','AP|PA', 70, 200, 15, NULL, NULL, NULL, 180, 200, NULL, NULL, NULL, 'AEC Mode', 'C', -1),
  ((SELECT id FROM body_parts WHERE name='SCOLIOSIS'), 'Paediatric','LAT',   80, 200, 30, NULL, NULL, NULL, 180, 200, NULL, NULL, NULL, 'AEC Mode', 'C', -1);

-- ========== CHEST ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA (gridsiz; uzun SID)
  ((SELECT id FROM body_parts WHERE name='CHEST'), 'Thin',     'AP|PA',  85, 200,  30, NULL, NULL, NULL, 180, 183, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CHEST'), 'Medium',   'AP|PA', 100, 200,  20, NULL, NULL, NULL, 180, 183, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CHEST'), 'Fat',      'AP|PA', 110, 200,  60, NULL, NULL, NULL, 180, 183, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT (≈ AP/PA mAs ×2, +10 kVp)
  ((SELECT id FROM body_parts WHERE name='CHEST'), 'Thin',     'LAT',    95, 200,  60, NULL, NULL, NULL, 180, 183, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CHEST'), 'Medium',   'LAT',   110, 200,  40, NULL, NULL, NULL, 180, 183, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CHEST'), 'Fat',      'LAT',   120, 200, 120, NULL, NULL, NULL, 180, 183, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='CHEST'), 'Paediatric','AP|PA', 70, 200, 10, NULL, NULL, NULL, 180, 183, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CHEST'), 'Paediatric','LAT',   80, 200, 20, NULL, NULL, NULL, 180, 183, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== RIBS ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='RIBS'), 'Thin',   'AP|PA', 70, 200,  60, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='RIBS'), 'Medium', 'AP|PA', 75, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='RIBS'), 'Fat',    'AP|PA', 80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- OBL (RAO/LAO; mAs ~+25%)
  ((SELECT id FROM body_parts WHERE name='RIBS'), 'Thin',   'OBL',   75, 200,  75, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='RIBS'), 'Medium', 'OBL',   80, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='RIBS'), 'Fat',    'OBL',   85, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='RIBS'), 'Paediatric','AP|PA', 60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='RIBS'), 'Paediatric','OBL',   65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== STERNUM ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- OBL (RAO; mAs değerleri)
  ((SELECT id FROM body_parts WHERE name='STERNUM'), 'Thin',   'OBL',  75, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='STERNUM'), 'Medium', 'OBL',  80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='STERNUM'), 'Fat',    'OBL',  85, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1',  NULL, 'Mas Mode', NULL, NULL),

  -- LAT (yüksek mAs)
  ((SELECT id FROM body_parts WHERE name='STERNUM'), 'Thin',   'LAT',  85, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='STERNUM'), 'Medium', 'LAT',  90, 200, 320, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='STERNUM'), 'Fat',    'LAT',  95, 200, 500, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='STERNUM'), 'Paediatric','OBL', 65, 200,  60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='STERNUM'), 'Paediatric','LAT', 75, 200, 120, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== CLAVICLE ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA
  ((SELECT id FROM body_parts WHERE name='CLAVICLE'), 'Thin',   'AP|PA', 65, 200, 40, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CLAVICLE'), 'Medium', 'AP|PA', 70, 200, 50, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CLAVICLE'), 'Fat',    'AP|PA', 75, 200, 75, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- AXIAL (lordotik; +5 kVp)
  ((SELECT id FROM body_parts WHERE name='CLAVICLE'), 'Thin',   'AXIAL', 70, 200, 50, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CLAVICLE'), 'Medium', 'AXIAL', 75, 200, 60, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CLAVICLE'), 'Fat',    'AXIAL', 80, 200, 90, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='CLAVICLE'), 'Paediatric','AP|PA', 60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CLAVICLE'), 'Paediatric','AXIAL', 65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== SCAPULA ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA
  ((SELECT id FROM body_parts WHERE name='SCAPULA'), 'Thin',   'AP|PA', 70, 200,  60, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SCAPULA'), 'Medium', 'AP|PA', 75, 200,  80, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SCAPULA'), 'Fat',    'AP|PA', 80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT (Y view)
  ((SELECT id FROM body_parts WHERE name='SCAPULA'), 'Thin',   'LAT',   75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SCAPULA'), 'Medium', 'LAT',   80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SCAPULA'), 'Fat',    'LAT',   85, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='SCAPULA'), 'Paediatric','AP|PA', 60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SCAPULA'), 'Paediatric','LAT',   65, 200, 50, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== ACJOINTS (Acromioclavicular Joints) ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP (bilateral)
  ((SELECT id FROM body_parts WHERE name='ACJOINTS'), 'Thin',   'AP|PA', 65, 200, 30, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ACJOINTS'), 'Medium', 'AP|PA', 70, 200, 40, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ACJOINTS'), 'Fat',    'AP|PA', 75, 200, 60, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='ACJOINTS'), 'Paediatric','AP|PA', 60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== ABDOMEN (KUB) ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA (grid)
  ((SELECT id FROM body_parts WHERE name='ABDOMEN'), 'Thin',   'AP|PA', 75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ABDOMEN'), 'Medium', 'AP|PA', 80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ABDOMEN'), 'Fat',    'AP|PA', 85, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT (more mAs; grid)
  ((SELECT id FROM body_parts WHERE name='ABDOMEN'), 'Thin',   'LAT',   85, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ABDOMEN'), 'Medium', 'LAT',   90, 200, 320, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ABDOMEN'), 'Fat',    'LAT',   95, 200, 400, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='ABDOMEN'), 'Paediatric','AP|PA', 65, 200,  40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ABDOMEN'), 'Paediatric','LAT',   70, 200,  60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== PELVIS ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA (grid)
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Thin',   'AP|PA', 75, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Medium', 'AP|PA', 80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Fat',    'AP|PA', 85, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT (cross-table; higher mAs; grid)
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Thin',   'LAT',   90, 200, 320, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Medium', 'LAT',   95, 200, 400, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Fat',    'LAT',  100, 200, 500, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Thin',   'AXIAL', 80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Medium', 'AXIAL', 85, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Fat',    'AXIAL', 90, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Paediatric','AXIAL', 70, 200,  80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  
  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Paediatric','AP|PA', 65, 200,  40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PELVIS'), 'Paediatric','LAT',   75, 200,  80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== HIP ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA (grid)
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Thin',   'AP|PA', 70, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel','8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Medium', 'AP|PA', 75, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel','8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Fat',    'AP|PA', 80, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT (frog-lateral; grid)
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Thin',   'LAT',   75, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel','8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Medium', 'LAT',   80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel','8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Fat',    'LAT',   85, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),

  -- AXIAL (cross-table lateral; grid)
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Thin',   'AXIAL', 80, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Medium', 'AXIAL', 85, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Fat',    'AXIAL', 90, 200, 320, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Paediatric','AP|PA', 60, 200,  30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HIP'), 'Paediatric','LAT',   65, 200,  40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== SACROILIAC JOINTS (SIJ) ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA (AP axial Ferguson often; grid)
  ((SELECT id FROM body_parts WHERE name='SACROILIAC JOINTS'), 'Thin',   'AP|PA', 80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SACROILIAC JOINTS'), 'Medium', 'AP|PA', 85, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SACROILIAC JOINTS'), 'Fat',    'AP|PA', 90, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),

  -- OBL (RPO/LPO; grid)
  ((SELECT id FROM body_parts WHERE name='SACROILIAC JOINTS'), 'Thin',   'OBL',   85, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SACROILIAC JOINTS'), 'Medium', 'OBL',   90, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SACROILIAC JOINTS'), 'Fat',    'OBL',   95, 200, 320, NULL, NULL, NULL, 100, 110, 'Parallel','10:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz; AP/PA)
  ((SELECT id FROM body_parts WHERE name='SACROILIAC JOINTS'), 'Paediatric','AP|PA', 70, 200, 60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== SHOULDER ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Thin',   'AP|PA', 65, 200,  50, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Medium', 'AP|PA', 70, 200,  75, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Fat',    'AP|PA', 75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- OBL (Grashey)
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Thin',   'OBL',   70, 200,  60, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Medium', 'OBL',   75, 200,  90, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Fat',    'OBL',   80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT (Scapular Y)
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Thin',   'LAT',   75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Medium', 'LAT',   80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Fat',    'LAT',   85, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- AXIAL (Axillary)
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Thin',   'AXIAL', 70, 200,  75, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Medium', 'AXIAL', 75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Fat',    'AXIAL', 80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Paediatric','AP|PA', 60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='SHOULDER'), 'Paediatric','LAT',   65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== HUMERUS ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA
  ((SELECT id FROM body_parts WHERE name='HUMERUS'), 'Thin',   'AP|PA', 65, 200,  50, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HUMERUS'), 'Medium', 'AP|PA', 70, 200,  75, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HUMERUS'), 'Fat',    'AP|PA', 75, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='HUMERUS'), 'Thin',   'LAT',   70, 200,  60, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HUMERUS'), 'Medium', 'LAT',   75, 200,  90, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HUMERUS'), 'Fat',    'LAT',   80, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='HUMERUS'), 'Paediatric','AP|PA', 55, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HUMERUS'), 'Paediatric','LAT',   60, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);



-- ========== ELBOW ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Thin',   'AP|PA', 60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Medium', 'AP|PA', 60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Fat',    'AP|PA', 65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Thin',   'LAT',   60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Medium', 'LAT',   60, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Fat',    'LAT',   65, 200, 45, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- OBL
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Thin',   'OBL',   60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Medium', 'OBL',   60, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Fat',    'OBL',   65, 200, 45, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz; Medium≈Fat aynı)
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Paediatric','AP|PA', 55, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ELBOW'), 'Paediatric','LAT',   55, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== FOREARM ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA
  ((SELECT id FROM body_parts WHERE name='FOREARM'), 'Thin',   'AP|PA', 60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOREARM'), 'Medium', 'AP|PA', 60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOREARM'), 'Fat',    'AP|PA', 65, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='FOREARM'), 'Thin',   'LAT',   60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOREARM'), 'Medium', 'LAT',   60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOREARM'), 'Fat',    'LAT',   65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='FOREARM'), 'Paediatric','AP|PA', 55, 200, 12, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOREARM'), 'Paediatric','LAT',   55, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== WRIST ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Thin',   'AP|PA', 55, 200, 12, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Medium', 'AP|PA', 55, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Fat',    'AP|PA', 60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Thin',   'LAT',   55, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Medium', 'LAT',   55, 200, 18, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Fat',    'LAT',   60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- OBL (Scaphoid)
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Thin',   'OBL',   60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Medium', 'OBL',   60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Fat',    'OBL',   65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Paediatric','AP|PA', 50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='WRIST'), 'Paediatric','LAT',   50, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== HAND ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Thin',   'AP|PA', 55, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Medium', 'AP|PA', 55, 200, 12, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Fat',    'AP|PA', 60, 200, 18, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- OBL
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Thin',   'OBL',   55, 200, 12, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Medium', 'OBL',   55, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Fat',    'OBL',   60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  ((SELECT id FROM body_parts WHERE name='HAND'), 'Thin',   'LAT', 55, 200, 12, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Medium', 'LAT', 55, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Fat',    'LAT', 60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Paediatric','LAT', 50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='HAND'), 'Paediatric','AP|PA', 50, 200, 6, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== FINGERS ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Thin',   'AP|PA', 50, 200,  6, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Medium', 'AP|PA', 50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Fat',    'AP|PA', 50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Thin',   'LAT',   50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Medium', 'LAT',   50, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Fat',    'LAT',   50, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- OBL
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Thin',   'OBL',   50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Medium', 'OBL',   50, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Fat',    'OBL',   50, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='FINGERS'), 'Paediatric','AP|PA', 48, 200, 5, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== THUMB ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP|PA
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Thin',   'AP|PA', 52, 200,  6, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Medium', 'AP|PA', 52, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Fat',    'AP|PA', 52, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Thin',   'LAT',   52, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Medium', 'LAT',   52, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Fat',    'LAT',   52, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- OBL
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Thin',   'OBL',   52, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Medium', 'OBL',   52, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Fat',    'OBL',   52, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='THUMB'), 'Paediatric','AP|PA', 48, 200, 5, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== FEMUR ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='FEMUR'), 'Thin',   'AP|PA', 70, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FEMUR'), 'Medium', 'AP|PA', 75, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FEMUR'), 'Fat',    'AP|PA', 80, 200, 200, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='FEMUR'), 'Thin',   'LAT',   75, 200, 125, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FEMUR'), 'Medium', 'LAT',   80, 200, 160, NULL, NULL, NULL, 100, 110, 'Parallel', '8:1',  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FEMUR'), 'Fat',    'LAT',   85, 200, 250, NULL, NULL, NULL, 100, 110, 'Parallel', '10:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='FEMUR'), 'Paediatric','AP|PA', 60, 200, 50, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FEMUR'), 'Paediatric','LAT',   65, 200, 80, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);

-- ========== KNEE ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Thin',   'AP|PA', 65, 200,  40, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Medium', 'AP|PA', 70, 200,  60, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Fat',    'AP|PA', 75, 200,  90, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Thin',   'LAT',   70, 200,  50, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Medium', 'LAT',   75, 200,  75, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Fat',    'LAT',   80, 200, 110, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- OBL
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Thin',   'OBL',   70, 200,  50, NULL, NULL, NULL, 100, 110, NULL,      NULL,  NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Medium', 'OBL',   75, 200,  70, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Fat',    'OBL',   80, 200, 100, NULL, NULL, NULL, 100, 110, 'Parallel','8:1', NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Paediatric','AP|PA', 55, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='KNEE'), 'Paediatric','LAT',   60, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== PATELLA ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='PATELLA'), 'Thin',   'AP|PA', 60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PATELLA'), 'Medium', 'AP|PA', 60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PATELLA'), 'Fat',    'AP|PA', 65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- AXIAL (Sunrise)
  ((SELECT id FROM body_parts WHERE name='PATELLA'), 'Thin',   'AXIAL', 65, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PATELLA'), 'Medium', 'AXIAL', 70, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PATELLA'), 'Fat',    'AXIAL', 75, 200, 55, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric (gridsiz)
  ((SELECT id FROM body_parts WHERE name='PATELLA'), 'Paediatric','AP|PA', 55, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='PATELLA'), 'Paediatric','AXIAL', 60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== TIBFIB (Tibia & Fibula) ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='TIBIAFIBULA'), 'Thin',   'AP|PA', 60, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TIBIAFIBULA'), 'Medium', 'AP|PA', 65, 200, 45, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TIBIAFIBULA'), 'Fat',    'AP|PA', 70, 200, 60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='TIBIAFIBULA'), 'Thin',   'LAT',   65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TIBIAFIBULA'), 'Medium', 'LAT',   70, 200, 55, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TIBIAFIBULA'), 'Fat',    'LAT',   75, 200, 75, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric
  ((SELECT id FROM body_parts WHERE name='TIBIAFIBULA'), 'Paediatric','AP|PA', 55, 200, 18, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TIBIAFIBULA'), 'Paediatric','LAT',   60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== ANKLE ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Thin',   'AP|PA', 60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Medium', 'AP|PA', 60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Fat',    'AP|PA', 65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Thin',   'LAT',   60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Medium', 'LAT',   60, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Fat',    'LAT',   65, 200, 45, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- OBL (Mortise)
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Thin',   'OBL',   60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Medium', 'OBL',   60, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Fat',    'OBL',   65, 200, 45, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Paediatric','AP|PA', 55, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='ANKLE'), 'Paediatric','LAT',   55, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== FOOT ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Thin',   'AP|PA', 58, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Medium', 'AP|PA', 58, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Fat',    'AP|PA', 60, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- OBL
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Thin',   'OBL',   60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Medium', 'OBL',   60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Fat',    'OBL',   65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Thin',   'LAT',   60, 200, 25, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Medium', 'LAT',   60, 200, 30, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Fat',    'LAT',   65, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Paediatric','AP|PA', 52, 200, 12, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='FOOT'), 'Paediatric','LAT',   52, 200, 15, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== TOES ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AP/PA
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Thin',   'AP|PA', 50, 200,  6, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Medium', 'AP|PA', 50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Fat',    'AP|PA', 50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Thin',   'LAT',   50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Medium', 'LAT',   50, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Fat',    'LAT',   50, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- OBL
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Thin',   'OBL',   50, 200,  8, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Medium', 'OBL',   50, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Fat',    'OBL',   50, 200, 10, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric
  ((SELECT id FROM body_parts WHERE name='TOES'), 'Paediatric','AP|PA', 48, 200, 5, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);


-- ========== CALCANEUS (Heel Bone) ==========
INSERT INTO technique_parameters
(body_part_id, size, technique_profile, kvp, ma, ms, fkvp, fma, focal_spot,
 sid_min, sid_max, grid_type, grid_ratio, grid_speed, exposure_style, aec_field, aec_density)
VALUES
  -- AXIAL (Plantodorsal)
  ((SELECT id FROM body_parts WHERE name='CALCANEUS'), 'Thin',   'AXIAL', 70, 200, 40, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CALCANEUS'), 'Medium', 'AXIAL', 75, 200, 50, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CALCANEUS'), 'Fat',    'AXIAL', 80, 200, 70, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- LAT
  ((SELECT id FROM body_parts WHERE name='CALCANEUS'), 'Thin',   'LAT',   65, 200, 35, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CALCANEUS'), 'Medium', 'LAT',   70, 200, 45, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CALCANEUS'), 'Fat',    'LAT',   75, 200, 60, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),

  -- Paediatric
  ((SELECT id FROM body_parts WHERE name='CALCANEUS'), 'Paediatric','AXIAL', 60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL),
  ((SELECT id FROM body_parts WHERE name='CALCANEUS'), 'Paediatric','LAT',   60, 200, 20, NULL, NULL, NULL, 100, 110, NULL, NULL, NULL, 'Mas Mode', NULL, NULL);



-- ========= POSITIONERS MASTER SEED =========
-- Names: short, human-readable; no codes.
-- Descriptions: generic technique cues; device-agnostic.
-- Safe to re-run (updates description if name exists).

INSERT INTO positioners (position_name, description)
VALUES
-- ===== HEAD =====
('Head AP',                 'Head AP: occiput to detector; CR ⟂; SID ~100–110 cm'),
('Head PA',                 'Head PA: forehead/nose to detector; CR ⟂; SID ~100–110 cm'),
('Head Lateral',            'True lateral head: side of head to detector; IPL ⟂; SID ~100–110 cm'),

('Head Lateral Left',       'Left side to detector; IPL ⟂'),
('Head Lateral Right',      'Right side to detector; IPL ⟂'),

('Skull AP',                'Skull AP: OML ⟂; CR ⟂ to glabella; SID ~100–110 cm'),
('Skull Lateral',           'Skull lateral: IPL ⟂; CR 5 cm above EAM; SID ~100–110 cm'),
('Skull PA',                    'Skull PA: OML ⟂; CR ⟂ to glabella; SID ~100–110 cm'),
('Skull AP Axial (Towne)',      'Skull AP axial (Towne): OML ⟂; 30° caudad; CR 6–7 cm above glabella'),
('Skull SMV',                   'Skull SMV (submentovertex): IOML ∥; vertex to detector'),

('Sinuses Waters',          'Paranasal sinuses Waters: chin on detector; OML ~37°; erect'),
('Sinuses Caldwell',        'Paranasal sinuses PA Caldwell: 15–20° caudal; erect'),
('Sinuses Lateral',         'Sinuses lateral: side of face to detector; erect'),

('Orbits PA',               'Orbits PA: forehead/nose to detector; CR ⟂; SID ~100–110 cm'),
('Orbits Lateral',          'Orbits lateral: true lateral orbit; SID ~100–110 cm'),
('Orbits Rhese',                'Orbits Rhese (parietoorbital): chin/cheek/nose on detector; AML ⟂ to IR'),

('Nasal Bones Lateral',     'Nasal bones lateral: CR to bridge; SID ~100–110 cm'),
('Nasal Bones AP/PA',           'Nasal bones AP/PA: CR ⟂ to nasion'),

('Facial Bones Waters',     'Facial bones Waters: chin on detector; OML ~37°; erect'),
('Facial Bones Lateral',    'Facial bones lateral: IPL ⟂; SID ~100–110 cm'),
('Facial Bones Caldwell',       'Facial bones PA axial (Caldwell): 15° caudad; OML ⟂'),

('Mandible PA',             'Mandible PA: forehead/nose or chin to detector; CR ⟂'),
('Mandible Oblique',        'Mandible oblique: 25–30° rotation; affected side to detector'),
('Mandible Axial (Towne)',      'Mandible AP axial (Towne): 35–40° caudad; CR to glabella'),
('Mandible SMV',                'Mandible SMV: IOML ∥; CR midway between angles'),

('TMJ Lateral',             'TMJ lateral: true lateral TMJ; open/closed mouth as needed'),
('TMJ Open Mouth',          'TMJ open-mouth: mouth open; condylar excursion assessment'),
('TMJ Closed Mouth',        'TMJ closed-mouth: teeth together; baseline joint position'),
('TMJ AP',                      'TMJ AP: closed/open mouth as requested; condylar position'),
('TMJ Oblique (Transcranial)',  'TMJ transcranial oblique: 25–30°; open/closed mouth options'),

-- ===== NECK =====
('Neck AP',                 'Neck AP: CR ⟂ to C5–C6; minimal lordotic angle'),
('Neck Lateral',            'Neck lateral: left lateral preferred; shoulders relaxed'),

('Soft Tissue Neck AP',     'Soft tissue neck AP: slow inspiration; CR ⟂ to larynx'),
('Soft Tissue Neck Lateral','Soft tissue neck lateral: airway profile; erect if possible'),

-- ===== SPINE =====
('C-Spine AP',              'Cervical spine AP: chin slightly elevated; CR to C4'),
('C-Spine Lateral',         'Cervical spine lateral: shoulders down; CR to C4'),
('C-Spine Oblique',         'Cervical spine oblique: 45° rotation; CR 15° cephalad'),
('C-Spine Odontoid',        'Open-mouth odontoid: mouth open; CR to center of mouth'),
('C-Spine Swimmers Lateral',  'Cervicothoracic (Swimmer’s) lateral: one arm up/one down; CR to C7–T1'),
('T-Spine Swimmers Lateral',  'Cervicothoracic (Swimmer’s) lateral for T1: arm positioning as above'),

('T-Spine AP',              'Thoracic spine AP: CR to T7; gentle expiration'),
('T-Spine Lateral',         'Thoracic spine lateral: arms forward; CR to T7'),

('L-Spine AP',              'Lumbar spine AP: hips/knees flexed if supine; CR to L3–L4'),
('L-Spine Lateral',         'Lumbar spine lateral: left lateral; CR to L3–L4'),
('L-Spine Oblique',             'Lumbar spine oblique (RPO/LPO) 45°: zygapophyseal joints'),
('L5-S1 Spot Lateral',          'Lumbosacral lateral (spot): CR to L5–S1'),

('Sacrum AP',               'Sacrum AP: CR 15° cephalad; CR 5 cm below ASIS'),
('Coccyx AP',               'Coccyx AP: CR 10° caudad; CR 5 cm below ASIS'),
('Sacrum/Coccyx Lateral',   'Sacrum/Coccyx lateral: CR to sacrum/coccyx profile'),

('Scoliosis PA',            'Whole-spine scoliosis PA: erect; low-dose setup'),
('Scoliosis Lateral',       'Whole-spine scoliosis lateral: erect; low-dose setup'),

-- ===== THORAX =====
('Chest PA',                'Chest PA: erect, anterior chest to detector; deep inspiration; SID ~180 cm'),
('Chest Lateral',           'Chest lateral: left side to detector; arms up; SID ~180 cm'),

('Ribs AP',                 'Ribs AP/PA: area of interest centered; inspiration/expiration per region'),
('Ribs Oblique',            'Ribs oblique: 30–45° rotation; affected side to detector if posterior ribs'),

('Sternum RAO',             'Sternum RAO: 15–20° RAO; sternum over heart shadow'),
('Sternum Lateral',         'Sternum lateral: arms behind back; deep inspiration'),

('Clavicle AP',             'Clavicle AP: CR ⟂ to mid-clavicle'),
('Clavicle AP Axial',       'Clavicle AP axial: 15–30° cephalad tilt'),

('Scapula AP',              'Scapula AP: arm abducted; CR to mid-scapula'),
('Scapula Y Lateral',       'Scapular Y lateral: 60–70° oblique; CR to scapulohumeral joint'),

('AC Joints AP',            'AC joints AP: bilateral; erect; without weights'),
('AC Joints AP With Weights','AC joints AP with weights: bilateral; erect'),

-- ===== ABDOMEN =====
('Abdomen AP Supine',       'Abdomen (KUB) AP supine: CR at iliac crests'),
('Abdomen AP Erect',        'Abdomen AP erect: CR 5–7 cm above crests for air–fluid levels'),
('Abdomen Left Lateral Decubitus', 'Left lateral decubitus: air–fluid/ascites; horizontal beam'),
('Abdomen Dorsal Decubitus',       'Dorsal decubitus: horizontal beam for free air/obstruction'),

-- ===== PELVIS =====
('Pelvis AP',               'Pelvis AP: feet internally rotated 15–20° if tolerated'),
('Pelvis Inlet',            'Pelvis inlet: 30–40° caudad CR'),
('Pelvis Outlet',           'Pelvis outlet: 30–45° cephalad CR'),
('Pelvis Lateral (Cross-Table)',   'Pelvis cross-table lateral: trauma setup; horizontal beam'),

('Hip AP',                  'Hip AP: leg internally rotated 15–20°; CR to femoral neck'),
('Hip Frog Lateral',        'Hip frog-leg lateral: abducted/external rotation'),
('Hip Cross-Table Lateral',        'Hip cross-table (Danelius–Miller): IR vertical; horizontal beam'),

('SI Joints Oblique',       'Sacroiliac joints oblique: 25–30° posterior oblique'),
('SI Joints AP Axial (Ferguson)',  'SI joints AP axial (Ferguson): 30–35° cephalad; CR midline'),

-- ===== UPPER EXTREMITY =====
('Shoulder AP External',    'Shoulder AP external rotation: epicondyles ⟂ to beam'),
('Shoulder AP Internal',    'Shoulder AP internal rotation: epicondyles ∥ to beam'),
('Shoulder Axial',          'Shoulder inferosuperior axial: arm abducted if tolerated'),
('Shoulder Scapular Y',     'Shoulder scapular Y lateral: 60–70° oblique'),

('Humerus AP',              'Humerus AP: arm extended/supinated; include shoulder & elbow'),
('Humerus Lateral',         'Humerus lateral: elbow flexed 90°; include shoulder & elbow'),

('Elbow AP',                'Elbow AP: arm extended, hand supinated'),
('Elbow Lateral',           'Elbow lateral: 90° flexion; thumb up'),
('Elbow Oblique',           'Elbow oblique: internal/external as required'),

('Forearm AP',              'Forearm AP: hand supinated; include wrist & elbow'),
('Forearm Lateral',         'Forearm lateral: true lateral; include wrist & elbow'),

('Wrist PA',                'Wrist PA: palm flat; fingers flexed'),
('Wrist Oblique',           'Wrist oblique: 45° pronated oblique'),
('Wrist Lateral',           'Wrist lateral: true lateral; thumb up'),
('Wrist Ulnar Deviation',   'Wrist scaphoid view: PA with ulnar deviation'),

('Hand PA',                 'Hand PA: fingers separated; palm flat'),
('Hand Oblique',            'Hand oblique: 45° pronated oblique'),
('Hand Lateral',            'Hand lateral: fan lateral as needed'),

('Finger PA',               'Finger PA: affected digit PA'),
('Finger Oblique',          'Finger oblique: 45° rotation'),
('Finger Lateral',          'Finger lateral: true lateral of digit'),

('Thumb AP',                'Thumb AP: hand internally rotated; CR to MCP'),
('Thumb Lateral',           'Thumb lateral: true lateral'),

-- ===== LOWER EXTREMITY =====
('Femur AP',                'Femur AP: leg internally rotated 5–15°; include joint'),
('Femur Lateral',           'Femur lateral: true or cross-table lateral'),

('Knee AP',                 'Knee AP: leg extended; CR 0–5° per habitus'),
('Knee Lateral',            'Knee lateral: 20–30° flexion; CR 5–7° cephalad'),
('Knee Oblique',            'Knee oblique: 45° medial or lateral rotation'),

('Patella PA',              'Patella PA: CR ⟂ to patella'),
('Patella Skyline',         'Patella tangential (skyline/merchant): CR angled to patellofemoral joint'),

('Tibia/Fibula AP',         'Tibia/Fibula AP: include knee & ankle'),
('Tibia/Fibula Lateral',    'Tibia/Fibula lateral: include knee & ankle'),

('Ankle AP',                'Ankle AP: dorsiflexed; CR midway between malleoli'),
('Ankle Mortise',           'Ankle mortise: 15–20° medial rotation'),
('Ankle Lateral',           'Ankle lateral: lateral malleolus posterior'),

('Foot AP',                 'Foot AP/DP: plantar surface on detector; CR 10° toward heel if needed'),
('Foot Oblique',            'Foot oblique: 30–45° medial rotation'),
('Foot Lateral',            'Foot lateral: true lateral'),

('Toes AP',                 'Toes AP: CR 10–15° toward heel if needed'),
('Toes Oblique',            'Toes oblique: 30–45° rotation'),
('Toes Lateral',            'Toes lateral: individual digit lateral'),

('Calcaneus Axial',         'Calcaneus axial (plantodorsal): CR 40° cephalad'),
('Calcaneus Lateral',       'Calcaneus lateral: CR 2.5 cm distal to medial malleolus'),

('Chest AP Supine',                'Chest AP supine/portable: CR ~8–10 cm below jugular notch; SID as available');



-- =========================
-- VIEWS — HEAD & FACE
-- =========================

-- HEAD
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Head AP',
 'Head AP: occiput to detector; CR ⟂',
 (SELECT id FROM body_parts WHERE name='HEAD'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Head AP', TRUE),
('Head PA',
 'Head PA: forehead/nose to detector',
 (SELECT id FROM body_parts WHERE name='HEAD'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Head PA', TRUE),
('Head LAT L',
 'Head true lateral (left side to detector)',
 (SELECT id FROM body_parts WHERE name='HEAD'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Head Lateral Left', TRUE),
('Head LAT R',
 'Head true lateral (right side to detector)',
 (SELECT id FROM body_parts WHERE name='HEAD'),
 'Lateral','LAT',
 NULL, NULL,
 'RL', NULL, NULL, 0,
 'R','RIGHT TOP','Head Lateral Right', TRUE);

-- SKULL
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Skull AP',
 'Skull AP: OML ⟂; CR ⟂ to glabella',
 (SELECT id FROM body_parts WHERE name='SKULL'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Skull AP', TRUE),
('Skull LAT',
 'Skull lateral: IPL ⟂; CR ~5 cm above EAM',
 (SELECT id FROM body_parts WHERE name='SKULL'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Skull Lateral', TRUE);

-- SINUSES
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Sinus Waters',
 'Waters: chin on detector; OML ~37°; erect',
 (SELECT id FROM body_parts WHERE name='SINUSES'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Sinuses Waters', TRUE),
('Sinus Caldwell',
 'PA Caldwell: 15–20° caudal; erect',
 (SELECT id FROM body_parts WHERE name='SINUSES'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Sinuses Caldwell', TRUE),
('Sinus LAT',
 'Sinuses lateral: side of face to detector',
 (SELECT id FROM body_parts WHERE name='SINUSES'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Sinuses Lateral', TRUE);

-- ORBITS
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Orbits PA',
 'Orbits PA: forehead/nose to detector',
 (SELECT id FROM body_parts WHERE name='ORBITS'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Orbits PA', TRUE),
('Orbits LAT',
 'Orbits lateral: true lateral orbit',
 (SELECT id FROM body_parts WHERE name='ORBITS'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Orbits Lateral', TRUE);

-- NASAL BONES
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Nasal LAT',
 'Nasal bones lateral: CR to bridge',
 (SELECT id FROM body_parts WHERE name='NASALBONES'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Nasal Bones Lateral', TRUE);

-- FACIAL BONES
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Facial Waters',
 'Facial Waters: chin on detector; OML ~37°',
 (SELECT id FROM body_parts WHERE name='FACIALBONES'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Facial Bones Waters', TRUE),
('Facial LAT',
 'Facial bones lateral: IPL ⟂',
 (SELECT id FROM body_parts WHERE name='FACIALBONES'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Facial Bones Lateral', TRUE);

-- MANDIBLE
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Mandible PA',
 'Mandible PA: forehead/nose or chin to detector',
 (SELECT id FROM body_parts WHERE name='MANDIBLE'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'PA','RIGHT TOP','Mandible PA', TRUE),
('Mandible OBL',
 'Mandible axiolateral oblique: 25–30° rotation',
 (SELECT id FROM body_parts WHERE name='MANDIBLE'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Mandible Oblique', TRUE);

-- TMJ
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('TMJ LAT',
 'TMJ lateral: true lateral (open/closed mouth as requested)',
 (SELECT id FROM body_parts WHERE name='TMJ'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','TMJ Lateral', TRUE),
('TMJ AP Open',
 'TMJ AP open mouth',
 (SELECT id FROM body_parts WHERE name='TMJ'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','TMJ Open Mouth', TRUE),
('TMJ AP Closed',
 'TMJ AP closed mouth',
 (SELECT id FROM body_parts WHERE name='TMJ'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','TMJ Closed Mouth', TRUE);

-- =========================
-- VIEWS — NECK & SPINE
-- =========================

-- NECK
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Neck AP',
 'Neck AP: CR ⟂ to C5–C6',
 (SELECT id FROM body_parts WHERE name='NECK'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Neck AP', TRUE),
('Neck LAT',
 'Neck lateral: left lateral preferred',
 (SELECT id FROM body_parts WHERE name='NECK'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Neck Lateral', TRUE);

-- SOFT TISSUE NECK
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('ST Neck AP',
 'Soft tissue neck AP',
 (SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Soft Tissue Neck AP', TRUE),
('ST Neck LAT',
 'Soft tissue neck lateral',
 (SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Soft Tissue Neck Lateral', TRUE);

-- C-SPINE
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('C-Spine AP',
 'Cervical spine AP: chin slightly elevated; CR to C4',
 (SELECT id FROM body_parts WHERE name='CSPINE'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','C-Spine AP', TRUE),
('C-Spine LAT',
 'Cervical spine lateral: shoulders down; CR to C4',
 (SELECT id FROM body_parts WHERE name='CSPINE'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','C-Spine Lateral', TRUE),
('C-Spine OBL',
 'Cervical spine oblique: 45° rotation; CR 15° cephalad',
 (SELECT id FROM body_parts WHERE name='CSPINE'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','C-Spine Oblique', TRUE),
('C-Spine OMO',
 'Open-mouth odontoid',
 (SELECT id FROM body_parts WHERE name='CSPINE'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','C-Spine Odontoid', TRUE);

-- T-SPINE
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('T-Spine AP',
 'Thoracic spine AP: CR to T7; gentle expiration',
 (SELECT id FROM body_parts WHERE name='TSPINE'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','T-Spine AP', TRUE),
('T-Spine LAT',
 'Thoracic spine lateral: arms forward; CR to T7',
 (SELECT id FROM body_parts WHERE name='TSPINE'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','T-Spine Lateral', TRUE);

-- L-SPINE
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('L-Spine AP',
 'Lumbar spine AP',
 (SELECT id FROM body_parts WHERE name='LSPINE'),
 'Supine','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','L-Spine AP', TRUE),
('L-Spine LAT',
 'Lumbar spine lateral',
 (SELECT id FROM body_parts WHERE name='LSPINE'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','L-Spine Lateral', TRUE);

-- SACRUM / COCCYX
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Sacrum AP',
 'Sacrum AP axial (per department protocol)',
 (SELECT id FROM body_parts WHERE name='SSPINE'),
 'Supine','AXIAL',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Sacrum AP', TRUE),
('Sac/Cocc LAT',
 'Sacrum/Coccyx lateral',
 (SELECT id FROM body_parts WHERE name='SSPINE'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Sacrum/Coccyx Lateral', TRUE),
('Coccyx AP',
 'Coccyx AP axial (per department protocol)',
 (SELECT id FROM body_parts WHERE name='COCCYX'),
 'Supine','AXIAL',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Coccyx AP', TRUE);

-- SCOLIOSIS
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Scoliosis PA',
 'Whole-spine scoliosis PA: erect; low dose',
 (SELECT id FROM body_parts WHERE name='SCOLIOSIS'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Scoliosis PA', TRUE),
('Scoliosis LAT',
 'Whole-spine scoliosis lateral: erect; low dose',
 (SELECT id FROM body_parts WHERE name='SCOLIOSIS'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Scoliosis Lateral', TRUE);

-- =========================
-- VIEWS — THORAX & SHOULDER GIRDLE
-- =========================

-- CHEST
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Chest PA',
 'Chest PA: erect; deep inspiration; SID ~180 cm',
 (SELECT id FROM body_parts WHERE name='CHEST'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Chest PA', TRUE),
('Chest LAT',
 'Chest lateral: left side to detector',
 (SELECT id FROM body_parts WHERE name='CHEST'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Chest Lateral', TRUE);

-- RIBS
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Ribs AP',
 'Ribs AP/PA: area of interest centered',
 (SELECT id FROM body_parts WHERE name='RIBS'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Ribs AP', TRUE),
('Ribs OBL',
 'Ribs oblique: 30–45° rotation',
 (SELECT id FROM body_parts WHERE name='RIBS'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Ribs Oblique', TRUE);

-- STERNUM
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Sternum RAO',
 'Sternum RAO: 15–20°; over heart shadow',
 (SELECT id FROM body_parts WHERE name='STERNUM'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Sternum RAO', TRUE),
('Sternum LAT',
 'Sternum lateral; deep inspiration',
 (SELECT id FROM body_parts WHERE name='STERNUM'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Sternum Lateral', TRUE);

-- CLAVICLE
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Clavicle AP',
 'Clavicle AP',
 (SELECT id FROM body_parts WHERE name='CLAVICLE'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Clavicle AP', TRUE),
('Clavicle AX',
 'Clavicle AP axial: 15–30° cephalad',
 (SELECT id FROM body_parts WHERE name='CLAVICLE'),
 'Erect','AXIAL',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Clavicle AP Axial', TRUE);

-- SCAPULA
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Scapula AP',
 'Scapula AP',
 (SELECT id FROM body_parts WHERE name='SCAPULA'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Scapula AP', TRUE),
('Scapula Y LAT',
 'Scapular Y lateral',
 (SELECT id FROM body_parts WHERE name='SCAPULA'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Scapula Y Lateral', TRUE);

-- AC JOINTS (bilateral)
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('ACJ AP',
 'AC joints AP (bilateral)',
 (SELECT id FROM body_parts WHERE name='ACJOINTS'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', 'B', NULL, 0,
 'AP','RIGHT TOP','AC Joints AP', TRUE),
('ACJ AP+W',
 'AC joints AP with weights (bilateral)',
 (SELECT id FROM body_parts WHERE name='ACJOINTS'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', 'B', NULL, 0,
 'AP','RIGHT TOP','AC Joints AP With Weights', TRUE);

-- =========================
-- VIEWS — ABDOMEN, PELVIS, HIP, SI JOINTS
-- =========================

-- ABDOMEN
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Abdomen Supine',
 'Abdomen AP supine (KUB)',
 (SELECT id FROM body_parts WHERE name='ABDOMEN'),
 'Supine','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Abdomen AP Supine', TRUE),
('Abdomen Erect',
 'Abdomen AP erect: air–fluid levels',
 (SELECT id FROM body_parts WHERE name='ABDOMEN'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Abdomen AP Erect', TRUE);

-- PELVIS
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Pelvis AP',
 'Pelvis AP: feet internally rotated 15–20° if tolerated',
 (SELECT id FROM body_parts WHERE name='PELVIS'),
 'Supine','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Pelvis AP', TRUE),
('Pelvis Inlet',
 'Pelvis inlet (AP axial caudad)',
 (SELECT id FROM body_parts WHERE name='PELVIS'),
 'Supine','AXIAL',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Pelvis Inlet', TRUE),
('Pelvis Outlet',
 'Pelvis outlet (AP axial cephalad)',
 (SELECT id FROM body_parts WHERE name='PELVIS'),
 'Supine','AXIAL',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Pelvis Outlet', TRUE);

-- HIP
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Hip AP',
 'Hip AP: leg internally rotated 15–20°',
 (SELECT id FROM body_parts WHERE name='HIP'),
 'Supine','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Hip AP', TRUE),
('Hip Frog LAT',
 'Hip frog-leg lateral',
 (SELECT id FROM body_parts WHERE name='HIP'),
 'Supine','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Hip Frog Lateral', TRUE);

-- SI JOINTS
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('SIJ OBL',
 'Sacroiliac joints oblique (RPO/LPO)',
 (SELECT id FROM body_parts WHERE name='SACROILIAC JOINTS'),
 'Supine','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','SI Joints Oblique', TRUE);

-- =========================
-- VIEWS — UPPER EXTREMITY
-- =========================

-- SHOULDER
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Shoulder AP Ext',
 'Shoulder AP external rotation',
 (SELECT id FROM body_parts WHERE name='SHOULDER'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Shoulder AP External', TRUE),
('Shoulder AP Int',
 'Shoulder AP internal rotation',
 (SELECT id FROM body_parts WHERE name='SHOULDER'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Shoulder AP Internal', TRUE),
('Shoulder Axial',
 'Shoulder inferosuperior axial',
 (SELECT id FROM body_parts WHERE name='SHOULDER'),
 'Supine','AXIAL',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Shoulder Axial', TRUE),
('Shoulder Y LAT',
 'Shoulder scapular Y lateral',
 (SELECT id FROM body_parts WHERE name='SHOULDER'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Shoulder Scapular Y', TRUE);

-- HUMERUS
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Humerus AP',
 'Humerus AP',
 (SELECT id FROM body_parts WHERE name='HUMERUS'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Humerus AP', TRUE),
('Humerus LAT',
 'Humerus lateral',
 (SELECT id FROM body_parts WHERE name='HUMERUS'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Humerus Lateral', TRUE);

-- ELBOW
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Elbow AP',
 'Elbow AP: arm extended, hand supinated',
 (SELECT id FROM body_parts WHERE name='ELBOW'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Elbow AP', TRUE),
('Elbow LAT',
 'Elbow lateral: 90° flexion; thumb up',
 (SELECT id FROM body_parts WHERE name='ELBOW'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Elbow Lateral', TRUE),
('Elbow OBL',
 'Elbow oblique: internal/external as required',
 (SELECT id FROM body_parts WHERE name='ELBOW'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Elbow Oblique', TRUE);

-- FOREARM
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Forearm AP',
 'Forearm AP: include wrist & elbow',
 (SELECT id FROM body_parts WHERE name='FOREARM'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Forearm AP', TRUE),
('Forearm LAT',
 'Forearm lateral: include wrist & elbow',
 (SELECT id FROM body_parts WHERE name='FOREARM'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Forearm Lateral', TRUE);

-- WRIST
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Wrist PA',
 'Wrist PA: palm flat; fingers flexed',
 (SELECT id FROM body_parts WHERE name='WRIST'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Wrist PA', TRUE),
('Wrist OBL',
 'Wrist oblique: 45° pronated oblique',
 (SELECT id FROM body_parts WHERE name='WRIST'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Wrist Oblique', TRUE),
('Wrist LAT',
 'Wrist lateral: true lateral; thumb up',
 (SELECT id FROM body_parts WHERE name='WRIST'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Wrist Lateral', TRUE),
('Wrist Ulnar Dev',
 'Wrist scaphoid view: PA with ulnar deviation',
 (SELECT id FROM body_parts WHERE name='WRIST'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Wrist Ulnar Deviation', TRUE);

-- HAND
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Hand PA',
 'Hand PA: fingers separated; palm flat',
 (SELECT id FROM body_parts WHERE name='HAND'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Hand PA', TRUE),
('Hand OBL',
 'Hand oblique: 45° pronated oblique',
 (SELECT id FROM body_parts WHERE name='HAND'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Hand Oblique', TRUE),
('Hand LAT',
 'Hand lateral: fan lateral as needed',
 (SELECT id FROM body_parts WHERE name='HAND'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Hand Lateral', TRUE);

-- FINGERS
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Finger PA',
 'Finger PA: affected digit PA',
 (SELECT id FROM body_parts WHERE name='FINGERS'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Finger PA', TRUE),
('Finger OBL',
 'Finger oblique: 45° rotation',
 (SELECT id FROM body_parts WHERE name='FINGERS'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Finger Oblique', TRUE),
('Finger LAT',
 'Finger lateral: true lateral of digit',
 (SELECT id FROM body_parts WHERE name='FINGERS'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Finger Lateral', TRUE);

-- THUMB
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Thumb AP',
 'Thumb AP: hand internally rotated; CR to MCP',
 (SELECT id FROM body_parts WHERE name='THUMB'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Thumb AP', TRUE),
('Thumb LAT',
 'Thumb lateral: true lateral',
 (SELECT id FROM body_parts WHERE name='THUMB'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Thumb Lateral', TRUE);

-- =========================
-- VIEWS — LOWER EXTREMITY
-- =========================

-- FEMUR
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Femur AP',
 'Femur AP: leg internally rotated 5–15°; include joint',
 (SELECT id FROM body_parts WHERE name='FEMUR'),
 'Supine','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Femur AP', TRUE),
('Femur LAT',
 'Femur lateral: true or cross-table',
 (SELECT id FROM body_parts WHERE name='FEMUR'),
 'Lateral','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Femur Lateral', TRUE);

-- KNEE
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Knee AP',
 'Knee AP: leg extended',
 (SELECT id FROM body_parts WHERE name='KNEE'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Knee AP', TRUE),
('Knee LAT',
 'Knee lateral: 20–30° flexion',
 (SELECT id FROM body_parts WHERE name='KNEE'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Knee Lateral', TRUE),
('Knee OBL',
 'Knee oblique: 45° rotation',
 (SELECT id FROM body_parts WHERE name='KNEE'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Knee Oblique', TRUE);

-- PATELLA
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Patella PA',
 'Patella PA',
 (SELECT id FROM body_parts WHERE name='PATELLA'),
 'Erect','AP|PA',
 NULL, NULL,
 'PA', NULL, NULL, 0,
 'PA','RIGHT TOP','Patella PA', TRUE),
('Patella Skyline',
 'Patella tangential (skyline/merchant)',
 (SELECT id FROM body_parts WHERE name='PATELLA'),
 'Erect','AXIAL',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Patella Skyline', TRUE);

-- TIBIA/FIBULA
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Tib-Fib AP',
 'Tibia/Fibula AP: include knee & ankle',
 (SELECT id FROM body_parts WHERE name='TIBIAFIBULA'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Tibia/Fibula AP', TRUE),
('Tib-Fib LAT',
 'Tibia/Fibula lateral: include knee & ankle',
 (SELECT id FROM body_parts WHERE name='TIBIAFIBULA'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Tibia/Fibula Lateral', TRUE);

-- ANKLE
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Ankle AP',
 'Ankle AP: dorsiflexed; CR midway between malleoli',
 (SELECT id FROM body_parts WHERE name='ANKLE'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Ankle AP', TRUE),
('Ankle Mortise',
 'Ankle mortise: 15–20° medial rotation',
 (SELECT id FROM body_parts WHERE name='ANKLE'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Ankle Mortise', TRUE),
('Ankle LAT',
 'Ankle lateral: lateral malleolus posterior',
 (SELECT id FROM body_parts WHERE name='ANKLE'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Ankle Lateral', TRUE);

-- FOOT
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Foot AP',
 'Foot AP/DP: CR 10° toward heel if needed',
 (SELECT id FROM body_parts WHERE name='FOOT'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Foot AP', TRUE),
('Foot OBL',
 'Foot oblique: 30–45° medial rotation',
 (SELECT id FROM body_parts WHERE name='FOOT'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Foot Oblique', TRUE),
('Foot LAT',
 'Foot lateral: true lateral',
 (SELECT id FROM body_parts WHERE name='FOOT'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Foot Lateral', TRUE);

-- TOES
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Toes AP',
 'Toes AP: CR 10–15° toward heel if needed',
 (SELECT id FROM body_parts WHERE name='TOES'),
 'Erect','AP|PA',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Toes AP', TRUE),
('Toes OBL',
 'Toes oblique: 30–45° rotation',
 (SELECT id FROM body_parts WHERE name='TOES'),
 'Erect','OBL',
 NULL, NULL,
 'RLO', NULL, NULL, 0,
 'OBL','RIGHT TOP','Toes Oblique', TRUE),
('Toes LAT',
 'Toes lateral: individual digit lateral',
 (SELECT id FROM body_parts WHERE name='TOES'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Toes Lateral', TRUE);

-- CALCANEUS
INSERT INTO views
(name, description, body_part_id, patient_position, projection_profile,
 patient_orientation_row, patient_orientation_col,
 view_position, image_laterality, image_rotate, image_horizontal_flip,
 label_mark, label_position, position_name, is_active)
VALUES
('Calcaneus AX',
 'Calcaneus axial (plantodorsal): CR 40° cephalad',
 (SELECT id FROM body_parts WHERE name='CALCANEUS'),
 'Erect','AXIAL',
 NULL, NULL,
 'AP', NULL, NULL, 0,
 'AP','RIGHT TOP','Calcaneus Axial', TRUE),
('Calcaneus LAT',
 'Calcaneus lateral: CR ~2.5 cm distal to medial malleolus',
 (SELECT id FROM body_parts WHERE name='CALCANEUS'),
 'Erect','LAT',
 NULL, NULL,
 'LL', NULL, NULL, 0,
 'L','RIGHT TOP','Calcaneus Lateral', TRUE);

 -- =========================
-- BIND TECHNIQUES TO ALL VIEWS (NO HARD-CODED IDS)
-- =========================

INSERT INTO view_techniques (view_id, technique_parameter_id, seq, role)
SELECT
  v.id,
  tp.id,
  CASE tp.size
    WHEN 'Thin'       THEN 1
    WHEN 'Medium'     THEN 2
    WHEN 'Fat'        THEN 3
    WHEN 'Paediatric' THEN 4
  END AS seq,
  'PRIMARY' AS role
FROM views v
JOIN technique_parameters tp
  ON tp.body_part_id = v.body_part_id
 AND tp.technique_profile = v.projection_profile
LEFT JOIN view_techniques vt
  ON vt.view_id = v.id
 AND vt.technique_parameter_id = tp.id
WHERE vt.view_id IS NULL;




-- ================= HEAD & FACE =================
INSERT INTO procedures
(name, code_value, code_meaning, is_true_size, print_orientation, print_format, anatomic_region_id, body_part_id, is_active)
VALUES
('Head Routine',        NULL,'Head Routine',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='HEAD'),TRUE),
('Skull Series',        NULL,'Skull Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='SKULL'),TRUE),
('Sinus Study',         NULL,'Paranasal Sinuses Study',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='SINUSES'),TRUE),
('Facial Bones',        NULL,'Facial Bones Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='FACIALBONES'),TRUE),
('Mandible Series',     NULL,'Mandible Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='MANDIBLE'),TRUE),
('TMJ Study',           NULL,'TMJ Study',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='TMJ'),TRUE),
('Orbit Series',        NULL,'Orbit Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='ORBITS'),TRUE),
('Nasal Bones',         NULL,'Nasal Bones',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='NASALBONES'),TRUE);

-- ============== NECK & SPINE =================
INSERT INTO procedures
(name, code_value, code_meaning, is_true_size, print_orientation, print_format, anatomic_region_id, body_part_id, is_active)
VALUES
('Neck Routine',            NULL,'Neck Routine',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='NECK'),TRUE),
('Soft Tissue Neck',        NULL,'Soft Tissue Neck',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='SOFTTISSUENECK'),TRUE),
('Cervical Spine Series',   NULL,'Cervical Spine Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='CSPINE'),TRUE),
('Thoracic Spine',          NULL,'Thoracic Spine AP/LAT',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='TSPINE'),TRUE),
('Lumbar Spine',            NULL,'Lumbar Spine AP/LAT',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='LSPINE'),TRUE),
('Sacrum/Coccyx',           NULL,'Sacrum/Coccyx Study',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='SSPINE'),TRUE),
('Scoliosis Survey',        NULL,'Whole-Spine Scoliosis',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='SCOLIOSIS'),TRUE);

-- ======= THORAX/CHEST & SHOULDER GIRDLE =======
INSERT INTO procedures
(name, code_value, code_meaning, is_true_size, print_orientation, print_format, anatomic_region_id, body_part_id, is_active)
VALUES
('Chest Routine',     NULL,'Chest PA and Lateral',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='CHEST'),TRUE),
('Rib Series',        NULL,'Ribs Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='RIBS'),TRUE),
('Sternum Series',    NULL,'Sternum Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='STERNUM'),TRUE),
('Clavicle Series',   NULL,'Clavicle Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='CLAVICLE'),TRUE),
('Scapula Series',    NULL,'Scapula Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='SCAPULA'),TRUE),
('AC Joints',         NULL,'Acromioclavicular Joints',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='ACJOINTS'),TRUE);

-- ================ ABDOMEN & PELVIS ================
INSERT INTO procedures
(name, code_value, code_meaning, is_true_size, print_orientation, print_format, anatomic_region_id, body_part_id, is_active)
VALUES
('Abdomen Routine',       NULL,'Abdomen Supine + Erect',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='ABDOMEN'),TRUE),
('Pelvis AP',             NULL,'Pelvis AP',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='PELVIS'),TRUE),
('Pelvis Inlet/Outlet',   NULL,'Pelvis Inlet and Outlet',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='PELVIS'),TRUE),
('Hip Series',            NULL,'Hip Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='HIP'),TRUE),
('SI Joint Series',       NULL,'Sacroiliac Joints Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='SACROILIAC JOINTS'),TRUE);

-- =============== UPPER EXTREMITY ===============
INSERT INTO procedures
(name, code_value, code_meaning, is_true_size, print_orientation, print_format, anatomic_region_id, body_part_id, is_active)
VALUES
('Shoulder Series',   NULL,'Shoulder Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='SHOULDER'),TRUE),
('Humerus Routine',   NULL,'Humerus AP and Lateral',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='HUMERUS'),TRUE),
('Elbow Series',      NULL,'Elbow Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='ELBOW'),TRUE),
('Forearm Routine',   NULL,'Forearm AP and Lateral',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='FOREARM'),TRUE),
('Wrist Series',      NULL,'Wrist Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='WRIST'),TRUE),
('Hand Series',       NULL,'Hand Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='HAND'),TRUE),
('Finger Series',     NULL,'Finger Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='FINGERS'),TRUE),
('Thumb Series',      NULL,'Thumb Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='THUMB'),TRUE);

-- =============== LOWER EXTREMITY ===============
INSERT INTO procedures
(name, code_value, code_meaning, is_true_size, print_orientation, print_format, anatomic_region_id, body_part_id, is_active)
VALUES
('Femur Routine',     NULL,'Femur AP and Lateral',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='FEMUR'),TRUE),
('Knee Series',       NULL,'Knee Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='KNEE'),TRUE),
('Patella Study',     NULL,'Patella Study',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='PATELLA'),TRUE),
('Tib-Fib Routine',   NULL,'Tibia/Fibula AP and Lateral',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='TIBIAFIBULA'),TRUE),
('Ankle Series',      NULL,'Ankle Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='ANKLE'),TRUE),
('Foot Series',       NULL,'Foot Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='FOOT'),TRUE),
('Toe Series',        NULL,'Toes Series',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='TOES'),TRUE),
('Calcaneus Study',   NULL,'Calcaneus Study',0,NULL,NULL,NULL,(SELECT id FROM body_parts WHERE name='CALCANEUS'),TRUE);

-- ================= HEAD & FACE =================
INSERT INTO procedure_views (procedure_id, view_id) VALUES
((SELECT id FROM procedures WHERE name='Head Routine'), (SELECT id FROM views WHERE name='Head AP')),
((SELECT id FROM procedures WHERE name='Head Routine'), (SELECT id FROM views WHERE name='Head PA')),
((SELECT id FROM procedures WHERE name='Head Routine'), (SELECT id FROM views WHERE name='Head LAT L')),

((SELECT id FROM procedures WHERE name='Skull Series'), (SELECT id FROM views WHERE name='Skull AP')),
((SELECT id FROM procedures WHERE name='Skull Series'), (SELECT id FROM views WHERE name='Skull LAT')),

((SELECT id FROM procedures WHERE name='Sinus Study'), (SELECT id FROM views WHERE name='Sinus Waters')),
((SELECT id FROM procedures WHERE name='Sinus Study'), (SELECT id FROM views WHERE name='Sinus Caldwell')),
((SELECT id FROM procedures WHERE name='Sinus Study'), (SELECT id FROM views WHERE name='Sinus LAT')),

((SELECT id FROM procedures WHERE name='Facial Bones'), (SELECT id FROM views WHERE name='Facial Waters')),
((SELECT id FROM procedures WHERE name='Facial Bones'), (SELECT id FROM views WHERE name='Facial LAT')),

((SELECT id FROM procedures WHERE name='Mandible Series'), (SELECT id FROM views WHERE name='Mandible PA')),
((SELECT id FROM procedures WHERE name='Mandible Series'), (SELECT id FROM views WHERE name='Mandible OBL')),

((SELECT id FROM procedures WHERE name='TMJ Study'), (SELECT id FROM views WHERE name='TMJ LAT')),
((SELECT id FROM procedures WHERE name='TMJ Study'), (SELECT id FROM views WHERE name='TMJ AP Open')),
((SELECT id FROM procedures WHERE name='TMJ Study'), (SELECT id FROM views WHERE name='TMJ AP Closed')),

((SELECT id FROM procedures WHERE name='Orbit Series'), (SELECT id FROM views WHERE name='Orbits PA')),
((SELECT id FROM procedures WHERE name='Orbit Series'), (SELECT id FROM views WHERE name='Orbits LAT')),

((SELECT id FROM procedures WHERE name='Nasal Bones'), (SELECT id FROM views WHERE name='Nasal LAT'));

-- ================= NECK & SPINE =================
INSERT INTO procedure_views (procedure_id, view_id) VALUES
((SELECT id FROM procedures WHERE name='Neck Routine'), (SELECT id FROM views WHERE name='Neck AP')),
((SELECT id FROM procedures WHERE name='Neck Routine'), (SELECT id FROM views WHERE name='Neck LAT')),

((SELECT id FROM procedures WHERE name='Soft Tissue Neck'), (SELECT id FROM views WHERE name='ST Neck AP')),
((SELECT id FROM procedures WHERE name='Soft Tissue Neck'), (SELECT id FROM views WHERE name='ST Neck LAT')),

((SELECT id FROM procedures WHERE name='Cervical Spine Series'), (SELECT id FROM views WHERE name='C-Spine AP')),
((SELECT id FROM procedures WHERE name='Cervical Spine Series'), (SELECT id FROM views WHERE name='C-Spine LAT')),
((SELECT id FROM procedures WHERE name='Cervical Spine Series'), (SELECT id FROM views WHERE name='C-Spine OBL')),
((SELECT id FROM procedures WHERE name='Cervical Spine Series'), (SELECT id FROM views WHERE name='C-Spine OMO')),

((SELECT id FROM procedures WHERE name='Thoracic Spine'), (SELECT id FROM views WHERE name='T-Spine AP')),
((SELECT id FROM procedures WHERE name='Thoracic Spine'), (SELECT id FROM views WHERE name='T-Spine LAT')),

((SELECT id FROM procedures WHERE name='Lumbar Spine'), (SELECT id FROM views WHERE name='L-Spine AP')),
((SELECT id FROM procedures WHERE name='Lumbar Spine'), (SELECT id FROM views WHERE name='L-Spine LAT')),

((SELECT id FROM procedures WHERE name='Sacrum/Coccyx'), (SELECT id FROM views WHERE name='Sacrum AP')),
((SELECT id FROM procedures WHERE name='Sacrum/Coccyx'), (SELECT id FROM views WHERE name='Sac/Cocc LAT')),
((SELECT id FROM procedures WHERE name='Sacrum/Coccyx'), (SELECT id FROM views WHERE name='Coccyx AP')),

((SELECT id FROM procedures WHERE name='Scoliosis Survey'), (SELECT id FROM views WHERE name='Scoliosis PA')),
((SELECT id FROM procedures WHERE name='Scoliosis Survey'), (SELECT id FROM views WHERE name='Scoliosis LAT'));

-- ================= THORAX/CHEST & SHOULDER GIRDLE =================
INSERT INTO procedure_views (procedure_id, view_id) VALUES
((SELECT id FROM procedures WHERE name='Chest Routine'), (SELECT id FROM views WHERE name='Chest PA')),
((SELECT id FROM procedures WHERE name='Chest Routine'), (SELECT id FROM views WHERE name='Chest LAT')),

((SELECT id FROM procedures WHERE name='Rib Series'), (SELECT id FROM views WHERE name='Ribs AP')),
((SELECT id FROM procedures WHERE name='Rib Series'), (SELECT id FROM views WHERE name='Ribs OBL')),

((SELECT id FROM procedures WHERE name='Sternum Series'), (SELECT id FROM views WHERE name='Sternum RAO')),
((SELECT id FROM procedures WHERE name='Sternum Series'), (SELECT id FROM views WHERE name='Sternum LAT')),

((SELECT id FROM procedures WHERE name='Clavicle Series'), (SELECT id FROM views WHERE name='Clavicle AP')),
((SELECT id FROM procedures WHERE name='Clavicle Series'), (SELECT id FROM views WHERE name='Clavicle AX')),

((SELECT id FROM procedures WHERE name='Scapula Series'), (SELECT id FROM views WHERE name='Scapula AP')),
((SELECT id FROM procedures WHERE name='Scapula Series'), (SELECT id FROM views WHERE name='Scapula Y LAT')),

((SELECT id FROM procedures WHERE name='AC Joints'), (SELECT id FROM views WHERE name='ACJ AP')),
((SELECT id FROM procedures WHERE name='AC Joints'), (SELECT id FROM views WHERE name='ACJ AP+W'));

-- ================= ABDOMEN & PELVIS =================
INSERT INTO procedure_views (procedure_id, view_id) VALUES
((SELECT id FROM procedures WHERE name='Abdomen Routine'), (SELECT id FROM views WHERE name='Abdomen Supine')),
((SELECT id FROM procedures WHERE name='Abdomen Routine'), (SELECT id FROM views WHERE name='Abdomen Erect')),

((SELECT id FROM procedures WHERE name='Pelvis AP'), (SELECT id FROM views WHERE name='Pelvis AP')),

((SELECT id FROM procedures WHERE name='Pelvis Inlet/Outlet'), (SELECT id FROM views WHERE name='Pelvis Inlet')),
((SELECT id FROM procedures WHERE name='Pelvis Inlet/Outlet'), (SELECT id FROM views WHERE name='Pelvis Outlet')),

((SELECT id FROM procedures WHERE name='Hip Series'), (SELECT id FROM views WHERE name='Hip AP')),
((SELECT id FROM procedures WHERE name='Hip Series'), (SELECT id FROM views WHERE name='Hip Frog LAT')),

((SELECT id FROM procedures WHERE name='SI Joint Series'), (SELECT id FROM views WHERE name='SIJ OBL'));

-- ================= UPPER EXTREMITY =================
INSERT INTO procedure_views (procedure_id, view_id) VALUES
((SELECT id FROM procedures WHERE name='Shoulder Series'), (SELECT id FROM views WHERE name='Shoulder AP Ext')),
((SELECT id FROM procedures WHERE name='Shoulder Series'), (SELECT id FROM views WHERE name='Shoulder AP Int')),
((SELECT id FROM procedures WHERE name='Shoulder Series'), (SELECT id FROM views WHERE name='Shoulder Axial')),
((SELECT id FROM procedures WHERE name='Shoulder Series'), (SELECT id FROM views WHERE name='Shoulder Y LAT')),

((SELECT id FROM procedures WHERE name='Humerus Routine'), (SELECT id FROM views WHERE name='Humerus AP')),
((SELECT id FROM procedures WHERE name='Humerus Routine'), (SELECT id FROM views WHERE name='Humerus LAT')),

((SELECT id FROM procedures WHERE name='Elbow Series'), (SELECT id FROM views WHERE name='Elbow AP')),
((SELECT id FROM procedures WHERE name='Elbow Series'), (SELECT id FROM views WHERE name='Elbow LAT')),
((SELECT id FROM procedures WHERE name='Elbow Series'), (SELECT id FROM views WHERE name='Elbow OBL')),

((SELECT id FROM procedures WHERE name='Forearm Routine'), (SELECT id FROM views WHERE name='Forearm AP')),
((SELECT id FROM procedures WHERE name='Forearm Routine'), (SELECT id FROM views WHERE name='Forearm LAT')),

((SELECT id FROM procedures WHERE name='Wrist Series'), (SELECT id FROM views WHERE name='Wrist PA')),
((SELECT id FROM procedures WHERE name='Wrist Series'), (SELECT id FROM views WHERE name='Wrist OBL')),
((SELECT id FROM procedures WHERE name='Wrist Series'), (SELECT id FROM views WHERE name='Wrist LAT')),
((SELECT id FROM procedures WHERE name='Wrist Series'), (SELECT id FROM views WHERE name='Wrist Ulnar Dev')),

((SELECT id FROM procedures WHERE name='Hand Series'), (SELECT id FROM views WHERE name='Hand PA')),
((SELECT id FROM procedures WHERE name='Hand Series'), (SELECT id FROM views WHERE name='Hand OBL')),
((SELECT id FROM procedures WHERE name='Hand Series'), (SELECT id FROM views WHERE name='Hand LAT')),

((SELECT id FROM procedures WHERE name='Finger Series'), (SELECT id FROM views WHERE name='Finger PA')),
((SELECT id FROM procedures WHERE name='Finger Series'), (SELECT id FROM views WHERE name='Finger OBL')),
((SELECT id FROM procedures WHERE name='Finger Series'), (SELECT id FROM views WHERE name='Finger LAT')),

((SELECT id FROM procedures WHERE name='Thumb Series'), (SELECT id FROM views WHERE name='Thumb AP')),
((SELECT id FROM procedures WHERE name='Thumb Series'), (SELECT id FROM views WHERE name='Thumb LAT'));

-- ================= LOWER EXTREMITY =================
INSERT INTO procedure_views (procedure_id, view_id) VALUES
((SELECT id FROM procedures WHERE name='Femur Routine'), (SELECT id FROM views WHERE name='Femur AP')),
((SELECT id FROM procedures WHERE name='Femur Routine'), (SELECT id FROM views WHERE name='Femur LAT')),

((SELECT id FROM procedures WHERE name='Knee Series'), (SELECT id FROM views WHERE name='Knee AP')),
((SELECT id FROM procedures WHERE name='Knee Series'), (SELECT id FROM views WHERE name='Knee LAT')),
((SELECT id FROM procedures WHERE name='Knee Series'), (SELECT id FROM views WHERE name='Knee OBL')),

((SELECT id FROM procedures WHERE name='Patella Study'), (SELECT id FROM views WHERE name='Patella PA')),
((SELECT id FROM procedures WHERE name='Patella Study'), (SELECT id FROM views WHERE name='Patella Skyline')),

((SELECT id FROM procedures WHERE name='Tib-Fib Routine'), (SELECT id FROM views WHERE name='Tib-Fib AP')),
((SELECT id FROM procedures WHERE name='Tib-Fib Routine'), (SELECT id FROM views WHERE name='Tib-Fib LAT')),

((SELECT id FROM procedures WHERE name='Ankle Series'), (SELECT id FROM views WHERE name='Ankle AP')),
((SELECT id FROM procedures WHERE name='Ankle Series'), (SELECT id FROM views WHERE name='Ankle Mortise')),
((SELECT id FROM procedures WHERE name='Ankle Series'), (SELECT id FROM views WHERE name='Ankle LAT')),

((SELECT id FROM procedures WHERE name='Foot Series'), (SELECT id FROM views WHERE name='Foot AP')),
((SELECT id FROM procedures WHERE name='Foot Series'), (SELECT id FROM views WHERE name='Foot OBL')),
((SELECT id FROM procedures WHERE name='Foot Series'), (SELECT id FROM views WHERE name='Foot LAT')),

((SELECT id FROM procedures WHERE name='Toe Series'), (SELECT id FROM views WHERE name='Toes AP')),
((SELECT id FROM procedures WHERE name='Toe Series'), (SELECT id FROM views WHERE name='Toes OBL')),
((SELECT id FROM procedures WHERE name='Toe Series'), (SELECT id FROM views WHERE name='Toes LAT')),

((SELECT id FROM procedures WHERE name='Calcaneus Study'), (SELECT id FROM views WHERE name='Calcaneus AX')),
((SELECT id FROM procedures WHERE name='Calcaneus Study'), (SELECT id FROM views WHERE name='Calcaneus LAT'));


-- ================= Environment Settings =================
INSERT INTO environment_settings
  (study_level, lookup_table,
   worklist_clear_period_days, worklist_refresh_period_seconds, delete_log_period_days,
   auto_refresh_worklist, auto_clear_disk_space, enable_mpps, continue_on_echo_fail)
VALUES
  ('Multi-Series Study', 'VOI LUT',
   14, 30, 60,
   1, 1, 1, 1);

-- ================= Status Tracking Triggers =================
-- These triggers automatically update status_updated_at when status changes

DELIMITER $$

CREATE TRIGGER trg_patients_status_update
BEFORE UPDATE ON patients
FOR EACH ROW
BEGIN
    IF NEW.status != OLD.status THEN
        SET NEW.status_updated_at = CURRENT_TIMESTAMP;
    END IF;
END$$

CREATE TRIGGER trg_studies_status_update
BEFORE UPDATE ON studies
FOR EACH ROW
BEGIN
    IF NEW.status != OLD.status THEN
        SET NEW.status_updated_at = CURRENT_TIMESTAMP;
    END IF;
END$$

CREATE TRIGGER trg_series_status_update
BEFORE UPDATE ON series
FOR EACH ROW
BEGIN
    IF NEW.status != OLD.status THEN
        SET NEW.status_updated_at = CURRENT_TIMESTAMP;
    END IF;
END$$

CREATE TRIGGER trg_images_status_update
BEFORE UPDATE ON images
FOR EACH ROW
BEGIN
    IF NEW.status != OLD.status THEN
        SET NEW.status_updated_at = CURRENT_TIMESTAMP;
    END IF;
END$$

DELIMITER ;

