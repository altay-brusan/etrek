# E-TREK Database Schema

## Overview

E-TREK uses MySQL as the relational database for storing:
- **Patient demographics** and medical imaging data (DICOM hierarchy)
- **Device configurations** (generators, X-ray tubes, detectors, positioners)
- **DICOM Modality Worklist (MWL)** entries and DICOM tag definitions
- **Imaging procedures** and technique parameters
- **User accounts**, roles, and authentication
- **System settings** and institutional metadata
- **PACS/MPPS connections**

## Entity Relationship Summary

```
┌─────────────────────────────────────────────────────────────────────┐
│                     AUTHENTICATION & AUTHORIZATION                   │
└─────────────────────────────────────────────────────────────────────┘
users ──< user_roles >── roles

┌─────────────────────────────────────────────────────────────────────┐
│                           DICOM HIERARCHY                            │
└─────────────────────────────────────────────────────────────────────┘
patients ──< studies ──< series ──< images ──< sop_commons
                      └──< acquisitions

┌─────────────────────────────────────────────────────────────────────┐
│                    DEVICE & HARDWARE CONFIGURATION                   │
└─────────────────────────────────────────────────────────────────────┘
institutions ──< general_equipments ──< detector_setup ── detectors
                                    └──< xray_tube_setup ── xray_tubes
                                                           └── generators

┌─────────────────────────────────────────────────────────────────────┐
│                 SCAN PROTOCOLS & IMAGING PROCEDURES                  │
└─────────────────────────────────────────────────────────────────────┘
anatomic_regions ──< body_parts ──< views ──< procedure_views >── procedures
                                   └──< technique_parameters ──< view_techniques >──┘

┌─────────────────────────────────────────────────────────────────────┐
│                      DICOM MODALITY WORKLIST                         │
└─────────────────────────────────────────────────────────────────────┘
mwl_profiles ──< mwl_presentation_contexts
            └──< profile_tag_association >── dicom_tags
                                           └──< mwl_attributes >── mwl_entries ──< mwl_task_mapping
```

## Core Domain Modules

### 1. Authentication & Authorization

#### users
Stores user login credentials and audit information.

**Key Fields:**
- `user_name` (UNIQUE): Login username
- `password_hash`: Encrypted password
- `name`, `surname`: User display name
- `is_active`, `is_deleted`: Account status
- `create_date`, `update_date`: Audit trail

**Relationships:**
- `users ──< user_roles >── roles` (Many-to-many)

#### roles
Available user roles (admin, technician, engineer).

#### user_roles
Junction table associating users with roles.

**Design Notes:**
- Password hashing handled by `CryptoManager` in Core package
- Soft delete pattern (`is_deleted`) preserves historical data
- Audit timestamps track account lifecycle

---

### 2. DICOM Patient Hierarchy

Implements DICOM Patient-Study-Series-Image model.

#### patients
Patient demographics (DICOM Patient Module).

**Key Fields:**
- `patient_name`: Patient's full name (0010,0010)
- `patient_id`: Primary identifier (0010,0020) - **COMPOSITE KEY with issuer**
- `issuer_of_patient_id`: Organization issuing ID (0010,0021)
- `patient_sex`: M/F/O/U (0010,0040)
- `patient_birth_date`: DOB (0010,0030)
- `patient_allergies`: Medical allergies (0010,2110)
- `other_patient_id`: JSON array of alternate IDs

**Indexes:**
- Composite UNIQUE: `(patient_id, issuer_of_patient_id)` - allows same ID from different systems
- Non-unique: `patient_id`, `patient_name` for efficient lookups

**Design Rationale:**
- Same patient_id can exist from multiple issuers (hospital systems)
- `issuer_of_patient_id` NULL means local system
- JSON field for flexible alternate ID storage

#### studies
Study-level DICOM metadata.

**Key Fields:**
- `study_instance_uid` (0020,000D): Unique study identifier
- `study_id` (0020,0010): User-friendly study number
- `admission_id` (0038,0010): Hospital admission ID
- `accession_number` (0008,0050): Order/requisition number
- `study_date`, `study_time`: When study performed
- `study_description`: Procedure description

**Relationships:**
- `patients ──< studies` (One patient → many studies)

**Foreign Keys:**
- `patient_id → patients(id)` with `ON DELETE RESTRICT` (protect data integrity)

