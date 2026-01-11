# Procedure-Based Patient Addition UI Design

## Document Information
- **Version**: 1.0
- **Date**: 2026-01-11
- **Status**: Design Phase (Implementation Deferred)
- **Related Issue**: GitHub Issue #124 [Epic 3]

---

## Overview

This document outlines the design for redesigning the "Add Patient" dialog to use procedure selection instead of body part selection for improved workflow efficiency.

---

## Current Behavior

### Existing Flow
1. User opens "Add Patient" dialog
2. User selects **Anatomic Region** (e.g., Chest, Abdomen)
3. User selects **Body Part** (e.g., Chest PA, Chest Lateral)
4. All views associated with the body part are shown
5. User confirms and views are pre-selected for examination

### Current Implementation
**File**: `View/Dialog/AddPatientDialog.h:27-29`
```cpp
explicit AddPatientDialog(
    const QVector<AnatomicRegion>& regions,
    const QVector<BodyPart>& bodyParts,  // Uses body parts
    QWidget *parent = nullptr);
```

### Limitations
- Body part selection shows ALL views for that body part
- No filtering based on clinical procedure/protocol
- Technologist must manually select/deselect views
- RIS procedure codes don't map directly to body parts

---

## Proposed Behavior

### New Flow
1. User opens "Add Patient" dialog
2. User selects **Anatomic Region** (filters available procedures)
3. User selects **Procedure** (e.g., "Chest 2-View", "Chest PA Only")
4. Only views associated with that specific procedure are shown
5. User confirms and relevant views are pre-selected for examination

### Benefits
- More intuitive workflow matching clinical protocols
- Direct mapping from RIS procedure codes to views
- Fewer clicks and less manual selection
- Better integration with MWL/RIS systems

---

## UI Design

### Proposed Dialog Layout

```
+----------------------------------------------------------+
|  Add Patient                                         [X] |
+----------------------------------------------------------+
|                                                          |
|  Patient Information                                     |
|  +----------------------------------------------------+  |
|  | Patient ID:    [______________]                    |  |
|  | Patient Name:  [______________]                    |  |
|  | Date of Birth: [__/__/____]                        |  |
|  +----------------------------------------------------+  |
|                                                          |
|  Procedure Selection                                     |
|  +----------------------------------------------------+  |
|  | Anatomic Region:  [ Chest          v ]             |  |
|  +----------------------------------------------------+  |
|  | Available Procedures:                              |  |
|  | +------------------------------------------------+ |  |
|  | | [ ] Chest 2-View (PA + Lateral)                | |  |
|  | | [ ] Chest PA Only                              | |  |
|  | | [ ] Chest Lateral Only                         | |  |
|  | | [ ] Chest AP (Portable)                        | |  |
|  | | [ ] Chest Lordotic                             | |  |
|  | +------------------------------------------------+ |  |
|  +----------------------------------------------------+  |
|                                                          |
|  Selected Views (auto-populated from procedure)          |
|  +----------------------------------------------------+  |
|  | [x] Chest PA                                       |  |
|  | [x] Chest Lateral                                  |  |
|  +----------------------------------------------------+  |
|                                                          |
|                      [ Cancel ]  [ Add Patient ]         |
+----------------------------------------------------------+
```

### Interaction Flow

```
[User selects Anatomic Region]
         |
         v
[Filter procedures by body_part_id]
         |
         v
[Display filtered procedures]
         |
         v
[User selects Procedure]
         |
         v
[Query procedure_views table]
         |
         v
[Display associated views (pre-selected)]
         |
         v
[User confirms]
         |
         v
[Create examination with selected views]
```

---

## Database Schema

### Required Queries

**1. Get procedures for anatomic region/body part:**
```sql
SELECT p.id, p.name, p.code, p.description
FROM procedures p
INNER JOIN body_parts bp ON p.body_part_id = bp.id
WHERE bp.anatomic_region_id = :anatomicRegionId
  AND p.is_active = TRUE
ORDER BY p.display_order, p.name;
```

