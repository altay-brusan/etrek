# DICOM Patient Module - Database Mapping

## Overview

This document describes the mapping between DICOM Patient Module attributes and the `patients` table in the database, as well as the relationship to the `studies` table.

## Database Schema

### Patients Table

The `patients` table stores DICOM Patient Module data with the following structure:

| Column Name | DICOM Tag | DICOM Attribute Name | Data Type | Notes |
|-------------|-----------|---------------------|-----------|-------|
| `id` | - | Primary Key | INT | Auto-increment surrogate key |
| `patient_name` | (0010,0010) | Patient's Name | VARCHAR(255) | Full name of the patient |
| `patient_id` | (0010,0020) | Patient ID | VARCHAR(64) | Primary identifier assigned by issuer |
| `issuer_of_patient_id` | (0010,0021) | Issuer of Patient ID | VARCHAR(128) | Organization that issued the Patient ID |
| `type_of_patient_id` | (0010,0022) | Type of Patient ID | VARCHAR(64) | TEXT, RFID, or BARCODE |
| `issuer_of_patient_id_qualifiers` | (0010,0024) | Issuer of Patient ID Qualifiers Sequence | JSON | Sequence stored as JSON |
| `other_patient_id` | (0010,1000) | Other Patient IDs | JSON | Array of alternative IDs stored as JSON |
| `patient_sex` | (0010,0040) | Patient's Sex | VARCHAR(16) | M, F, O, or U |
| `patient_birth_date` | (0010,0030) | Patient's Birth Date | DATE | DICOM DA format parsed to DATE |
| `patient_comments` | (0010,4000) | Patient Comments | TEXT | Free-text comments |
| `patient_allergies` | (0010,2110) | Allergies | TEXT | Patient allergies |
| `requesting_physician` | (0032,1032) | Requesting Physician | VARCHAR(255) | Requesting physician name |
| `patient_address` | (0010,1040) | Patient's Address | TEXT | Mailing address |
| `create_date` | - | Record Created | TIMESTAMP | Auto-populated on insert |
| `update_date` | - | Record Updated | TIMESTAMP | Auto-updated on modification |

### Uniqueness Constraint

A composite unique index ensures patient identity uniqueness:

```sql
UNIQUE KEY uq_patient_identity (patient_id, issuer_of_patient_id)
```

This allows:
- Different issuers to have the same `patient_id`
- Each issuer to have only one patient with a given `patient_id`

## Study-Patient Relationship

### Foreign Key

The `studies` table has a foreign key to the `patients` table:

```sql
ALTER TABLE studies 
ADD COLUMN patient_ref_id INT DEFAULT NULL,
ADD FOREIGN KEY (patient_ref_id) REFERENCES patients(id) ON DELETE RESTRICT;
```

**Relationship**: One patient → Many studies

**Delete Behavior**: `ON DELETE RESTRICT` prevents deletion of a patient that has associated studies. This protects study integrity.

## Ingestion Mapping

During DICOM object ingestion, the following mapping should be applied:

### Patient-Level Attributes

1. **Patient Name** (0010,0010) → `patient_name`
2. **Patient ID** (0010,0020) → `patient_id`
3. **Issuer of Patient ID** (0010,0021) → `issuer_of_patient_id`
4. **Type of Patient ID** (0010,0022) → `type_of_patient_id`
5. **Issuer of Patient ID Qualifiers Sequence** (0010,0024) → `issuer_of_patient_id_qualifiers` (serialize to JSON)
6. **Other Patient IDs** (0010,1000) → `other_patient_id` (serialize to JSON array)
7. **Patient's Sex** (0010,0040) → `patient_sex`
8. **Patient's Birth Date** (0010,0030) → `patient_birth_date` (parse DA format to DATE)
9. **Patient Comments** (0010,4000) → `patient_comments`
10. **Allergies** (0010,2110) → `patient_allergies`
11. **Requesting Physician** (0032,1032) → `requesting_physician`
12. **Patient's Address** (0010,1040) → `patient_address`

### Study-Level Linking

When creating or updating a study:
1. Extract patient identification from DICOM (patient_id + issuer_of_patient_id)
2. Look up or create patient record in `patients` table
3. Set `studies.patient_ref_id` to the patient's `id`

## Notes

### JSON Serialization

Two fields use JSON for storing DICOM sequences:

- **issuer_of_patient_id_qualifiers**: Stores the Issuer of Patient ID Qualifiers Sequence (0010,0024)
- **other_patient_id**: Stores array of Other Patient IDs (0010,1000)

Example JSON for `other_patient_id`:
```json
["ALT-12345", "PREV-98765"]
```

### Retired Attributes

The attribute "Other Patient ID" (0010,1000) is retired in DICOM but included for backward compatibility. Modern systems should use "Other Patient IDs Sequence" (0010,1002).

### Soft Delete

The current implementation uses `ON DELETE RESTRICT` for referential integrity. If soft-delete functionality is needed later, consider adding `is_deleted` flag to the `patients` table similar to the `users` table pattern.

## Schema Diagram

```
┌─────────────┐
│  patients   │
├─────────────┤
│ id (PK)     │◄────┐
│ patient_id  │     │
│ issuer_...  │     │
│ ...         │     │
└─────────────┘     │
                    │ FK: patient_ref_id
                    │ ON DELETE RESTRICT
┌─────────────┐     │
│  studies    │     │
├─────────────┤     │
│ id (PK)     │     │
│ patient_... ├─────┘
│ study_...   │
│ ...         │
└─────────────┘
      │
      │ FK: study_id
      ▼
┌─────────────┐
│   series    │
└─────────────┘
```

## Migration Notes

This is an **additive schema change**:
- New `patients` table
- New `patient_ref_id` column in `studies` (nullable to support existing data)
- New FK constraint

Existing studies without patient references will have `patient_ref_id = NULL` until backfilled during DICOM re-ingestion or migration script.