#### series
Series-level metadata (grouping of related images).

**Key Fields:**
- `series_instance_uid` (0020,000E): Unique series ID
- `series_number`: Series ordering
- `modality` (0008,0060): DX, CR, CT, MR, etc.
- `series_description`: Series purpose
- `body_part_examined` (0018,0015)
- `patient_position` (0018,5100): Supine, Prone, Erect, Lateral
- `view_position` (0018,5101): AP, PA, LL, RL, etc.

**Relationships:**
- `studies ──< series` (One study → many series)

**Design Notes:**
- CASCADE delete: Deleting study removes all series
- `acquisition_device_id` links to `general_equipments` for device tracking

#### images
Image-level metadata and processing information.

**Key Fields:**
- `instance_number`: Image ordering within series
- `content_date`, `content_time`: Image acquisition timestamp
- `image_type` (0008,0008): ORIGINAL\\PRIMARY, DERIVED, etc.
- `samples_per_pixel`, `photometric_interpretation`: Pixel data encoding
- `rows`, `columns`, `bits_allocated`, `bits_stored`: Image dimensions
- `rescale_intercept`, `rescale_slope`: Pixel value transformation
- `kvp`: X-ray tube voltage used

**Relationships:**
- `series ──< images` (One series → many images)

#### sop_commons
SOP (Service-Object Pair) Common Module - DICOM metadata.

**Key Fields:**
- `sop_class_uid` (0008,0016): DICOM object type
- `sop_instance_uid` (0008,0018): Unique instance identifier
- `instance_creation_date`, `instance_creation_time`: When created

**Relationships:**
- `images ──< sop_commons` (One-to-one)

#### acquisitions
Acquisition-level metadata (exposure details).

**Key Fields:**
- `acquisition_uid` (0008,0017)
- `acquisition_date`, `acquisition_time`
- `radiation_dose`: Exposure dose
- `aec_position`: Auto Exposure Control position

**Relationships:**
- `studies ──< acquisitions` (One study → many acquisitions)
- `series ──< acquisitions`

#### entity_status
Unified workflow status tracking for all DICOM entities.

**Key Fields:**
- `entity_type`: PATIENT, STUDY, SERIES, IMAGE
- `entity_id`: References the actual entity
- `status`: SCHEDULED, IN_PROGRESS, COMPLETED, CANCELLED, ABORTED
- `priority`: URGENT, HIGH, NORMAL, LOW
- `assigned_to`, `transitioned_by`: User tracking

**Indexes:**
- `(entity_type, entity_id, transitioned_at DESC)` - full status history
- `(assigned_to, status)` - task assignment queries
- `(entity_type, status)` - status filtering

**Design Pattern:**
- Single table for all entity types (polymorphic)
- Full audit trail of status transitions
- Supports workflow management (task assignment, prioritization)

---

### 3. Device & Hardware Configuration

Manages X-ray equipment: generators, tubes, detectors, positioners.

#### institutions
Medical institutions where equipment is installed.

**Key Fields:**
- `name`: Institution name (0008,0080)
- `department_name`: Department (0008,1040)
- `address`, `contact_information`

#### general_equipments
X-ray equipment metadata.

**Key Fields:**
- `device_serial_number` (UNIQUE): Equipment serial (0018,1000)
- `device_uid` (0018,1002): Unique device identifier
- `manufacturer` (0008,0070)
- `model_name` (0008,1090)
- `station_name` (0008,1010)
- `date_of_last_calibration` (0018,1200)
- `software_versions` (0018,1020)

**Relationships:**
- `institutions ──< general_equipments` (One institution → many devices)

#### generators
X-ray generator specifications.

**Key Fields:**
- `manufacturer`, `model_number`, `serial_number`
- `output1`, `output2`: Tube output connections
- `is_output1_active`, `is_output2_active`: Active outputs
- `calibration_date`, `installation_date`

**Design Notes:**
- **Denormalized**: Combines manufacturer metadata + installation data
- Intentional for read/write simplicity
- Fixed list: Only software provider can add entries after validation

#### xray_tubes
X-ray tube characteristics.

**Key Fields:**
- `manufacturer`, `model_number`, `serial_number` (UNIQUE)
- `focal_spot`: "0.6mm/1.2mm"
- `anode_heat_capacity`: Heat units (HU)
- `cooling_rate`: HU/min
- `max_voltage`, `max_current`
- `tube_filter`: "0.8 Be", "1.1 AL/75"