**2. Get views for selected procedure:**
```sql
SELECT v.id, v.name, v.code, v.description, pv.is_required
FROM views v
INNER JOIN procedure_views pv ON pv.view_id = v.id
WHERE pv.procedure_id = :procedureId
  AND v.is_active = TRUE
ORDER BY pv.display_order, v.name;
```

### Entity Relationships

```
+------------------+       +------------------+       +------------------+
|  anatomic_regions|       |    body_parts    |       |   procedures     |
+------------------+       +------------------+       +------------------+
| id               |<----->| anatomic_region_id|<---->| body_part_id     |
| name             |       | id               |       | id               |
| code             |       | name             |       | name             |
+------------------+       +------------------+       | code             |
                                                      +------------------+
                                                             |
                                                             v
                                                      +------------------+
                                                      | procedure_views  |
                                                      +------------------+
                                                      | procedure_id     |
                                                      | view_id          |
                                                      | is_required      |
                                                      | display_order    |
                                                      +------------------+
                                                             |
                                                             v
                                                      +------------------+
                                                      |      views       |
                                                      +------------------+
                                                      | id               |
                                                      | name             |
                                                      | code             |
                                                      +------------------+
```

---

## Backward Compatibility

### Option 1: Keep Both Modes
- Add a toggle/preference for "Simple Mode" (body part) vs "Advanced Mode" (procedure)
- Default to procedure-based for new installations
- Allow existing sites to continue using body part selection

### Option 2: Migration Path
- Phase 1: Add procedure selection alongside body part selection
- Phase 2: Make procedure selection the default
- Phase 3: Deprecate body part-only selection (with notice)

### Recommendation
**Option 1** is recommended for initial release to avoid disruption to existing workflows.

---

## Implementation Considerations

### Files to Modify

**Dialog:**
- `View/Dialog/AddPatientDialog.h` - Update constructor and add procedure support
- `View/Dialog/AddPatientDialog.cpp` - Implement new selection logic
- `View/Dialog/AddPatientDialog.ui` - Redesign UI layout

**Repository:**
- `ScanProtocol/Repository/ScanProtocolRepository.h` - Add procedure query methods
- `ScanProtocol/Repository/ScanProtocolRepository.cpp` - Implement queries

**Entity (if not existing):**
- `Common/Include/ScanProtocol/Data/Entity/ProcedureView.h` - Junction table entity

### Proposed Constructor Signature
```cpp
explicit AddPatientDialog(
    const QVector<AnatomicRegion>& regions,
    const QVector<Procedure>& procedures,  // New: procedures with views
    QWidget *parent = nullptr,
    SelectionMode mode = SelectionMode::Procedure);  // New: selection mode
```

---

## Testing Plan

### Unit Tests
- [ ] ProcedureRepository.getProceduresByAnatomicRegion()
- [ ] ProcedureRepository.getViewsByProcedure()
- [ ] AddPatientDialog selection mode switching

### Integration Tests
- [ ] End-to-end patient addition with procedure selection
- [ ] MWL integration with procedure code mapping
- [ ] Backward compatibility with body part selection

### User Acceptance Criteria
- [ ] Technologist can add patient using procedure selection
- [ ] Views are correctly pre-populated based on procedure
- [ ] RIS procedure codes map to correct procedures
- [ ] Existing body part workflow still functions

---

## Open Questions

1. **Multi-procedure selection**: Should users be able to select multiple procedures for a single patient visit?

2. **Custom view modification**: After procedure selection, can users add/remove views from the pre-populated list?

3. **Procedure ordering**: How should procedures be ordered in the list (alphabetical, frequency, custom)?

4. **RIS integration**: Should procedure selection be auto-populated from MWL worklist entries?

---

## Next Steps

1. [ ] Review design with clinical stakeholders
2. [ ] Finalize UI mockups with UX team
3. [ ] Verify database schema supports proposed queries
4. [ ] Estimate development effort
5. [ ] Schedule implementation sprint

---

## Related Documentation

- [07-Examination-Workflow-Design.md](./07-Examination-Workflow-Design.md)
- [02-Database-Schema.md](./02-Database-Schema.md)
- [Epic 4: RIS Procedure Mapping](../docs/EPIC_STATUS_SUMMARY.md)
