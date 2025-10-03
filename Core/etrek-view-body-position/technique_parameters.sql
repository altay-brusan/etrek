-- Disable foreign key checks
SET FOREIGN_KEY_CHECKS = 0;
-- Drop the table
DROP TABLE technique_parameters;
-- Re-enable foreign key checks
SET FOREIGN_KEY_CHECKS = 1;


CREATE TABLE technique_parameters (
    id INT AUTO_INCREMENT PRIMARY KEY,
    body_part_id INT NOT NULL,
    size ENUM('Fat','Medium','Thin','Paediatric') NOT NULL,
    technique_profile ENUM('AP|PA','LAT','OBL','AXIAL') DEFAULT 'AP|PA',
    kvp INT,
    ma INT,
    ms DECIMAL(6,2),                -- or INT if you prefer
    fkvp INT,
    fma DECIMAL(6,2),
    focal_spot DECIMAL(3,1) DEFAULT NULL,
    sid_min INT,
    sid_max INT,
    grid_type ENUM('Parallel','Focused','Crossed','Moving','Virtual') DEFAULT NULL,
    grid_ratio VARCHAR(50) DEFAULT NULL,
    grid_speed VARCHAR(50) DEFAULT NULL,
    exposure_style ENUM('Mas Mode','Time Mode','AEC Mode','Manual') DEFAULT NULL,
    aec_field VARCHAR(10),
    aec_density INT,
    CONSTRAINT fk_tp_bodypart FOREIGN KEY (body_part_id) REFERENCES body_parts(id),
    CONSTRAINT uk_tp_unique UNIQUE (body_part_id, size, technique_profile),
    CONSTRAINT chk_sid_bounds CHECK (sid_min IS NULL OR sid_max IS NULL OR sid_min <= sid_max),
    CONSTRAINT chk_aec_when_aecmode CHECK (
      (exposure_style <> 'AEC Mode' AND aec_field IS NULL AND aec_density IS NULL)
      OR (exposure_style = 'AEC Mode')
    )
);
CREATE INDEX ix_tp_bodypart ON technique_parameters (body_part_id, technique_profile, size);


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



-- select * from technique_parameters