**Relationships:**
- Relation to `generators`: 1 generator → 1 or 2 tubes (via `xray_tube_setup`)

#### detectors
Detector specifications (flat-panel, TDI line-scan).

**Key Fields:**
- `manufacturer`, `model_name`
- `resolution`: "8160 × 256"
- `pixel_width`, `pixel_height`: Pixel pitch in mm
- `width`, `height`: Pixel dimensions
- `saturation_value`: Max pixel intensity

#### xray_tube_setup
Links tubes to equipment with position configuration.

**Key Fields:**
- `device_id` → `xray_tubes(id)`
- `equipment_id` → `general_equipments(id)`
- `tube_order`: Tube1, Tube2
- `position`: Stand, Table, Motorized, Portable
- `is_active`: Only ONE active setup per (equipment, tube_order, position)

**Unique Constraint:**
```sql
active_key = CONCAT(equipment_id, '_', tube_order, '_', position) WHEN is_active = 1
UNIQUE KEY uq_active_tube (active_key)
```
**Design Pattern:**
- Generated column ensures only one active tube per configuration
- NULL when `is_active = 0` (allows multiple inactive records)

#### detector_setup
Links detectors to equipment with configuration.

**Key Fields:**
- `device_id` → `detectors(id)`
- `equipment_id` → `general_equipments(id)`
- `detector_order`: Detector1, Detector2
- `position`: Stand, Table, Motorized, Portable
- `horizontal_flip`: Image flip flag
- `crop_mod`: None, Software, Hardware
- `save_raw_data`: No save, Before calibration, After calibration, All

**Unique Constraint:**
Similar active_key pattern as `xray_tube_setup`.

#### positioners
Motorized positioning system definitions.

**Key Fields:**
- `position_name` (UNIQUE): "C-Arm Rotation", "Table Vertical"
- `description`

#### positioner_steps
Motion sequences for each positioner.

**Key Fields:**
- `positioner_id`
- `role`: TableDetector, StandDetector, PortableDetector, PrimaryTube, SecondaryTube
- `motion_code`: "X-2331", "D-1234" (device-specific command)
- `step_order`: Execution sequence

**Design Pattern:**
- Supports multi-step motions for complex positioning
- Role-based: Same positioner can move different components

#### device_connections
Communication interfaces for devices.

**Key Fields:**
- `generator_id`, `detector_id`, `tube_id`, `positioner_id` (nullable FKs)
- `connector`: SyncBox, Direct
- `protocol`: RS_232, RS_485, CAN, LAN, MODBUS_RTU, MODBUS_TCP, WIFI, USB, ANALOG
- `interface_name`: COM port or network interface
- `parameters`: JSON (baud rate, IP address, etc.)

**Design Pattern:**
- Polymorphic: One connection record can link to different device types
- JSON parameters for protocol-specific settings

---

### 4. Scan Protocols & Imaging Procedures

#### anatomic_regions
High-level body regions (HEAD, NECK, THORAX, ABDOMEN, PELVIS, SPINE, UPPER EXTREMITY, LOWER EXTREMITY).

**Key Fields:**
- `name`: UI display name
- `code_value`, `coding_scheme` (SRT/SNOMED): DICOM-compliant coding
- `code_meaning`: Human-readable description

#### body_parts
Specific anatomical structures within regions.

**Key Fields:**
- `name`: SKULL, CLAVICLE, KNEE, etc.
- `code_value`, `coding_scheme` (ETRK custom or SNOMED)
- `anatomic_region_id` → `anatomic_regions(id)`

**Relationships:**
- `anatomic_regions ──< body_parts` (One region → many body parts)

#### technique_parameters
Exposure technique factors (kV, mA, ms) per body part and patient size.

**Key Fields:**
- `body_part_id` → `body_parts(id)`
- `size`: Fat, Medium, Thin, Paediatric
- `technique_profile`: AP|PA, LAT, OBL, AXIAL, DUAL
- `kvp`, `ma`, `ms`: Exposure factors
- `focal_spot`: Small/large focal spot selection
- `sid_min`, `sid_max`: Source-to-Image Distance range (cm)
- `grid_type`, `grid_ratio`: Anti-scatter grid
- `exposure_style`: Mas Mode, Time Mode, AEC Mode, Manual
- `aec_field`, `aec_density`: Auto Exposure Control settings

