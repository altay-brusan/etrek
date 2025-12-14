# Examination Workflow Design

## Overview

This document outlines the complete examination workflow from worklist selection to image acquisition, including LOCAL vs RIS source handling, database operations, and UI components.

## Workflow Diagram

```
User Double-Clicks Worklist Entry
    ↓
WorkListPageDelegate::onWorklistItemDoubleClicked()
    ↓
Retrieve WorklistEntry from Repository
    ↓
Set ExaminationContext in ContextManager
    ↓
Check WorklistEntry.Source
    ↓
┌──────────────────────┴──────────────────────┐
│ LOCAL                          │ RIS        │
│ (User-created in worklist)    │ (From PACS)│
├───────────────────────────────┼────────────┤
│ 1. Open ViewSelectionDialog   │ Skip this  │
│ 2. User selects Procedure     │            │
│ 3. User selects Views         │            │
│ 4. Update ExaminationContext  │            │
└──────────────────────┬──────────────────────┘
    ↓
Create/Update Study in Database
    │
    ├─ studies table
    │  - study_instance_uid (DICOM UID)
    │  - patient_id (FK → patients)
    │  - accession_number (from worklist)
    │  - study_date, study_time
    │
    └─ Update MWL Entry Status
       mwl_entries.status = IN_PROGRESS
    ↓
Create Series for Each Selected View
    │
    └─ series table (one per view)
       - series_instance_uid (DICOM UID)
       - study_id (FK → studies)
       - modality = 'DX'
       - series_description = view name
       - body_part_examined
       - view_position
    ↓
Open ExaminationPage
    │
    ├─ Left Panel (Main Display Area)
    │  ├─ View Thumbnails (ThumbnailImageListWidget)
    │  │  - VTK-based icon rendering
    │  │  - Shows each series/view
    │  │  - Clickable to select active series
    │  │
    │  └─ Main Image Viewer (QVTKOpenGLNativeWidget)
    │     - Large VTK viewer for current image
    │     - Window/level controls
    │     - Zoom/pan controls
    │
    └─ Right Panel (Control Area)
       ├─ Patient Demographics (ExamTitleWidget)
       │  - Patient Name, ID, Gender, Age
       │  - Accession Number
       │
       ├─ Study Control (StudyControlWidget)
       │  - New Study, Edit Study buttons
       │  - Study list table
       │
       ├─ Technical Parameters (ExposureDetailWidget)
       │  - kVp, mA, mAs, TIME, DEN
       │  - Load from TechniqueParameter
       │  - Editable with up/down buttons
       │
       ├─ Body Size Selection (BodySizeWidget)
       │  - Child, Adult icons
       │  - Standing, hand positions
       │
       ├─ Exposure Controls (ExposureControlWidget)
       │  - AEC, Manual, Time mode
       │
       ├─ Generator Control (GeneratorControlWidget)
       │  - Focal spot selection
       │  - Tube selection
       │
       ├─ Detector Control (DetectorControlWidget)
       │  - Detector selection
       │  - Position control
       │
       ├─ Message Display
       │  - Current view name
       │  - Device status messages
       │
       └─ Action Buttons
          - Ready, Reset, 0%
          - Print, Close
    ↓
Image Acquisition Loop
    │
    For each selected view/series:
    │
    ├─ 1. Load Technique Parameters
    │     - Retrieve from technique_parameters table
    │     - Match: body_part + size + projection
    │     - Display in ExposureDetailWidget
    │
    ├─ 2. Configure Equipment
    │     - Set generator parameters (kVp, mA)
    │     - Position detector
    │     - Set collimator
    │
    ├─ 3. User Presses "Ready" → Expose Button
    │     - Send exposure command to generator
    │     - Trigger image acquisition
    │
    ├─ 4. Image Received from Detector
    │     - Raw image data
    │     - Apply calibration
    │     - Display in VTK viewer
    │
    ├─ 5. Create Image Record in Database
    │     - images table
    │     - sop_commons table
    │     - acquisitions table
    │     - Link to series_id
    │
    ├─ 6. Add Thumbnail to List
    │     - Update ThumbnailImageListWidget
    │     - Show acquired image icon
    │
    └─ 7. Move to Next View (if any)
       - Repeat loop
    ↓
Examination Complete
    │
    ├─ Update MWL Entry Status
    │  mwl_entries.status = COMPLETED
    │
    ├─ Create MWL Task Mapping
    │  mwl_task_mapping links worklist → study → series → images
    │
    └─ Optional: Send to PACS
       - C-STORE to Archive PACS
       - MPPS status update
```

## Database Operations

### 1. Study Creation

```sql
INSERT INTO studies (
    patient_id,
    study_instance_uid,
    study_id,
    admission_id,
    accession_number,
    referring_physician_name,
    study_date,
    study_time,
    study_description
) VALUES (
    <patient_id from worklist>,
    <generated DICOM UID>,
    <auto-increment or user-defined>,
    <from worklist>,
    <from worklist>,
    <from worklist>,
    CURDATE(),
    CURTIME(),
    <procedure name>
);
```

