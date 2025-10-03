
-- Disable foreign key checks
SET FOREIGN_KEY_CHECKS = 0;
-- Drop the table
DROP TABLE body_parts;
-- Re-enable foreign key checks
SET FOREIGN_KEY_CHECKS = 1;

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

select * from body_parts