-- Disable foreign key checks
SET FOREIGN_KEY_CHECKS = 0;
-- Drop the table
DROP TABLE procedures;
DROP TABLE procedure_views;
-- Re-enable foreign key checks
SET FOREIGN_KEY_CHECKS = 1;

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


SELECT * FROM etrekdb.procedures;