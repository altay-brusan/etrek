#include "ViewSelectionDialog.h"
#include "ui_ViewSelectionDialog.h"
#include "Procedure.h"
#include "View.h"
#include "BodyPart.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPixmap>
#include <QGroupBox>
#include <QFrame>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>

// VTK includes
#include <QVTKOpenGLNativeWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageActor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <QFile>

ViewSelectionDialog::ViewSelectionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ViewSelectionDialog)
{
    ui->setupUi(this);
    setupConnections();
    // VTK initialization can be added here if needed for 3D visualization
    // initializeVTK();
}

ViewSelectionDialog::~ViewSelectionDialog()
{
    delete ui;
}

void ViewSelectionDialog::setupConnections()
{
    connect(ui->procedureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ViewSelectionDialog::onProcedureChanged);
    connect(ui->startExaminationButton, &QPushButton::clicked,
            this, &ViewSelectionDialog::onStartExaminationClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
}

void ViewSelectionDialog::setPatientInfo(const QString& patientName, const QString& patientId,
                                         const QString& bodyPart, const QString& accessionNo)
{
    ui->patientNameValue->setText(patientName);
    ui->patientIdValue->setText(patientId);
    ui->bodyPartValue->setText(bodyPart);
    ui->accessionNoValue->setText(accessionNo);
}

void ViewSelectionDialog::setProcedures(const QVector<Etrek::ScanProtocol::Data::Entity::Procedure>& procedures)
{
    m_procedures = procedures;

    ui->procedureComboBox->clear();

    if (procedures.isEmpty()) {
        ui->procedureComboBox->addItem("No procedures available");
        ui->procedureComboBox->setEnabled(false);
        ui->startExaminationButton->setEnabled(false);
        return;
    }

    ui->procedureComboBox->setEnabled(true);

    for (const auto& procedure : procedures) {
        ui->procedureComboBox->addItem(procedure.Name, procedure.Id);
    }

    // Automatically load views for the first procedure
    if (!procedures.isEmpty()) {
        onProcedureChanged(0);
    }
}

void ViewSelectionDialog::onProcedureChanged(int index)
{
    if (index < 0 || index >= m_procedures.size()) {
        clearViewsLayout();
        return;
    }

    m_selectedProcedureId = m_procedures[index].Id;
    loadViewsForProcedure(index);
    emit procedureSelected(m_selectedProcedureId);
}

void ViewSelectionDialog::loadViewsForProcedure(int procedureIndex)
{
    clearViewsLayout();

    if (procedureIndex < 0 || procedureIndex >= m_procedures.size())
        return;

    const auto& procedure = m_procedures[procedureIndex];

    if (procedure.Views.isEmpty()) {
        QLabel* noViewsLabel = new QLabel("No views available for this procedure");
        noViewsLabel->setAlignment(Qt::AlignCenter);
        noViewsLabel->setStyleSheet("color: rgb(208, 208, 208); font-size: 12pt;");
        ui->viewsGridLayout->addWidget(noViewsLabel, 0, 0);
        updateStartButtonState();
        return;
    }

    // Create a container widget to hold the grid
    QWidget* containerWidget = new QWidget();
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    // Create a new grid layout for this container
    QGridLayout* gridLayout = new QGridLayout(containerWidget);
    gridLayout->setHorizontalSpacing(20);
    gridLayout->setVerticalSpacing(20);
    gridLayout->setContentsMargins(15, 15, 15, 15);
    gridLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // Display views in a grid layout (3 columns)
    int row = 0;
    int col = 0;
    const int maxColumns = 3;

    for (const auto& view : procedure.Views) {
        QWidget* viewWidget = createViewWidget(view);
        gridLayout->addWidget(viewWidget, row, col, Qt::AlignTop | Qt::AlignLeft);

        col++;
        if (col >= maxColumns) {
            col = 0;
            row++;
        }
    }
    
    // Add the container to the main layout (which is in the scroll area)
    ui->viewsGridLayout->addWidget(containerWidget, 0, 0, Qt::AlignTop | Qt::AlignLeft);

    updateStartButtonState();
}

QWidget* ViewSelectionDialog::createViewWidget(const Etrek::ScanProtocol::Data::Entity::View& view)
{
    // Create a frame for each view
    QFrame* frame = new QFrame();
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Raised);
    frame->setLineWidth(2);
    frame->setStyleSheet("QFrame { border: 2px solid rgb(120, 120, 120); border-radius: 5px; padding: 10px; }");
    frame->setFixedSize(320, 300);
    frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->setSpacing(8);
    layout->setContentsMargins(10, 10, 10, 10);

    // View name with checkbox in horizontal layout
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(5);
    
    QCheckBox* checkbox = new QCheckBox();
    checkbox->setMinimumSize(20, 20);
    checkbox->setMaximumSize(20, 20);
    m_viewCheckboxes[view.Id] = checkbox;

    connect(checkbox, &QCheckBox::toggled, this, [this, viewId = view.Id](bool checked) {
        onViewCheckboxToggled(viewId, checked);
    });
    
    QLabel* nameLabel = new QLabel(view.Name);
    nameLabel->setFont(QFont("Arial", 11, QFont::Bold));
    nameLabel->setStyleSheet("color: rgb(208, 208, 208);");
    nameLabel->setWordWrap(true);
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    headerLayout->addWidget(checkbox);
    headerLayout->addWidget(nameLabel);
    headerLayout->addStretch();
    
    layout->addLayout(headerLayout);

    // VTK widget for 2D image display or text fallback
    QVTKOpenGLNativeWidget* vtkWidget = new QVTKOpenGLNativeWidget();
    vtkWidget->setMinimumSize(280, 160);
    vtkWidget->setMaximumSize(280, 160);
    vtkWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    // Create VTK render window and renderer
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkWidget->setRenderWindow(renderWindow);
    
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(0.235, 0.235, 0.235); // rgb(60, 60, 60)
    renderWindow->AddRenderer(renderer);

    // Try to load and display image
    bool imageLoaded = false;
    if (!view.IconFileLocation.isEmpty() && QFile::exists(view.IconFileLocation)) {
        // Load image using VTK
        vtkNew<vtkImageReader2Factory> readerFactory;
        vtkSmartPointer<vtkImageReader2> imageReader;
        imageReader.TakeReference(readerFactory->CreateImageReader2(view.IconFileLocation.toStdString().c_str()));
        
        if (imageReader) {
            imageReader->SetFileName(view.IconFileLocation.toStdString().c_str());
            imageReader->Update();
            
            // Create image actor for 2D display
            vtkNew<vtkImageActor> imageActor;
            imageActor->SetInputData(imageReader->GetOutput());
            
            renderer->AddActor(imageActor);
            renderer->ResetCamera();
            imageLoaded = true;
        }
    }
    
    // If no image, display text using VTK
    if (!imageLoaded) {
        vtkNew<vtkTextActor> textActor;
        QString displayText = view.Name;
        if (view.ViewPosition.has_value()) {
            displayText += QString("\n%1").arg(
                Etrek::ScanProtocol::ScanProtocolUtil::toString(view.ViewPosition.value()));
        }
        
        textActor->SetInput(displayText.toStdString().c_str());
        textActor->GetTextProperty()->SetFontSize(16);
        textActor->GetTextProperty()->SetColor(0.588, 0.588, 0.588); // rgb(150, 150, 150)
        textActor->GetTextProperty()->SetJustificationToCentered();
        textActor->GetTextProperty()->SetVerticalJustificationToCentered();
        textActor->SetPosition(140, 80); // Center of widget
        
        renderer->AddActor2D(textActor);
    }
    
    layout->addWidget(vtkWidget);

    // View details
    if (!view.Description.isEmpty()) {
        QLabel* descLabel = new QLabel(view.Description);
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("color: rgb(180, 180, 180); font-size: 9pt;");
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setMaximumHeight(60);
        descLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        layout->addWidget(descLabel);
    }
    
    layout->addStretch();

    return frame;
}

