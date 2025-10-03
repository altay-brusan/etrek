-- Disable foreign key checks
SET FOREIGN_KEY_CHECKS = 0;
-- Drop the table
DROP TABLE positioners;
-- Re-enable foreign key checks
SET FOREIGN_KEY_CHECKS = 1;

-- details of supported positions and movements
CREATE TABLE positioners (
    id INT AUTO_INCREMENT PRIMARY KEY,
    position_name VARCHAR(100) NOT NULL UNIQUE, -- unique position name
    description VARCHAR(255) NULL,
    create_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);


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

SELECT * FROM positioners