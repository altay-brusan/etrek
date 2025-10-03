#include "OrientationDetailWidget.h"
#include "ui_OrientationDetailWidget.h"
#include "ExposureDetailWidget.h"
#include <QVBoxLayout>

using namespace Etrek::ScanProtocol;
using namespace Etrek::ScanProtocol::Data::Entity;

OrientationDetailWidget::OrientationDetailWidget(TechniqueProfile profile,
    const QVector<TechniqueParameter>* data,
    QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::OrientationDetailWidget)
    , m_profile(profile)
    , m_all(data)
{
    ui->setupUi(this);
    initStyles();

    // Create the 4 size widgets once
    m_large = new ExposureDetailWidget(this);
    m_medium = new ExposureDetailWidget(this);
    m_small = new ExposureDetailWidget(this);
    m_child = new ExposureDetailWidget(this);

    auto* l0 = new QVBoxLayout(ui->largPatientExposureDetailPlaceholder);
    l0->setContentsMargins(0, 0, 0, 0);
    l0->addWidget(m_large);

    auto* l1 = new QVBoxLayout(ui->mediumPatientSizeExposureDetailPlaceholder);
    l1->setContentsMargins(0, 0, 0, 0);
    l1->addWidget(m_medium);

    auto* l2 = new QVBoxLayout(ui->smallPatientSizeExposureDetailPlaceholder);
    l2->setContentsMargins(0, 0, 0, 0);
    l2->addWidget(m_small);

    auto* l3 = new QVBoxLayout(ui->childPatientExposureDetailPlaceholder);
    l3->setContentsMargins(0, 0, 0, 0);
    l3->addWidget(m_child);

    // start all tabs cleared
    m_large->clear();
    m_medium->clear();
    m_small->clear();
    m_child->clear();
}

OrientationDetailWidget::~OrientationDetailWidget() { delete ui; }

void OrientationDetailWidget::initStyles()
{
    const char* tabCss =
        "QTabWidget#detailsTabWidget::pane { border:2px solid rgb(120,120,120); top:-1px; } "
        "QTabWidget#detailsTabWidget QTabBar::tab { background-color:rgb(83,83,83); color:rgb(208,208,208); "
        " padding:6px; border:none; } "
        "QTabWidget#detailsTabWidget QTabBar::tab:selected { background-color:rgb(60,60,60); color:white; }";

    const char* spinCss =
        "QSpinBox { border:1px solid rgb(120,120,120); border-radius:4px; background:rgb(74,74,74); "
        " min-height:28px; padding:2px 6px; } QSpinBox:focus { border-color: rgb(160,160,160); }";

    const char* comboCss =
        "QComboBox { border:1px solid rgb(120,120,120); border-radius:4px; background:rgb(74,74,74); "
        " min-height:28px; padding:2px 28px 2px 6px; } "
        "QComboBox:focus { border-color: rgb(160,160,160); } "
        "QComboBox::drop-down { width:26px; border-left:1px solid rgb(120,120,120); margin:0; } "
        "QComboBox::down-arrow { image: url(:/Images/Asset/Icon/chevron-down.svg); width:12px; height:12px; } "
        "QComboBox QAbstractItemView { border:1px solid rgb(120,120,120); background:rgb(74,74,74); "
        " selection-background-color: rgb(92,92,92); }";

    ui->detailsTabWidget->setStyleSheet(tabCss);
    ui->exposureModeComboBox->setStyleSheet(comboCss);
    ui->fieldComboBox->setStyleSheet(comboCss);
    ui->focusComboBox->setStyleSheet(comboCss);
    ui->densitySpinBox->setStyleSheet(spinCss);
}

void OrientationDetailWidget::onBodyPartChanged(int bodyPartId)
{
    // 1) Filter by selected body part + this profile
    QMap<BodySize, TechniqueParameter> bySize;
    if (m_all) {
        for (const auto& tp : *m_all) {
            if (tp.BodyPart.Id == bodyPartId && tp.Profile == m_profile) {
                bySize.insert(tp.Size, tp);
            }
        }
    }

    // 2) Push to size tabs (clear when absent)
    if (bySize.contains(BodySize::Fat))        m_large->load(bySize[BodySize::Fat]);       else m_large->clear();
    if (bySize.contains(BodySize::Medium))     m_medium->load(bySize[BodySize::Medium]);   else m_medium->clear();
    if (bySize.contains(BodySize::Thin))       m_small->load(bySize[BodySize::Thin]);      else m_small->clear();
    if (bySize.contains(BodySize::Paediatric)) m_child->load(bySize[BodySize::Paediatric]); else m_child->clear();

    // 3) Top controls from representative or defaults
    updateTopAecControls(bySize);
}