### 2. Series Creation (One Per View)

```sql
INSERT INTO series (
    study_id,
    series_instance_uid,
    series_number,
    modality,
    series_description,
    body_part_examined,
    patient_position,
    view_position,
    image_laterality
) VALUES (
    <study_id>,
    <generated DICOM UID>,
    <sequence 1, 2, 3...>,
    'DX',
    <view name>,
    <body part from view>,
    <patient position from view>,
    <view position: AP, LAT, etc.>,
    <laterality: R, L, B>
);
```

### 3. Image Creation (After Acquisition)

```sql
INSERT INTO images (
    study_id,
    series_id,
    instance_number,
    content_date,
    content_time,
    rows,
    columns,
    bits_allocated,
    bits_stored,
    kvp,
    rescale_intercept,
    rescale_slope,
    -- ... more DICOM attributes
) VALUES (...);
```

### 4. Worklist Status Update

```sql
-- When examination starts
UPDATE mwl_entries
SET status = 'IN_PROGRESS',
    study_instance_uid = <created_study_uid>
WHERE id = <worklist_entry_id>;

-- When examination completes
UPDATE mwl_entries
SET status = 'COMPLETED'
WHERE id = <worklist_entry_id>;
```

### 5. MWL Task Mapping

```sql
INSERT INTO mwl_task_mapping (
    mwl_entry_id,
    procedure_id,
    study_id,
    series_id,
    images_id,
    sop_common_id,
    acquisition_id
) VALUES (
    <worklist_entry_id>,
    <procedure_id>,
    <study_id>,
    <series_id>,
    <image_id>,
    <sop_common_id>,
    <acquisition_id>
);
```

## Component Architecture

### ExamPage Widget Composition

```
ExamPage (QWidget)
├─ Main Layout (QHBoxLayout)
│  ├─ Left Panel (QWidget)
│  │  ├─ Thumbnail Area (QWidget)
│  │  │  └─ ThumbnailImageListWidget
│  │  │     - VTK-based thumbnail rendering
│  │  │     - Horizontal or vertical list
│  │  │     - Click to select active view
│  │  │
│  │  └─ Main Viewer Area (QWidget)
│  │     └─ QVTKOpenGLNativeWidget
│  │        - vtkGenericOpenGLRenderWindow
│  │        - vtkRenderer
│  │        - vtkImageActor for DICOM image
│  │        - vtkInteractorStyleImage (window/level, zoom, pan)
│  │
│  └─ Right Panel (QScrollArea)
│     ├─ ExamTitleWidget (Patient demographics)
│     ├─ StudyControlWidget (Study management)
│     ├─ ExposureDetailWidget (kVp, mA, mAs, TIME, DEN)
│     ├─ BodySizeWidget (Patient size selection)
│     ├─ ExposureControlWidget (AEC, Manual, Time mode)
│     ├─ GeneratorControlWidget (Focal spot, tube)
│     ├─ DetectorControlWidget (Detector selection)
│     ├─ BuckyControlWidget (Grid control)
│     ├─ AecControlWidget (AEC fields)
│     ├─ EndExposureControlWidget (Termination mode)
│     ├─ FocalSpotControlWidget (Focal spot size)
│     ├─ ExposureApplicationControlWidget (Ready, Expose buttons)
│     ├─ Message Display (QLabel)
│     └─ Action Buttons (Print, Close)
```

### ExamPageDelegate Responsibilities

