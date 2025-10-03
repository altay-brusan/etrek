#include "ExamPage.h"
#include "ExposureControlWidget.h"
#include "BodySizeWidget.h"
#include "EndExposureControlWidget.h"
#include "AecControlWidget.h"
#include "FocalSpotControlWidget.h"
#include <QVBoxLayout>
#include "ui_ExamPage.h"
#include "GeneratorControlWidget.h"
#include "DetectorControlWidget.h"
#include "BuckyControlWidget.h"
//#include "ThumbnailImageListlWidget.h"
#include "StudyControlWidget.h"
#include "ExamTitleWidget.h"
#include "ExposureApplicationControlWidget.h"

ExamPage::ExamPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExamPage)
{
    ui->setupUi(this);
    // Create instance of your custom widget
    ExposureControlWidget *exposureWidget = new ExposureControlWidget(this);
    BodySizeWidget *bodySizeWidget = new BodySizeWidget(this);
    EndExposureControlWidget *endExposureControlWidget = new EndExposureControlWidget(this);
    AecControlWidget *aecControlWidget = new AecControlWidget(this);
    FocalPointControlWidget *focalPointControlWidget = new FocalPointControlWidget(this);
    GeneratorControlWidget *generatorControlWidget = new GeneratorControlWidget(this);
    DetectorControlWidget *detectorControlWidget = new DetectorControlWidget(this);
    BuckyControlWidget *buckyControlWidget = new BuckyControlWidget(this);
    //ThumbnailImageListlWidget *thumbnailImageListlWidget = new ThumbnailImageListlWidget(this);
    StudyControlWidget *studyControlWidget = new StudyControlWidget(this);
    ExamTitleWidget *examTitleWidget = new ExamTitleWidget(this);
    ExposureApplicationControlWidget *exposureApplicationControlWidget = new ExposureApplicationControlWidget(this);


    QVBoxLayout *layout = new QVBoxLayout(ui->studyPlaceholder);
    layout->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout->addWidget(studyControlWidget);

    //Put it into the placeholder (replace or embed it)
    QVBoxLayout *layout1 = new QVBoxLayout(ui->exposurePlaceholder);
    layout1->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout1->addWidget(exposureWidget);


     QVBoxLayout *layout2 = new QVBoxLayout(ui->bodySizePlaceholder);
    layout2->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout2->addWidget(bodySizeWidget);


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
    layout10->addWidget(examTitleWidget);

    QVBoxLayout *layout11 = new QVBoxLayout(ui->applicationControlGroupBox);
    layout11->setContentsMargins(0, 0, 0, 0); // Optional: removes spacing
    layout11->addWidget(exposureApplicationControlWidget);


}

ExamPage::~ExamPage()
{
    delete ui;
}