void OrientationDetailWidget::updateTopAecControls(
    const QMap<BodySize, TechniqueParameter>& bySize) const
{
    ui->exposureModeComboBox->blockSignals(true);
    ui->fieldComboBox->blockSignals(true);
    ui->focusComboBox->blockSignals(true);

    ui->exposureModeComboBox->clear();
    for (const auto& s : ScanProtocolUtil::allExposureStyleStrings())
        ui->exposureModeComboBox->addItem(s);

    const TechniqueParameter* rep = pickRepresentative(bySize);
    if (rep) {
        const QString styleStr = ScanProtocolUtil::toString(rep->ExposureStyle);
        const int idx = ui->exposureModeComboBox->findText(styleStr, Qt::MatchFixedString);
        if (idx >= 0) ui->exposureModeComboBox->setCurrentIndex(idx); else ui->exposureModeComboBox->setCurrentIndex(-1);

        ui->fieldComboBox->clear();
        if (!rep->AecFields.isEmpty()) {
            ui->fieldComboBox->addItem(rep->AecFields);
            ui->fieldComboBox->setCurrentIndex(0);
        }
        else {
            ui->fieldComboBox->setCurrentIndex(-1);
        }

        ui->focusComboBox->clear();
        if (rep->FocalSpotSize > 0) {
            ui->focusComboBox->addItem(QString::number(rep->FocalSpotSize));
            ui->focusComboBox->setCurrentIndex(0);
        }
        else {
            ui->focusComboBox->setCurrentIndex(-1);
        }

        ui->densitySpinBox->setValue(rep->AecDensity);
    }
    else {
        ui->exposureModeComboBox->setCurrentIndex(-1);
        ui->fieldComboBox->clear();
        ui->focusComboBox->clear();
        ui->densitySpinBox->setValue(0);
    }

    ui->exposureModeComboBox->blockSignals(false);
    ui->fieldComboBox->blockSignals(false);
    ui->focusComboBox->blockSignals(false);
}

const TechniqueParameter* OrientationDetailWidget::pickRepresentative(
    const QMap<BodySize, TechniqueParameter>& bySize)
{
    auto tryKey = [&](BodySize k) -> const TechniqueParameter* {
        auto it = bySize.constFind(k);
        return (it != bySize.cend()) ? &it.value() : nullptr;
        };
    if (auto p = tryKey(BodySize::Medium))     return p;
    if (auto p = tryKey(BodySize::Fat))        return p;
    if (auto p = tryKey(BodySize::Thin))       return p;
    if (auto p = tryKey(BodySize::Paediatric)) return p;
    return nullptr;
}

// OrientationDetailWidget.cpp (append these defs)

QVector<TechniqueParameter>
OrientationDetailWidget::collectForBodyPart(const BodyPart& bp) const
{
    QVector<TechniqueParameter> out;
    out.reserve(4);

    // Top controls (apply to each size snapshot)
    const auto expoIdx = ui->exposureModeComboBox->currentIndex();
    const QString expoStr = (expoIdx >= 0)
        ? ui->exposureModeComboBox->currentText()
        : QString();
    const ExposureStyle exposureStyle = ScanProtocolUtil::parseExposureStyle(expoStr);

    const QString aecField = ui->fieldComboBox->currentText();
    const int     focalSpot = ui->focusComboBox->currentText().toInt(); // empty -> 0
    const int     density = ui->densitySpinBox->value();

    // helper to grab from each ExposureDetailWidget
    auto grab = [&](ExposureDetailWidget* edw, BodySize sz) {
        if (!edw) return;
        auto maybe = edw->snapshot(bp, m_profile, sz, exposureStyle, aecField, focalSpot, density);
        if (maybe.has_value())
            out.push_back(std::move(*maybe));
        };

    grab(m_large, BodySize::Fat);
    grab(m_medium, BodySize::Medium);
    grab(m_small, BodySize::Thin);
    grab(m_child, BodySize::Paediatric);

    return out;
}

