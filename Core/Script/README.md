# Database Migration Scripts

This directory contains SQL scripts for database schema management and data migration.

## Scripts

### setup_database.sql

Complete database schema definition for Etrek medical imaging application. This script creates all tables, indexes, and initial data from scratch.

**Usage**: For new database installations only.

```bash
mysql -u username -p -e "CREATE DATABASE etrek_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"
mysql -u username -p etrek_db < setup_database.sql
```

### migration_add_admission_id.sql

**Purpose**: Adds `admission_id` column to the `studies` table for existing databases.

**Details**:
- Adds `admission_id VARCHAR(64) DEFAULT NULL` column
- Creates index `idx_admission_id` on the new column
- Idempotent - safe to run multiple times
- Will skip changes if column/index already exists

**Usage**:
```bash
mysql -u username -p etrek_db < migration_add_admission_id.sql
```

**Related**: GitHub Issue #29, see docs/admission_id_schema.md for details

### backfill_admission_id.sql

**Purpose**: Populates `admission_id` in studies table from existing worklist data.

**Details**:
- Best-effort backfill from `mwl_attributes` table
- Only updates studies where `admission_id` IS NULL
- Uses transaction for data safety
- Idempotent - safe to run multiple times
- Provides before/after statistics

**Prerequisites**:
1. `migration_add_admission_id.sql` must be run first
2. Worklist data must exist in `mwl_entries` and `mwl_attributes` tables
3. Studies must have matching `study_instance_uid` in worklist

**Usage**:
```bash
mysql -u username -p etrek_db < backfill_admission_id.sql
```

**Output**: The script will display:
- Number of studies before/after
- Number of mappings found
- Number of studies updated

**Example Output**:
```
+---------------+-----------------------------+-------------------------+
| total_studies | studies_without_admission_id | studies_with_admission_id |
+---------------+-----------------------------+-------------------------+
|           100 |                          85 |                      15 |
+---------------+-----------------------------+-------------------------+

+-----------------+------------------------------------------------------------------+
| mappings_found  | Info                                                             |
+-----------------+------------------------------------------------------------------+
|              42 | These studies will be updated with admission_id from worklist... |
+-----------------+------------------------------------------------------------------+

+-----------------+-------------------------------------------------------------+
| studies_updated | Info                                                        |
+-----------------+-------------------------------------------------------------+
|              42 | studies were updated with admission_id from worklist data   |
+-----------------+-------------------------------------------------------------+
```

## Migration Workflow

For existing databases upgrading to support Admission ID:

1. **Backup your database**:
   ```bash
   mysqldump -u username -p etrek_db > etrek_db_backup_$(date +%Y%m%d).sql
   ```

2. **Run the migration**:
   ```bash
   mysql -u username -p etrek_db < migration_add_admission_id.sql
   ```

3. **Verify the migration**:
   ```bash
   mysql -u username -p etrek_db -e "DESCRIBE studies;"
   mysql -u username -p etrek_db -e "SHOW INDEX FROM studies WHERE Key_name='idx_admission_id';"
   ```

4. **Optionally backfill existing data**:
   ```bash
   mysql -u username -p etrek_db < backfill_admission_id.sql
   ```

## Migration Safety

All migration scripts are designed to be:
- **Idempotent**: Can be run multiple times safely
- **Non-destructive**: Never delete or modify existing data (except backfill which only updates NULL values)
- **Transaction-safe**: Use transactions where appropriate
- **Informative**: Provide clear output about what was changed

## Troubleshooting

### Migration fails with "Column already exists"

This is expected if the migration was already run. The scripts check for existence before making changes.

### Backfill doesn't find any mappings

This means:
- No worklist entries exist for your studies, OR
- Worklist entries don't have AdmissionID tag values, OR
- Studies already have admission_id populated

Check with:
```sql
-- Check if worklist has admission IDs
SELECT COUNT(*) FROM mwl_attributes ma
JOIN dicom_tags dt ON ma.dicom_tag_id = dt.id
WHERE dt.name = 'AdmissionID' AND ma.tag_value IS NOT NULL;

-- Check current state of studies
SELECT 
    COUNT(*) as total,
    SUM(CASE WHEN admission_id IS NULL THEN 1 ELSE 0 END) as without_id,
    SUM(CASE WHEN admission_id IS NOT NULL THEN 1 ELSE 0 END) as with_id
FROM studies;
```

### Permission errors

Ensure your MySQL user has the following privileges:
```sql
GRANT SELECT, INSERT, UPDATE, CREATE, ALTER, INDEX ON etrek_db.* TO 'username'@'localhost';
FLUSH PRIVILEGES;
```

## See Also

- `docs/admission_id_schema.md` - Detailed documentation about Admission ID support
- `CLAUDE.md` - Build and development instructions
- GitHub Issue #29 - Original feature request
