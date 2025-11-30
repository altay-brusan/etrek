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

    // Display views in a grid layout (3 columns)
    int row = 0;
    int col = 0;
    const int maxColumns = 3;

    for (const auto& view : procedure.Views) {
        QWidget* viewWidget = createViewWidget(view);
        ui->viewsGridLayout->addWidget(viewWidget, row, col);

        col++;
        if (col >= maxColumns) {
            col = 0;
            row++;
        }
    }

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
    frame->setMinimumSize(280, 200);
    frame->setMaximumSize(350, 250);

    QVBoxLayout* layout = new QVBoxLayout(frame);

    // View name checkbox
    QCheckBox* checkbox = new QCheckBox(view.Name);
    checkbox->setFont(QFont("Arial", 11, QFont::Bold));
    checkbox->setStyleSheet("color: rgb(208, 208, 208);");
    m_viewCheckboxes[view.Id] = checkbox;

    connect(checkbox, &QCheckBox::toggled, this, [this, viewId = view.Id](bool checked) {
        onViewCheckboxToggled(viewId, checked);
    });

    layout->addWidget(checkbox);

    // View icon/image placeholder
    QLabel* iconLabel = new QLabel();
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setMinimumSize(200, 120);
    iconLabel->setMaximumSize(300, 180);
    iconLabel->setStyleSheet("background-color: rgb(60, 60, 60); border: 1px solid rgb(100, 100, 100);");

    // Try to load icon from file, otherwise show placeholder
    if (!view.IconFileLocation.isEmpty()) {
        QPixmap pixmap(view.IconFileLocation);
        if (!pixmap.isNull()) {
            iconLabel->setPixmap(pixmap.scaled(iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            // Dummy placeholder text
            iconLabel->setText(QString("Icon\n%1").arg(view.Name));
            iconLabel->setAlignment(Qt::AlignCenter);
            iconLabel->setStyleSheet("background-color: rgb(60, 60, 60); color: rgb(150, 150, 150); "
                                    "border: 1px solid rgb(100, 100, 100); font-size: 10pt;");
        }
    } else {
        // Dummy placeholder with view details
        QString placeholderText = QString("%1\n%2")
            .arg(view.Name)
            .arg(view.ViewPosition.has_value() ?
                 Etrek::ScanProtocol::ScanProtocolUtil::toString(view.ViewPosition.value()) : "");
        iconLabel->setText(placeholderText);
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setStyleSheet("background-color: rgb(60, 60, 60); color: rgb(150, 150, 150); "
                                "border: 1px solid rgb(100, 100, 100); font-size: 9pt;");
    }

    layout->addWidget(iconLabel);

    // View details
    if (!view.Description.isEmpty()) {
        QLabel* descLabel = new QLabel(view.Description);
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("color: rgb(180, 180, 180); font-size: 9pt;");
        descLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(descLabel);
    }

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
