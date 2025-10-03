-- Disable foreign key checks
SET FOREIGN_KEY_CHECKS = 0;
-- Drop the table
DROP TABLE views;
DROP TABLE view_techniques;
-- Re-enable foreign key checks
SET FOREIGN_KEY_CHECKS = 1;

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

SELECT * FROM views;
SELECT * FROM  view_techniques