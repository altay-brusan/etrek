# Examination Workflow Implementation Progress

## Summary

We've begun implementing the complete examination workflow from worklist selection to image acquisition. This document tracks our progress and outlines next steps.

## Completed Tasks

### 1. ✅ Comprehensive Workflow Design

**File**: `Documents/07-Examination-Workflow-Design.md`

Created a complete design document covering:
- LOCAL vs RIS source routing logic
- Database operations (study/series/image creation)
- Component architecture (ExamPage, ExamPageDelegate)
- VTK image viewer integration plan
- Step-by-step implementation phases

### 2. ✅ WorkListPageDelegate Source Routing

**Modified**: `Application/Delegate/WorkListPageDelegate.cpp`

**Changes**:
```cpp
void WorkListPageDelegate::onWorklistItemDoubleClicked(int entryId) {
    // Create ExaminationContext
    auto examContext = std::make_shared<ExaminationContext>(selectedEntry);
    ctxMgr->setWorkflowContext("Examination", examContext);

    // Route based on source
    if (selectedEntry.Source == Source::LOCAL) {
        // LOCAL: Show ViewSelectionDialog
        // User selects procedure and views
        ViewSelectionDialogBuilder builder;
        auto [dialog, delegate] = builder.build(params, ui, this);

        // Update context with selected items
        examContext->setProcedureId(procedureId);
        examContext->setViewIds(viewIds);

        emit startExamination(entryId);
    } else {
        // RIS: Skip dialog, go directly to examination
        // Views already defined in worklist attributes
        emit startExamination(entryId);
    }
}
```

**Behavior**:
- **LOCAL** worklist entries: Opens ViewSelectionDialog for user to select procedure and views
- **RIS** worklist entries: Skips dialog, proceeds directly to examination (views already defined)

### 3. ✅ ExaminationContext Enhancement

**Modified**:
- `Core/Context/ExaminationContext.h`
- `Core/Context/ExaminationContext.cpp`

**Added Methods**:
```cpp
// Set/get selected procedure ID (for LOCAL worklist entries)
void setProcedureId(int procedureId);
int procedureId() const;

// Set/get selected view IDs (one per series)
void setViewIds(const QVector<int>& viewIds);
QVector<int> viewIds() const;
```

**New Fields**:
```cpp
int m_procedureId = -1;
QVector<int> m_viewIds;
```

**Purpose**:
- Stores user's selection from ViewSelectionDialog (LOCAL source)
- Carries procedure and view information through the examination workflow
- Available to ExamPageDelegate when page loads

### 4. ✅ ExamPageDelegate Implementation

**Created Files**:
- `Application/Delegate/ExamPageDelegate.h`
- `Application/Delegate/ExamPageDelegate.cpp`

**Key Features**:
- Full MVD pattern implementation
- Repository injection (WorklistRepository, DicomRepository, ScanProtocolRepository, DeviceRepository)
- IDelegate and IPageAction interface implementation
- Comprehensive slot implementations for:
  - Initialization (onPageLoaded, loadExaminationContext, loadViews, loadTechniqueParameters)
  - View management (onViewSelected, onNextView, onPreviousView)
  - Image acquisition (onReadyButtonClicked, onExposeButtonClicked, onImageReceived)
  - Technical parameters (onKvpChanged, onMaChanged, etc.)
  - Study operations (onNewStudyClicked, onEditStudyClicked, onNewViewClicked)
- Database operation methods (createStudy, createSeriesForViews, updateWorklistStatus)
- VTK viewer integration (initializeVtkViewer, displayImage, clearVtkViewer)
- Equipment control placeholders (configureGenerator, configureDetector)

### 5. ✅ ExamPageBuilder Implementation

**Created Files**:
- `Application/Builder/ExamPageBuilder.h`
- `Application/Builder/ExamPageBuilder.cpp`

**Implementation Details**:
```cpp
std::pair<ExamPage*, ExamPageDelegate*>
ExamPageBuilder::build(const DelegateParameter& params,
                       QWidget* parentWidget,
                       QObject* parentDelegate) {
    // Create page widget
    auto* page = new ExamPage(parentWidget);

    // Create all repositories
    auto worklistRepo = std::make_shared<WorklistRepository>(params.dbConnection);
    auto dicomRepo = std::make_shared<DicomRepository>(params.dbConnection);
    auto scanProtocolRepo = std::make_shared<ScanProtocolRepository>(params.dbConnection, nullptr);
    auto deviceRepo = std::make_shared<DeviceRepository>(params.dbConnection, nullptr);

    // Create delegate with all dependencies
    auto* delegate = new ExamPageDelegate(
        page, worklistRepo, dicomRepo, scanProtocolRepo, deviceRepo,
        params.dbConnection, params.contextManager, parentDelegate
    );

    return { page, delegate };
}
```

