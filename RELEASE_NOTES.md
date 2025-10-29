# Release Notes

## [Unreleased] - 2025-10-29

### Added - Admission ID Support

#### Database Schema
- **[FEATURE]** Added `admission_id` column to `studies` table (#29)
  - Column type: VARCHAR(64), nullable
  - DICOM tag mapping: (0038,0010) - Admission ID  
  - Added non-unique index `idx_admission_id` for query performance
  - Backward compatible - no breaking changes to existing queries

#### Migration & Backfill
- **[MIGRATION]** Added `migration_add_admission_id.sql` script
  - Idempotent migration script for existing databases
  - Safely adds `admission_id` column and index
  - Can be run multiple times without errors

- **[MIGRATION]** Added `backfill_admission_id.sql` script
  - Best-effort backfill of admission_id from worklist data
  - Only updates NULL values, preserving existing data
  - Transaction-safe with rollback support
  - Provides before/after statistics

#### Documentation
- **[DOCS]** Added `docs/admission_id_schema.md`
  - Comprehensive documentation of Admission ID feature
  - DICOM tag mapping details
  - Data flow explanation
  - Integration examples for C++ code
  - Usage examples and testing guidance

- **[DOCS]** Added `Core/Script/README.md`
  - Migration workflow documentation
  - Script usage instructions
  - Troubleshooting guide

### Technical Details

**DICOM Compliance**: 
- Tag (0038,0010) from DICOM PS3.3 Study-Patient module
- Already supported in worklist module (WorklistEnum, MappingProfile)
- Now persistable in studies table for long-term storage

**Compatibility**:
- ✅ Fully backward compatible
- ✅ No breaking changes to existing code
- ✅ Nullable column with DEFAULT NULL
- ✅ Existing queries unaffected
- ✅ Safe for production deployment

**Performance**:
- Indexed column for efficient queries by admission_id
- No impact on existing query performance

### Migration Instructions

For existing deployments:

1. **Backup database** before migration
2. Run `migration_add_admission_id.sql` to add schema changes
3. Optionally run `backfill_admission_id.sql` to populate from worklist data
4. Update application code to populate/query admission_id (future work)

See `Core/Script/README.md` for detailed migration instructions.

### Future Work

The following items are NOT included in this release but are prepared for:

- [ ] C++ entity/model updates for Study to include admissionId field
- [ ] Repository layer methods to read/write admission_id
- [ ] DICOM ingestion code to extract (0038,0010) tag
- [ ] UI integration to display/filter by admission_id
- [ ] Unit and integration tests
- [ ] API/DTO updates (if REST API is added in future)

This release provides the foundational database schema and documentation. Application layer integration should follow in subsequent releases.

### References

- GitHub Issue: https://github.com/altay-brusan/etrek/issues/29
- DICOM Standard: [PS3.3 Study-Patient Module](http://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.7.2.2.html)
- Pull Request: TBD

---

## Version History

### Legend
- **[FEATURE]**: New functionality
- **[BUGFIX]**: Bug fix
- **[BREAKING]**: Breaking change (requires code updates)
- **[MIGRATION]**: Database migration required
- **[DOCS]**: Documentation update
- **[PERF]**: Performance improvement
- **[SECURITY]**: Security fix

---

*Note: This is the first entry in the release notes. Future releases will be added above this line.*
