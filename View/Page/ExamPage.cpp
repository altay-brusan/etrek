#include "ExamPage.h"
#include "ExposureControlWidget.h"
#include "BodySizeWidget.h"
#include "EndExposureControlWidget.h"
#include "AecControlWidget.h"
#include "FocalSpotControlWidget.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QGroupBox>
#include "ui_ExamPage.h"
#include "GeneratorControlWidget.h"
#include "DetectorControlWidget.h"
#include "BuckyControlWidget.h"
//#include "ThumbnailImageListlWidget.h"
#include "StudyControlWidget.h"
#include "ExamTitleWidget.h"
#include "ExposureApplicationControlWidget.h"
#include <QVTKOpenGLNativeWidget.h>

ExamPage::ExamPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExamPage)
{
    ui->setupUi(this);
    // Create instance of your custom widget
    m_exposureControlWidget = new ExposureControlWidget(this);
    m_bodySizeWidget = new BodySizeWidget(this);
    EndExposureControlWidget *endExposureControlWidget = new EndExposureControlWidget(this);
    AecControlWidget *aecControlWidget = new AecControlWidget(this);
    FocalPointControlWidget *focalPointControlWidget = new FocalPointControlWidget(this);
    GeneratorControlWidget *generatorControlWidget = new GeneratorControlWidget(this);
    DetectorControlWidget *detectorControlWidget = new DetectorControlWidget(this);
    BuckyControlWidget *buckyControlWidget = new BuckyControlWidget(this);
    //ThumbnailImageListlWidget *thumbnailImageListlWidget = new ThumbnailImageListlWidget(this);
    m_studyControlWidget = new StudyControlWidget(this);
    m_examTitleWidget = new ExamTitleWidget(this);
    m_exposureApplicationControlWidget = new ExposureApplicationControlWidget(this);


    QVBoxLayout *layout = new QVBoxLayout(ui->studyPlaceholder);
    layout->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout->addWidget(m_studyControlWidget);

    //Put it into the placeholder (replace or embed it)
    QVBoxLayout *layout1 = new QVBoxLayout(ui->exposurePlaceholder);
    layout1->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout1->addWidget(m_exposureControlWidget);


     QVBoxLayout *layout2 = new QVBoxLayout(ui->bodySizePlaceholder);
    layout2->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout2->addWidget(m_bodySizeWidget);


    QVBoxLayout *layout3 = new QVBoxLayout(ui->aecControlPlaceholder);
    layout3->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout3->addWidget(aecControlWidget);


    QVBoxLayout *layout4 = new QVBoxLayout(ui->endExposurePlaceholder);
    layout4->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout4->addWidget(endExposureControlWidget);

    QVBoxLayout *layout5 = new QVBoxLayout(ui->focalSpotPlaceholder);
    layout5->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout5->addWidget(focalPointControlWidget);

    QVBoxLayout *layout6 = new QVBoxLayout(ui->generatorControlPlaceholder);
    layout6->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout6->addWidget(generatorControlWidget);

    QVBoxLayout *layout7 = new QVBoxLayout(ui->firstDetectorPlaceholder);
    layout7->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout7->addWidget(detectorControlWidget);

    QVBoxLayout *layout8 = new QVBoxLayout(ui->buckyControlPlaceholder);
    layout8->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout8->addWidget(buckyControlWidget);

    QVBoxLayout *layout9 = new QVBoxLayout(ui->thumbnailImageListPlaceholder);
    layout9->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    //layout9->addWidget(thumbnailImageListlWidget);

    QVBoxLayout *layout10 = new QVBoxLayout(ui->titlePlaceholder);
    layout10->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout10->addWidget(m_examTitleWidget);

    QVBoxLayout *layout11 = new QVBoxLayout(ui->applicationControlGroupBox);
    layout11->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout11->addWidget(m_exposureApplicationControlWidget);


}

ExamPage::~ExamPage()
{
    delete ui;
}

// --- Accessor Methods ---

QVTKOpenGLNativeWidget* ExamPage::getVtkImageViewer() const
{
    return ui->vtkImageViewer;
}

ExamTitleWidget* ExamPage::getExamTitleWidget() const
{
    return m_examTitleWidget;
}

StudyControlWidget* ExamPage::getStudyControlWidget() const
{
    return m_studyControlWidget;
}

BodySizeWidget* ExamPage::getBodySizeWidget() const
{
    return m_bodySizeWidget;
}

ExposureApplicationControlWidget* ExamPage::getExposureApplicationControlWidget() const
{
    return m_exposureApplicationControlWidget;
}

ExposureControlWidget* ExamPage::getExposureControlWidget() const
{
    return m_exposureControlWidget;
}

QFrame* ExamPage::getTitlePlaceholder() const
{
    return ui->titlePlaceholder;
}

QFrame* ExamPage::getThumbnailPlaceholder() const
{
    return ui->thumbnailImageListPlaceholder;
}

QGroupBox* ExamPage::getStudyPlaceholder() const
{
    return ui->studyPlaceholder;
}

QGroupBox* ExamPage::getExposurePlaceholder() const
{
    return ui->exposurePlaceholder;
}

QGroupBox* ExamPage::getBodySizePlaceholder() const
{
    return ui->bodySizePlaceholder;
}

QGroupBox* ExamPage::getGeneratorControlPlaceholder() const
{
    return ui->generatorControlPlaceholder;
}