### 6. ✅ MainWindowDelegate Integration

**Modified Files**:
- `Application/Delegate/MainWindowDelegate.h` - Added ExamPageDelegate member and onStartExamination slot
- `Application/Delegate/MainWindowDelegate.cpp` - Implemented examination page wiring

**Key Changes**:
1. Added `#include "ExamPageDelegate.h"` and `#include "ExamPageBuilder.h"`
2. Added member variable: `ExamPageDelegate *m_examPageDelegate{nullptr};`
3. Added slot: `void onStartExamination(int worklistEntryId);`
4. Connected `WorkListPageDelegate::startExamination` signal to `onStartExamination` slot
5. Implemented examination page loading with proper signal connections:
   - `closeExamination` → closes page
   - `examinationCompleted` → logs study ID and closes page
   - `errorOccurred` → logs error message

**Signal Flow**:
```
WorkListPageDelegate::onWorklistItemDoubleClicked()
    ↓
emit startExamination(entryId)
    ↓
MainWindowDelegate::onStartExamination(entryId)
    ↓
ExamPageBuilder::build() creates ExamPage + ExamPageDelegate
    ↓
MainWindow::loadPage(page)
    ↓
ExamPageDelegate initialization begins
```

## Current Architecture

### Workflow Context Flow

```
User Double-Clicks Worklist Entry
    ↓
WorkListPageDelegate::onWorklistItemDoubleClicked()
    ↓
Create ExaminationContext(worklistEntry)
    ↓
ContextManager::setWorkflowContext("Examination", examContext)
    ↓
┌────────────────────┴────────────────────┐
│ LOCAL Source       │ RIS Source         │
├────────────────────┼────────────────────┤
│ ViewSelectionDialog│ Skip               │
│ User selects:      │                    │
│ - Procedure        │                    │
│ - Views (multiple) │                    │
│                    │                    │
│ Update context:    │                    │
│ examContext→       │                    │
│   setProcedureId() │                    │
│   setViewIds()     │                    │
└────────────────────┴────────────────────┘
    ↓
emit startExamination(entryId)
    ↓
[Next: Open ExaminationPage]
```

### ExaminationContext Data

```cpp
ExaminationContext {
    // From worklist entry
    WorklistEntry m_entry;
    QMap<QString, QString> m_attributes;  // DICOM tags

    // Patient demographics (extracted from DICOM tags)
    QString patientName();
    QString patientId();
    QDate patientBirthDate();
    QString patientGender();
    QString accessionNumber();
    QString bodyPartExamined();

    // For LOCAL source: user selections
    int m_procedureId;           // Selected procedure
    QVector<int> m_viewIds;      // Selected views (will become series)

    // Workflow state
    bool m_isComplete;
    QDateTime m_timestamp;
}
```

## Next Steps

### Phase 1: ExamPage Integration (Next Priority)

#### Step 1: Create ExamPageDelegate Skeleton

**New File**: `Application/Delegate/ExamPageDelegate.h`

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
    QString name() const override;
    void attachDelegates(const QVector<QObject*>& delegates) override;

    // IPageAction
    void apply() override;   // Save intermediate state
    void accept() override;  // Complete examination
    void reject() override;  // Cancel examination

signals:
    void examinationCompleted(int studyId);
    void closeExamination();

private slots:
    void onPageLoaded();
    void loadExaminationContext();
    void loadPatientDemographics();
    void loadViews();