void ViewSelectionDialog::clearViewsLayout()
{
    // Remove all widgets from the grid layout
    QLayoutItem* item;
    while ((item = ui->viewsGridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    m_viewCheckboxes.clear();
    updateStartButtonState();
}

void ViewSelectionDialog::onViewCheckboxToggled(int viewId, bool checked)
{
    Q_UNUSED(viewId);
    Q_UNUSED(checked);
    updateStartButtonState();
    emit viewSelectionChanged();
}

void ViewSelectionDialog::updateStartButtonState()
{
    // Enable start button only if at least one view is selected
    bool hasSelection = false;
    for (auto it = m_viewCheckboxes.constBegin(); it != m_viewCheckboxes.constEnd(); ++it) {
        if (it.value()->isChecked()) {
            hasSelection = true;
            break;
        }
    }

    ui->startExaminationButton->setEnabled(hasSelection);
}

void ViewSelectionDialog::onStartExaminationClicked()
{
    emit startExamination();
    accept();
}

int ViewSelectionDialog::getSelectedProcedureId() const
{
    return m_selectedProcedureId;
}

QVector<int> ViewSelectionDialog::getSelectedViewIds() const
{
    QVector<int> selectedIds;

    for (auto it = m_viewCheckboxes.constBegin(); it != m_viewCheckboxes.constEnd(); ++it) {
        if (it.value()->isChecked()) {
            selectedIds.append(it.key());
        }
    }

    return selectedIds;
}

void ViewSelectionDialog::initializeVTK()
{
    // This method can be used to initialize VTK for 3D visualization
    // For now, it's a placeholder for future enhancement

    // Example VTK initialization (commented out for now):
    /*
    m_vtkWidget = new QVTKOpenGLNativeWidget(this);
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    m_vtkWidget->setRenderWindow(renderWindow);

    m_renderer = vtkRenderer::New();
    m_vtkWidget->renderWindow()->AddRenderer(m_renderer);
    m_renderer->SetBackground(0.1, 0.1, 0.1); // Dark background

    // Add VTK widget to a layout for 3D body visualization
    */
}
