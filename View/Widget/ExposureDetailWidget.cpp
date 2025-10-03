#include "ExposureDetailWidget.h"
#include "ui_ExposureDetailWidget.h"
#include "ExposureDetailWidget.h"
#include "ui_ExposureDetailWidget.h"
#include "TechniqueParameter.h"
#include <QComboBox>

using namespace Etrek::ScanProtocol;
using namespace Etrek::ScanProtocol::Data::Entity;

ExposureDetailWidget::ExposureDetailWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::ExposureDetailWidget)
{
    ui->setupUi(this);
    initStyles();
    clear(); // start in default state
}

ExposureDetailWidget::~ExposureDetailWidget() { delete ui; }

void ExposureDetailWidget::initStyles()
{
    const char* lineCss =
        "QLineEdit { border:1px solid rgb(120,120,120); border-radius:4px; "
        " background:rgb(74,74,74); min-height:20px; padding:2px 6px; } "
        "QLineEdit:focus { border-color: rgb(160,160,160); }";

    const char* spinCss =
        "QSpinBox { border:1px solid rgb(120,120,120); border-radius:4px; "
        " background:rgb(74,74,74); min-height:20px; padding:2px 6px; } "
        "QSpinBox:focus { border-color: rgb(160,160,160); }";

    const char* comboCss =
        "QComboBox { border:1px solid rgb(120,120,120); border-radius:4px; "
        " background:rgb(74,74,74); min-height:20px; padding:2px 28px 2px 6px; } "
        "QComboBox:focus { border-color: rgb(160,160,160); } "
        "QComboBox::drop-down { width:26px; border-left:1px solid rgb(120,120,120); margin:0; } "
        "QComboBox::down-arrow { image: url(:/Images/Asset/Icon/chevron-down.svg); width:12px; height:12px; } "
        "QComboBox QAbstractItemView { border:1px solid rgb(120,120,120); background:rgb(74,74,74); "
        " selection-background-color: rgb(92,92,92); }";

    ui->kvpSpinBox->setStyleSheet(spinCss);
    ui->maSpinBox->setStyleSheet(spinCss);
    ui->msSpinBox->setStyleSheet(spinCss);
    ui->gridTypeComboBox->setStyleSheet(comboCss);
    ui->gridRatioLineEdit->setStyleSheet(lineCss);
    ui->gridSpeedLineEdit->setStyleSheet(lineCss);
    ui->sidMinSpinBox->setStyleSheet(spinCss);
    ui->sidMaxSpinBox->setStyleSheet(spinCss);
    ui->sidLineEdit->setStyleSheet(lineCss);
}

// ExposureDetailWidget.cpp (append)

using namespace Etrek::ScanProtocol;
using namespace Etrek::ScanProtocol::Data::Entity;

void ExposureDetailWidget::load(const TechniqueParameter& tp)
{
    ui->kvpSpinBox->setValue(tp.Kvp);
    ui->maSpinBox->setValue(tp.Ma);
    ui->msSpinBox->setValue(tp.Ms);

    // Grid
    if (tp.GridType.has_value()) {
        ui->gridTypeComboBox->setCurrentText(ScanProtocolUtil::toString(*tp.GridType));
    }
    else {
        ui->gridTypeComboBox->setCurrentIndex(0); // empty entry
    }
    ui->gridRatioLineEdit->setText(tp.GridRatio);
    ui->gridSpeedLineEdit->setText(tp.GridSpeed);

    ui->sidMinSpinBox->setValue(int(tp.SIDMin));
    ui->sidMaxSpinBox->setValue(int(tp.SIDMax));
    ui->sidLineEdit->setText(
        (tp.SIDMin > 0 && tp.SIDMax > 0) ? QString("%1-%2").arg(int(tp.SIDMin)).arg(int(tp.SIDMax))
        : QString());
}

void ExposureDetailWidget::clear()
{
    ui->kvpSpinBox->setValue(0);
    ui->maSpinBox->setValue(0);
    ui->msSpinBox->setValue(0);
    ui->gridTypeComboBox->setCurrentIndex(0); // assume first is empty
    ui->gridRatioLineEdit->clear();
    ui->gridSpeedLineEdit->clear();
    ui->sidMinSpinBox->setValue(0);
    ui->sidMaxSpinBox->setValue(0);
    ui->sidLineEdit->clear();
}

std::optional<TechniqueParameter>
ExposureDetailWidget::snapshot(const BodyPart& bp,
    TechniqueProfile profile,
    BodySize size,
    ExposureStyle exposureStyle,
    const QString& aecField,
    int focalSpot,
    int aecDensity) const
{
    TechniqueParameter tp;
    tp.BodyPart = bp;
    tp.Profile = profile;
    tp.Size = size;

    tp.Kvp = ui->kvpSpinBox->value();
    tp.Ma = ui->maSpinBox->value();
    tp.Ms = ui->msSpinBox->value();

    const QString gridStr = ui->gridTypeComboBox->currentText().trimmed();
    if (gridStr.isEmpty())
        tp.GridType.reset();
    else
        tp.GridType = ScanProtocolUtil::parseGridType(gridStr);

    tp.GridRatio = ui->gridRatioLineEdit->text().trimmed();
    tp.GridSpeed = ui->gridSpeedLineEdit->text().trimmed();

    tp.SIDMin = ui->sidMinSpinBox->value();
    tp.SIDMax = ui->sidMaxSpinBox->value();

    // from the top controls
    tp.ExposureStyle = exposureStyle;
    tp.AecFields = aecField;
    tp.FocalSpotSize = focalSpot;
    tp.AecDensity = aecDensity;

    // Emit "empty" if nothing meaningful is set
    const bool allEmpty =
        tp.Kvp == 0 && tp.Ma == 0 && tp.Ms == 0 &&
        !tp.GridType.has_value() &&
        tp.GridRatio.isEmpty() && tp.GridSpeed.isEmpty() &&
        tp.SIDMin == 0 && tp.SIDMax == 0 &&
        tp.AecFields.isEmpty() && tp.FocalSpotSize == 0 && tp.AecDensity == 0;

    if (allEmpty)
        return std::nullopt;

    return tp;
}