**Design Notes:**
- Supports multiple techniques per body part (different sizes/projections)
- AEC (Automatic Exposure Control) parameters included

#### views
Imaging views/orientations with positioning details.

**Key Fields:**
- `name`: AP, LAT, RL, LLD, etc.
- `body_part_id` → `body_parts(id)`
- `patient_position` (0018,5100): Supine, Prone, Erect, Lateral
- `projection_profile`: AP|PA, LAT, OBL, AXIAL, DUAL
- `patient_orientation_row`, `patient_orientation_col` (0020,0020): DICOM orientation codes
- `view_position` (0018,5101): AP, PA, LL, RL, RLD, LLD, RLO, LLO
- `image_laterality` (0020,0062): L, R, B
- `collimator_size`, `image_processing_algorithm`
- `position_name` → `positioners(position_name)`: Motorized positioning

**Relationships:**
- `body_parts ──< views` (One body part → many views)
- `views → positioners` (Optional: Motorized positioning)

#### view_techniques
Links views to technique parameters (supports multi-exposure views).

**Key Fields:**
- `view_id` → `views(id)`
- `technique_parameter_id` → `technique_parameters(id)`
- `seq`: Execution order (1, 2, ...)
- `role`: PRIMARY, LOW, HIGH (for dual-energy imaging)

**Unique Constraints:**
- `PRIMARY KEY (view_id, seq)` - ensures sequential ordering
- `UNIQUE (view_id, technique_parameter_id)` - prevents duplicate techniques

**Design Pattern:**
- Supports dual-energy/multi-shot imaging
- Example: Chest PA with low/high kV exposures

#### procedures
Imaging procedures (exam types).

**Key Fields:**
- `name`: "CSpine AP", "Chest PA", "Pelvis AP"
- `code_value`, `coding_scheme` (SRT): DICOM procedure code
- `anatomic_region_id`, `body_part_id` (nullable): Optional classification
- `is_true_size`: Print at 1:1 scale
- `print_orientation`, `print_format`: Print settings

**Design Notes:**
- Custom procedures can span multiple body parts (nullable region/body_part)
- Standard procedures linked to specific body part

#### procedure_views
Associates procedures with required views (many-to-many).

**Relationships:**
- `procedures ──< procedure_views >── views`

---

### 5. DICOM Modality Worklist

Implements DICOM MWL query/response handling.

#### dicom_tags
Global DICOM tag dictionary.

**Key Fields:**
- `name`: Tag identifier (e.g., 'AccessionNumber')
- `display_name`: UI display
- `group_hex`, `element_hex`: DICOM tag (0x0008, 0x0050)
- `pgroup_hex`, `pelement_hex`: Parent tag for sequences
- `is_active`, `is_retired`: Tag lifecycle

**Design Pattern:**
- Global admin-managed reference table
- ALL foreign keys use `ON DELETE RESTRICT` to prevent accidental deletion
- Supports nested DICOM sequences (parent tags)

#### mwl_profiles
MWL query profiles (DxWorklist, etc.).

#### mwl_presentation_contexts
Transfer syntaxes per profile.

**Key Fields:**
- `profile_id` → `mwl_profiles(id)`
- `transfer_syntax_uid`: "1.2.840.10008.1.2" (Little Endian Implicit)

#### profile_tag_association
Links profiles to DICOM tags with identifier/mandatory flags.

**Key Fields:**
- `profile_id` → `mwl_profiles(id)`
- `tag_id` → `dicom_tags(id)` with `ON DELETE RESTRICT`
- `is_identifier`: Tag used for matching (PatientID, AccessionNumber)
- `is_mandatory`: Must be present

**Design Pattern:**
- Flexible MWL tag selection per profile
- `is_mandatory = TRUE` requires `is_identifier = TRUE`

#### mwl_entries
Worklist queue entries.

**Key Fields:**
- `source`: LOCAL, RIS
- `profile_id` → `mwl_profiles(id)`
- `status`: SCHEDULED, PENDING, COMPLETED, CANCELLED, IN_PROGRESS, ABORTED
- `study_instance_uid`: Links to future study

**Relationships:**
- `mwl_entries ──< mwl_attributes` (One entry → many tag values)

