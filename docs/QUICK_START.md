# Quick Start Guide: Completing admission_id Implementation

This guide helps the next developer quickly complete the admission_id implementation.

## What's Already Done ✅

The database layer is **complete and ready to deploy**:
- ✅ Schema updated with `admission_id` column (VARCHAR 64, nullable, indexed)
- ✅ Migration script created (idempotent, safe to run multiple times)
- ✅ Validation script created
- ✅ DICOM tag (0038,0010) already defined
- ✅ Worklist mapping already exists
- ✅ Comprehensive documentation

## What You Need to Do

### Step 1: Deploy Database Changes (5 minutes)

```bash
# On test database:
mysql -u root -p etrekdb < Core/Script/add_admission_id_to_studies.sql
mysql -u root -p etrekdb < Core/Script/validate_admission_id.sql

# Verify the output shows:
# - admission_id column exists
# - idx_admission_id index exists
# - AdmissionID DICOM tag exists
```

### Step 2: Find Study Creation Code (15-30 minutes)

The codebase doesn't have explicit Study entity classes. Look for:

```bash
# Search for SQL INSERT/UPDATE on studies table:
grep -r "INSERT.*studies" --include="*.cpp" --include="*.h"
grep -r "UPDATE.*studies" --include="*.cpp" --include="*.h"

# Or search for DICOM storage handlers:
grep -r "C-STORE" --include="*.cpp" --include="*.h"
grep -r "DcmDataset" --include="*.cpp" --include="*.h"
```

### Step 3: Add admission_id to Study Creation (30 minutes)

Once you find the study creation code, add admission_id extraction:

```cpp
// Extract AdmissionID from DICOM dataset
OFString admissionIdOF;
QString admissionId;
if (dataset->findAndGetOFString(DCM_AdmissionID, admissionIdOF).good()) {
    admissionId = QString::fromStdString(admissionIdOF.c_str()).trimmed();
}

// Include in INSERT query
QString insertQuery = R"(
    INSERT INTO studies (
        study_instance_uid,
        accession_number,
        admission_id,        -- Add this
        study_date,
        study_time
        -- ... other fields ...
    ) VALUES (?, ?, ?, ?, ?)  -- Add ? for admission_id
)";

// Bind the value
query.prepare(insertQuery);
query.bindValue(0, studyInstanceUid);
query.bindValue(1, accessionNumber);
query.bindValue(2, admissionId);       // Add this
query.bindValue(3, studyDate);
query.bindValue(4, studyTime);
// ... bind other values ...
```

### Step 4: Add admission_id to Study Queries (15 minutes)

Update any SELECT queries:

```cpp
QString selectQuery = R"(
    SELECT 
        id,
        study_instance_uid,
        accession_number,
        admission_id,        -- Add this
        study_date,
        study_time
        -- ... other fields ...
    FROM studies
    WHERE ...
)";
```

### Step 5: Add Filtering Support (15 minutes)

If there's a study query API, add admission_id filtering:

```cpp
// In repository or query service:
Result<QList<Study>> getStudies(const QString& admissionId = QString()) {
    QString query = "SELECT * FROM studies WHERE 1=1";
    
    if (!admissionId.isEmpty()) {
        query += " AND admission_id = ?";
        // bind admissionId to query
    }
    
    // Execute query...
}
```

### Step 6: Update DTOs (if they exist) (10 minutes)

If there are Study DTOs or entity classes:

```cpp
struct Study {
    int id;
    QString studyInstanceUid;
    QString accessionNumber;
    QString admissionId;      // Add this
    QDate studyDate;
    QTime studyTime;
    // ... other fields ...
};
```

### Step 7: Build and Test (30 minutes)

```bash
# Configure
cmake --preset Qt-Debug

# Build
cmake --build out/build/debug

# Run existing tests
cd out/build/debug
ctest

# Or run specific tests
./tst_WorklistRepository
```

### Step 8: Add Observability (15 minutes)

Add logging and metrics:

```cpp
// Log when admission_id is ingested
if (!admissionId.isEmpty()) {
    logger->LogInfo(
        QString("Study %1 ingested with admission_id: %2")
        .arg(studyInstanceUid)
        .arg(admissionId)
    );
    
    // Increment counter
    metrics->IncrementCounter("admission_id_ingestion_count");
}

// Periodically calculate population rate
void calculatePopulationRate() {
    auto result = db.exec("SELECT COUNT(*) FROM studies WHERE admission_id IS NOT NULL");
    int withAdmissionId = result.value(0).toInt();
    
    result = db.exec("SELECT COUNT(*) FROM studies");
    int total = result.value(0).toInt();
    
    double rate = total > 0 ? (withAdmissionId * 100.0 / total) : 0.0;
    metrics->SetGauge("admission_id_population_rate", rate);
}
```

## Testing Checklist

- [ ] Build succeeds without errors
- [ ] Existing tests pass
- [ ] Can insert study with admission_id
- [ ] Can insert study without admission_id (NULL)
- [ ] Can query studies by admission_id
- [ ] Can retrieve studies with admission_id included
- [ ] DICOM study with (0038,0010) populates admission_id
- [ ] DICOM study without (0038,0010) leaves admission_id NULL
- [ ] Index improves query performance (check EXPLAIN)

## Test Data

Use this DICOM test file or create one with dcmtk:

```bash
# Create test DICOM with AdmissionID
dcmodify --insert "(0038,0010)=ADM-2024-12345" test_study.dcm

# Send to your C-STORE SCP
storescu localhost 11112 test_study.dcm

# Verify in database
mysql> SELECT study_instance_uid, admission_id FROM studies WHERE admission_id IS NOT NULL;
```

## Common Issues and Solutions

### Issue: Can't find study creation code
**Solution**: Check if studies are created through worklist conversion. Look at `mwl_task_mapping` table creation.

### Issue: Build fails on Linux
**Solution**: This is a Windows/MSVC project. You need Windows + Qt 6.5.3 + MSVC 2019.

### Issue: Migration fails
**Solution**: The migration script is idempotent. Check MySQL version supports `IF NOT EXISTS` (MySQL 5.7+).

### Issue: Tests fail
**Solution**: Make sure you're only running tests related to your changes. Don't try to fix unrelated failures.

## Time Estimate

- Database deployment: **5 minutes**
- Code changes: **1-2 hours**
- Testing: **30-60 minutes**
- Documentation update: **15 minutes**

**Total: 2-3 hours** for an experienced developer familiar with the codebase.

## Documentation References

- **Full Implementation Guide**: `docs/ADMISSION_ID_IMPLEMENTATION.md`
- **Acceptance Criteria**: `docs/ACCEPTANCE_CRITERIA_STATUS.md`
- **Migration Guide**: `Core/Script/README_ADMISSION_ID.md`
- **DICOM Standard**: https://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.7.2.2.html

## Questions?

Contact the database team if you have questions about the schema changes, or refer to the comprehensive documentation in the `docs/` directory.

Good luck! 🚀