```cpp
class ExamPageDelegate : public QObject, public IDelegate, public IPageAction {
    Q_OBJECT
    Q_INTERFACES(IDelegate IPageAction)

public:
    ExamPageDelegate(
        ExamPage* ui,
        std::shared_ptr<WorklistRepository> worklistRepo,
        std::shared_ptr<DicomRepository> dicomRepo,
        std::shared_ptr<ScanProtocolRepository> scanProtocolRepo,
        std::shared_ptr<DeviceRepository> deviceRepo,
        std::weak_ptr<IContextManager> contextManager,
        QObject* parent = nullptr
    );

    // IDelegate
    QString name() const override { return "ExamPageDelegate"; }
    void attachDelegates(const QVector<QObject*>& delegates) override;

    // IPageAction
    void apply() override;   // Save intermediate state
    void accept() override;  // Finalize examination
    void reject() override;  // Cancel examination

signals:
    void examinationCompleted(int studyId);
    void closeExamination();

private slots:
    // Initialization
    void onPageLoaded();
    void loadExaminationContext();
    void loadPatientDemographics();
    void loadTechniqueParameters();

    // View/Series management
    void onViewSelected(int seriesId);
    void onNextView();
    void onPreviousView();

    // Image acquisition
    void onReadyButtonClicked();
    void onExposeButtonClicked();
    void onImageReceived(const QByteArray& imageData);
    void onAcquisitionComplete();

    // Technical parameter changes
    void onKvpChanged(int kvp);
    void onMaChanged(int ma);
    void onMasChanged(int mas);
    void onTimeChanged(int time);

    // Study/Series operations
    void onNewStudyClicked();
    void onEditStudyClicked();
    void onNewViewClicked();

private:
    // Database operations
    void createStudy();
    void createSeries(const Etrek::ScanProtocol::Data::Entity::View& view);
    void createImage(const QByteArray& imageData);
    void updateWorklistStatus(ProcedureStepStatus status);

    // VTK operations
    void initializeVtkViewer();
    void displayImage(const QByteArray& imageData);
    void updateThumbnails();

    // Equipment control
    void configureGenerator();
    void configureDetector();
    void setTechniqueParameters();

    ExamPage* ui;
    std::shared_ptr<WorklistRepository> m_worklistRepo;
    std::shared_ptr<DicomRepository> m_dicomRepo;
    std::shared_ptr<ScanProtocolRepository> m_scanProtocolRepo;
    std::shared_ptr<DeviceRepository> m_deviceRepo;
    std::weak_ptr<IContextManager> m_contextManager;

    // Current examination state
    WorklistEntry m_worklistEntry;
    int m_studyId = -1;
    QVector<int> m_seriesIds;
    int m_currentSeriesIndex = 0;
    QVector<Etrek::ScanProtocol::Data::Entity::View> m_views;
    Etrek::ScanProtocol::Data::Entity::TechniqueParameter m_currentTechnique;

    // VTK members
    vtkRenderer* m_renderer = nullptr;
    vtkGenericOpenGLRenderWindow* m_renderWindow = nullptr;
    vtkImageActor* m_imageActor = nullptr;
};
```

### ExamPageBuilder

```cpp
class ExamPageBuilder : public IWidgetDelegateBuilder<ExamPage, ExamPageDelegate> {
public:
    std::pair<ExamPage*, ExamPageDelegate*>
    build(const DelegateParameter& params,
          QWidget* parentWidget = nullptr,
          QObject* parentDelegate = nullptr) override {

        // Create repositories
        auto worklistRepo = std::make_shared<WorklistRepository>(params.dbConnection);
        auto dicomRepo = std::make_shared<DicomRepository>(params.dbConnection);
        auto scanProtocolRepo = std::make_shared<ScanProtocolRepository>(params.dbConnection, nullptr);
        auto deviceRepo = std::make_shared<DeviceRepository>(params.dbConnection, nullptr);

        // Create widget
        auto widget = new ExamPage(parentWidget);

        // Create delegate
        auto delegate = new ExamPageDelegate(
            widget,
            worklistRepo,
            dicomRepo,
            scanProtocolRepo,
            deviceRepo,
            params.contextManager,
            parentDelegate
        );

        return { widget, delegate };
    }
};
```

## Implementation Steps

### Phase 1: Workflow Routing (LOCAL vs RIS)

1. Update `WorkListPageDelegate::onWorklistItemDoubleClicked()`
   - Check `WorklistEntry.Source`
   - If LOCAL: Open ViewSelectionDialog
   - If RIS: Skip directly to ExaminationPage

### Phase 2: ExamPageDelegate Creation

1. Create `Application/Delegate/ExamPageDelegate.h`
2. Create `Application/Delegate/ExamPageDelegate.cpp`
3. Implement MVD pattern with repository injection

### Phase 3: ExamPageBuilder Creation

1. Create `Application/Builder/ExamPageBuilder.h`
2. Create `Application/Builder/ExamPageBuilder.cpp`
3. Follow builder pattern with DelegateParameter

### Phase 4: VTK Image Viewer Integration

1. Add QVTKOpenGLNativeWidget to ExamPage.ui
2. Initialize VTK pipeline in ExamPageDelegate
3. Implement image display functions

### Phase 5: Database Operations

1. Implement study creation
2. Implement series creation (one per view)
3. Implement image creation
4. Implement worklist status updates

### Phase 6: Context Flow

1. Set ExaminationContext when worklist item selected
2. Pass context through ViewSelectionDialog (if LOCAL)
3. Retrieve context in ExamPageDelegate
4. Update context as examination progresses

### Phase 7: UI Enhancements

1. Connect technical parameter widgets to delegate
2. Implement thumbnail list updates
3. Add device message display
4. Wire action buttons

## Next Steps

1. **Immediate**: Update WorkListPageDelegate to route based on source
2. **Next**: Create ExamPageDelegate skeleton
3. **Then**: Add VTK viewer to ExamPage
4. **Finally**: Implement database operations

---

**Related Documentation**:
- [Model-View-Delegate Pattern](03-Model-View-Delegate-Pattern.md)
- [Builder Pattern](04-Builder-Pattern.md)
- [Application Workflows](06-Application-Workflows.md)
- [Database Schema](02-Database-Schema.md)