private:
    // Initialization
    void createStudy();
    void createSeries(int viewId);

    // VTK viewer
    void initializeVtkViewer();
    void displayImage(const QByteArray& imageData);

    ExamPage* ui;
    std::shared_ptr<WorklistRepository> m_worklistRepo;
    std::shared_ptr<DicomRepository> m_dicomRepo;
    std::shared_ptr<ScanProtocolRepository> m_scanProtocolRepo;
    std::shared_ptr<DeviceRepository> m_deviceRepo;
    std::weak_ptr<IContextManager> m_contextManager;

    // Current state
    WorklistEntry m_worklistEntry;
    int m_studyId = -1;
    QVector<int> m_seriesIds;
    QVector<View> m_views;
};
```

#### Step 2: Create ExamPageBuilder

**New File**: `Application/Builder/ExamPageBuilder.h`

```cpp
class ExamPageBuilder : public IWidgetDelegateBuilder<ExamPage, ExamPageDelegate> {
public:
    std::pair<ExamPage*, ExamPageDelegate*>
    build(const DelegateParameter& params,
          QWidget* parentWidget = nullptr,
          QObject* parentDelegate = nullptr) override;
};
```

**Implementation**: `Application/Builder/ExamPageBuilder.cpp`

```cpp
std::pair<ExamPage*, ExamPageDelegate*>
ExamPageBuilder::build(const DelegateParameter& params,
                       QWidget* parentWidget,
                       QObject* parentDelegate) {
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
```

#### Step 3: Wire ExaminationPage in MainWindowDelegate

**Modify**: `Application/Delegate/MainWindowDelegate.cpp`

Add method to load examination page:

```cpp
void MainWindowDelegate::onStartExamination(int worklistEntryId) {
    // Build examination page
    ExamPageBuilder builder;
    auto [page, delegate] = builder.build(m_params, nullptr, this);

    // Wire signals
    connect(delegate, &ExamPageDelegate::closeExamination,
            this, [this, page]() {
                m_mainWindow->closePage();
            });

    connect(delegate, &ExamPageDelegate::examinationCompleted,
            this, [this](int studyId) {
                qDebug() << "Examination completed, study ID:" << studyId;
                // Optionally refresh worklist
            });

    // Display page
    m_mainWindow->loadPage(page);
    m_mainWindow->finishLoadingPage();
}
```

Connect WorkListPageDelegate signal:

```cpp
// In MainWindowDelegate when building WorkListPage
connect(worklistDelegate, &WorkListPageDelegate::startExamination,
        this, &MainWindowDelegate::onStartExamination);
```

### Phase 2: VTK Image Viewer Integration

#### Step 1: Add VTK Widget to ExamPage.ui

Update the UI file to include a QVTKOpenGLNativeWidget for the main image display area.

#### Step 2: Initialize VTK in ExamPageDelegate

```cpp
void ExamPageDelegate::initializeVtkViewer() {
    // Get VTK widget from UI
    QVTKOpenGLNativeWidget* vtkWidget = ui->getVtkWidget();

    // Create render window
    m_renderWindow = vtkGenericOpenGLRenderWindow::New();
    vtkWidget->setRenderWindow(m_renderWindow);

    // Create renderer
    m_renderer = vtkRenderer::New();
    m_renderWindow->AddRenderer(m_renderer);

    // Create image actor for DICOM images
    m_imageActor = vtkImageActor::New();
    m_renderer->AddActor(m_imageActor);

    // Setup camera and interactor
    m_renderer->ResetCamera();

    // Set interactor style for medical imaging (window/level, zoom, pan)
    vtkSmartPointer<vtkInteractorStyleImage> style =
        vtkSmartPointer<vtkInteractorStyleImage>::New();
    vtkWidget->interactor()->SetInteractorStyle(style);
}
```

#### Step 3: Display DICOM Images

```cpp
void ExamPageDelegate::displayImage(const QByteArray& imageData) {
    // Convert raw image data to vtkImageData
    // Apply window/level
    // Update vtkImageActor
    // Render
    m_renderWindow->Render();
}
```

### Phase 3: Database Operations

#### Step 1: Study Creation

```cpp
void ExamPageDelegate::createStudy() {
    // Generate DICOM UID
    QString studyInstanceUid = DicomUidGenerator::generateStudyInstanceUid();

    // Extract patient info from ExaminationContext
    QString patientName = examContext->patientName();
    QString patientId = examContext->patientId();
    QString accessionNumber = examContext->accessionNumber();

    // Create study record
    Study study;
    study.studyInstanceUid = studyInstanceUid;
    study.patientId = getOrCreatePatientId(patientId, patientName);
    study.accessionNumber = accessionNumber;
    study.studyDate = QDate::currentDate();
    study.studyTime = QTime::currentTime();
    study.studyDescription = examContext->studyDescription();

    auto result = m_dicomRepo->createStudy(study);
    if (result.isSuccess) {
        m_studyId = result.value.id;

        // Update worklist status to IN_PROGRESS
        updateWorklistStatus(ProcedureStepStatus::IN_PROGRESS, studyInstanceUid);
    }
}
```

#### Step 2: Series Creation (One Per View)

```cpp
void ExamPageDelegate::createSeriesForViews() {
    QVector<int> viewIds = examContext->viewIds();

    for (int i = 0; i < viewIds.size(); ++i) {
        int viewId = viewIds[i];

        // Get view details
        auto viewResult = m_scanProtocolRepo->getViewById(viewId);
        if (!viewResult.isSuccess) continue;

        View view = viewResult.value;

        // Generate series UID
        QString seriesInstanceUid = DicomUidGenerator::generateSeriesInstanceUid();

        // Create series record
        Series series;
        series.studyId = m_studyId;
        series.seriesInstanceUid = seriesInstanceUid;
        series.seriesNumber = i + 1;
        series.modality = "DX";
        series.seriesDescription = view.name;
        series.bodyPartExamined = view.bodyPart;
        series.patientPosition = view.patientPosition;
        series.viewPosition = view.viewPosition;
        series.imageLaterality = view.imageLaterality;

        auto result = m_dicomRepo->createSeries(series);
        if (result.isSuccess) {
            m_seriesIds.append(result.value.id);
        }
    }
}
```

#### Step 3: Worklist Status Update

```cpp
void ExamPageDelegate::updateWorklistStatus(
    ProcedureStepStatus status,
    const QString& studyInstanceUid = QString()) {

    int worklistEntryId = examContext->worklistEntryId();

    auto entry = m_worklistRepo->getWorklistEntryById(worklistEntryId);
    if (entry.isSuccess) {
        entry.value.status = status;
        if (!studyInstanceUid.isEmpty()) {
            entry.value.studyInstanceUid = studyInstanceUid;
        }
        m_worklistRepo->updateWorklistEntry(entry.value);
    }
}
```

## Files Created/Modified So Far

### Documentation Files
1. `Documents/07-Examination-Workflow-Design.md` - Complete workflow design
2. `Documents/08-Examination-Implementation-Progress.md` - This file (tracking progress)

### Phase 1 Completed Files

#### Created Files
1. ✅ `Application/Delegate/ExamPageDelegate.h` - Delegate header with all interfaces
2. ✅ `Application/Delegate/ExamPageDelegate.cpp` - Complete delegate implementation
3. ✅ `Application/Builder/ExamPageBuilder.h` - Builder header
4. ✅ `Application/Builder/ExamPageBuilder.cpp` - Builder implementation

#### Modified Files
1. ✅ `Application/Delegate/WorkListPageDelegate.cpp` - Added LOCAL vs RIS routing
2. ✅ `Core/Context/ExaminationContext.h` - Added procedure/view ID support
3. ✅ `Core/Context/ExaminationContext.cpp` - Implemented new methods
4. ✅ `Application/Delegate/MainWindowDelegate.h` - Added ExamPageDelegate member and slot
5. ✅ `Application/Delegate/MainWindowDelegate.cpp` - Wired examination page loading

### Phase 2 - Next Files to Modify
1. `View/Page/ExamPage.ui` - Add VTK widget placeholder
2. `View/Page/ExamPage.h` - Add VTK widget accessors
3. `View/Page/ExamPage.cpp` - Initialize VTK widget

## Testing Plan

### Unit Tests Needed
1. ExaminationContext procedure/view ID storage
2. WorkListPageDelegate source routing logic
3. Study/Series creation with correct DICOM UIDs
4. Worklist status transitions

### Integration Tests Needed
1. LOCAL worklist entry → ViewSelectionDialog → ExaminationPage
2. RIS worklist entry → ExaminationPage (skip dialog)
3. ExaminationContext flow through entire workflow
4. Database operations (study, series, images)
5. VTK image display

### Manual Testing Scenarios
1. Create LOCAL worklist entry, select views, start examination
2. Receive RIS worklist entry, start examination directly
3. Acquire images for multiple views/series
4. Verify DICOM hierarchy in database
5. Verify worklist status updates correctly

## Next Actions

### ✅ Phase 1 Completed: MVD Infrastructure
1. ✅ **Completed**: Created ExamPageDelegate with full MVD pattern
2. ✅ **Completed**: Created ExamPageBuilder following builder pattern
3. ✅ **Completed**: Wired ExaminationPage in MainWindowDelegate
4. ✅ **Completed**: Connected all signals between MainWindow → ExamPage workflow

### Phase 2: VTK Image Viewer Integration
1. **Next**: Add VTK widget to ExamPage.ui (QVTKOpenGLNativeWidget)
2. **Then**: Add VTK widget accessors to ExamPage.h/cpp
3. **After**: Implement VTK initialization in ExamPageDelegate

### Phase 3: Database Operations
1. Implement actual study creation logic
2. Implement series creation for each view
3. Implement image storage with DICOM metadata
4. Implement worklist status updates

### Phase 4: Equipment Integration
1. Implement generator configuration
2. Implement detector configuration
3. Wire image acquisition hardware callbacks

## Questions/Decisions Needed

1. **Image Acquisition**: How is raw image data received from detector?
   - Via hardware SDK callback?
   - Via file watcher?
   - Via network socket?

2. **DICOM UID Generation**: Use existing `DicomUidGenerator` or implement new one?

3. **Patient Record**:
   - Create new patient if not exists?
   - Match on PatientID + IssuerOfPatientID?

4. **Thumbnail Display**:
   - Use VTK for thumbnails?
   - Use Qt pixmaps?
   - Lazy load or pre-render?

5. **Equipment Control**:
   - Integrate with existing Device module?
   - Mock for now and implement later?

---

**Related Documentation**:
- [Examination Workflow Design](07-Examination-Workflow-Design.md)
- [Model-View-Delegate Pattern](03-Model-View-Delegate-Pattern.md)
- [Builder Pattern](04-Builder-Pattern.md)
- [Application Workflows](06-Application-Workflows.md)
- [Database Schema](02-Database-Schema.md)