#### mwl_attributes
Actual DICOM tag values from MWL query.

**Key Fields:**
- `mwl_entry_id` → `mwl_entries(id)` with `CASCADE`
- `dicom_tag_id` → `dicom_tags(id)` with `RESTRICT`
- `tag_value`: String representation

**Design Pattern:**
- EAV (Entity-Attribute-Value) model for flexible DICOM data
- CASCADE delete: Removing worklist entry removes all attributes
- RESTRICT delete: Prevent deletion of tag definitions

#### mwl_task_mapping
Links worklist entries to created DICOM instances.

**Key Fields:**
- `mwl_entry_id` → `mwl_entries(id)`
- `procedure_id` → `procedures(id)`
- `study_id`, `series_id`, `images_id`, `sop_common_id`, `acquisition_id`

**Design Pattern:**
- Tracks worklist fulfillment
- Links scheduled procedure to actual performed imaging

#### worklist_field_configurations
Configurable primary key fields for MWL matching.

**Key Fields:**
- `field_name`: PatientID, AccessionNumber, StudyInstanceUID, etc.
- `is_enabled`: Whether field is used for matching

---

### 6. PACS & MPPS

#### pacs_nodes
PACS/MPPS server configurations.

**Key Fields:**
- `entity_type`: Archive, MPPS
- `host_name`, `host_ip`, `host_port`
- `called_aet`, `calling_aet`: DICOM Application Entity Titles
- `is_default`: Default node selection

---

### 7. System Configuration

#### environment_settings
Global system settings (singleton table).

**Key Fields:**
- `study_level`: Multi-Series Study, Single-Series Study
- `lookup_table`: VOI LUT, None
- `worklist_clear_period_days`, `worklist_refresh_period_seconds`
- `delete_log_period_days`: Log cleanup
- `auto_refresh_worklist`, `enable_mpps`, `continue_on_echo_fail`: Feature flags

#### image_comments
Predefined image quality comments.

**Key Fields:**
- `is_reject`: Flag for rejection reasons
- `heading`, `comment`: Display and full text
- Trigger: `TRIM(comment)` before insert

**Use Case:**
- Standardized QA/QC comments
- Technologist annotations
- Reject reason tracking

---

## Design Patterns & Best Practices

### 1. DICOM Compliance
- Field names match DICOM tag descriptions
- Tag numbers in comments: `(0010,0020)`
- Supports DICOM hierarchy (Patient → Study → Series → Image)

### 2. Soft Delete Pattern
- `is_deleted` field instead of hard deletes
- Preserves historical data
- Supports audit requirements

### 3. Audit Trail
- `create_date`, `update_date` on most tables
- Automatic timestamp updates via `ON UPDATE CURRENT_TIMESTAMP`
- `entity_status` tracks state transitions with user attribution

### 4. Generated Columns for Constraints
- `xray_tube_setup.active_key`, `detector_setup.active_key`
- Enforces "only one active" business rule at database level
- NULL-safe uniqueness (inactive records allowed)

### 5. Polymorphic Relationships
- `entity_status` tracks multiple entity types
- `device_connections` links to different devices via nullable FKs

### 6. JSON for Flexible Data
- `patients.other_patient_id`: Array of alternate IDs
- `device_connections.parameters`: Protocol-specific settings

### 7. Foreign Key Policies
- **CASCADE**: Deleting parent removes children (DICOM hierarchy)
- **RESTRICT**: Prevent deletion of referenced data (global reference tables)
- **SET NULL**: Optional relationships (e.g., `mwl_entries.profile_id`)

### 8. Indexed for Performance
- Composite indexes on common query patterns
- UNIQUE constraints on business keys
- Covering indexes for workflow queries

---

## Current Limitations & Future Enhancements

### Known Limitations
1. **Single timezone**: No timezone-aware timestamps
2. **Limited DICOM tag history**: Tag value changes not tracked
3. **No image binary storage**: Images stored in filesystem, not database

### Planned Enhancements
1. **Audit logging table**: Comprehensive change tracking
2. **DICOM SR support**: Structured reporting tables
3. **Multi-tenancy**: Partition data by institution
4. **HL7 integration**: Admission/discharge/transfer messages

---

**Previous**: [01-Architecture-Overview.md](./01-Architecture-Overview.md)  
**Next**: (To be added - Module-specific documentation)
