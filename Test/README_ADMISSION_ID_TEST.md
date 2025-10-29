# Admission ID Schema Tests

This test suite validates the database schema changes for Admission ID support (GitHub Issue #29).

## Test: tst_AdmissionIdSchema

### Purpose

Verifies that the `admission_id` column has been correctly added to the `studies` table with:
- Correct data type (VARCHAR(64))
- Nullable constraint (DEFAULT NULL)
- Index `idx_admission_id` for query performance
- Full CRUD functionality

### Test Coverage

1. **Schema Validation**
   - ✓ Column `admission_id` exists in `studies` table
   - ✓ Column has type VARCHAR(64)
   - ✓ Column is nullable (allows NULL values)
   - ✓ Index `idx_admission_id` exists

2. **Functional Tests**
   - ✓ Insert study with admission_id
   - ✓ Insert study without admission_id (NULL value)
   - ✓ Update admission_id on existing study
   - ✓ Query studies by admission_id
   - ✓ Verify index usage in queries

### Prerequisites

1. **Test Database**: Set up a MySQL test database
2. **Environment Variables** (optional):
   ```bash
   export ETREK_TEST_DB_NAME="etrek_test"      # Default if not set
   export ETREK_TEST_DB_HOST="localhost"        # Default if not set  
   export ETREK_TEST_DB_USER="root"             # Default if not set
   export ETREK_TEST_DB_PASS="your_password"    # Required
   ```

3. **Schema**: Ensure the test database has been initialized with the updated schema:
   ```bash
   mysql -u root -p etrek_test < Core/Script/setup_database.sql
   ```
   OR run the migration on an existing test database:
   ```bash
   mysql -u root -p etrek_test < Core/Script/migration_add_admission_id.sql
   ```

### Running the Test

#### Using CMake/CTest

```bash
# Configure
cmake --preset Qt-Debug

# Build tests
cmake --build out/build/debug --target tst_AdmissionIdSchema

# Run the test
cd out/build/debug/Test
./tst_AdmissionIdSchema

# Or use ctest
ctest -R tst_AdmissionIdSchema --verbose
```

#### Direct Execution

```bash
# Navigate to test binary directory
cd out/build/debug/Test

# Set environment variables
export ETREK_TEST_DB_PASS="your_password"

# Run the test
./tst_AdmissionIdSchema
```

### Expected Output

If all tests pass:

```
********* Start testing of AdmissionIdSchemaTest *********
Config: Using QtTest library 6.5.3, Qt 6.5.3
PASS   : AdmissionIdSchemaTest::initTestCase()
PASS   : AdmissionIdSchemaTest::testAdmissionIdColumnExists()
PASS   : AdmissionIdSchemaTest::testAdmissionIdColumnType()
PASS   : AdmissionIdSchemaTest::testAdmissionIdColumnIsNullable()
PASS   : AdmissionIdSchemaTest::testAdmissionIdIndexExists()
PASS   : AdmissionIdSchemaTest::testInsertStudyWithAdmissionId()
PASS   : AdmissionIdSchemaTest::testInsertStudyWithoutAdmissionId()
PASS   : AdmissionIdSchemaTest::testUpdateAdmissionId()
PASS   : AdmissionIdSchemaTest::testQueryByAdmissionId()
PASS   : AdmissionIdSchemaTest::testQueryUsesIndex()
PASS   : AdmissionIdSchemaTest::cleanupTestCase()
Totals: 11 passed, 0 failed, 0 skipped, 0 blacklisted
********* Finished testing of AdmissionIdSchemaTest *********
```

### Troubleshooting

#### Test fails: "admission_id column should exist in studies table"

**Cause**: The migration has not been run on the test database.

**Solution**: Run the migration script:
```bash
mysql -u root -p etrek_test < Core/Script/migration_add_admission_id.sql
```

#### Test fails: "Failed to connect to test database"

**Cause**: Database connection configuration is incorrect or database doesn't exist.

**Solution**: 
1. Verify database exists:
   ```bash
   mysql -u root -p -e "SHOW DATABASES LIKE 'etrek_test';"
   ```
2. Create if missing:
   ```bash
   mysql -u root -p -e "CREATE DATABASE etrek_test;"
   ```
3. Check environment variables are set correctly

#### Test fails: "Failed to insert study with admission_id"

**Cause**: Other constraints on studies table might be violated, or column doesn't exist.

**Solution**: 
1. Check current schema:
   ```bash
   mysql -u root -p etrek_test -e "DESCRIBE studies;"
   ```
2. Ensure all required columns exist and migration is complete

### Continuous Integration

For CI environments, set up the test database before running tests:

```bash
# Create test database
mysql -u root -p$MYSQL_ROOT_PASSWORD -e "CREATE DATABASE IF NOT EXISTS etrek_test;"

# Initialize schema
mysql -u root -p$MYSQL_ROOT_PASSWORD etrek_test < Core/Script/setup_database.sql

# Run tests
ctest --output-on-failure
```

### Manual Testing

To manually verify the schema without running automated tests:

```sql
-- Check column exists
SELECT COLUMN_NAME, COLUMN_TYPE, IS_NULLABLE, COLUMN_DEFAULT, COLUMN_COMMENT
FROM INFORMATION_SCHEMA.COLUMNS
WHERE TABLE_SCHEMA = 'etrek_test'
  AND TABLE_NAME = 'studies'
  AND COLUMN_NAME = 'admission_id';

-- Check index exists
SHOW INDEX FROM studies WHERE Key_name = 'idx_admission_id';

-- Test insert with admission_id
INSERT INTO studies (study_instance_uid, study_id, admission_id)
VALUES ('1.2.3.test', 'ST_TEST', 'ADM_TEST_123');

-- Test query by admission_id
SELECT * FROM studies WHERE admission_id = 'ADM_TEST_123';

-- Test EXPLAIN to verify index usage
EXPLAIN SELECT * FROM studies WHERE admission_id = 'ADM_TEST_123';

-- Cleanup
DELETE FROM studies WHERE study_instance_uid LIKE '1.2.3.test%';
```

## See Also

- `docs/admission_id_schema.md` - Detailed schema documentation
- `Core/Script/README.md` - Migration script documentation
- `RELEASE_NOTES.md` - Feature release notes
- GitHub Issue #29 - Original feature request
