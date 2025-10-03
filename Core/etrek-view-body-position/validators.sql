-- A) Views with ZERO bound techniques
SELECT
  v.id, v.name, v.projection_profile, b.name AS body_part
FROM views v
JOIN body_parts b ON b.id = v.body_part_id
WHERE NOT EXISTS (
  SELECT 1 FROM view_techniques vt WHERE vt.view_id = v.id
)
ORDER BY b.name, v.name;


-- B) Techniques that DIDN'T bind (matching body_part + profile exists,
--    but this technique still isn't attached to any view)
SELECT
  tp.id AS technique_id,
  b.name AS body_part,
  tp.size,
  tp.technique_profile
FROM technique_parameters tp
JOIN body_parts b ON b.id = tp.body_part_id
JOIN views v
  ON v.body_part_id = tp.body_part_id
 AND v.projection_profile = tp.technique_profile
LEFT JOIN view_techniques vt
  ON vt.technique_parameter_id = tp.id
WHERE vt.technique_parameter_id IS NULL
ORDER BY b.name, tp.technique_profile, tp.size;

-- C) Views whose projection_profile has NO matching technique_parameters
--    (i.e., you made a view but have no techniques for that profile/body part)
SELECT
  v.id, v.name, v.projection_profile, b.name AS body_part
FROM views v
JOIN body_parts b ON b.id = v.body_part_id
LEFT JOIN technique_parameters tp
  ON tp.body_part_id = v.body_part_id
 AND tp.technique_profile = v.projection_profile
WHERE tp.id IS NULL
ORDER BY b.name, v.name;


-- D) Summary: how many techniques are bound per view
SELECT
  v.id, v.name, b.name AS body_part, v.projection_profile,
  COUNT(vt.technique_parameter_id) AS bound_techniques
FROM views v
JOIN body_parts b ON b.id = v.body_part_id
LEFT JOIN view_techniques vt ON vt.view_id = v.id
GROUP BY v.id, v.name, b.name, v.projection_profile
ORDER BY b.name, v.name;

