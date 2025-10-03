#include <QWidget>
#include <QStandardItem>
#include <QSet>
#include <QVBoxLayout>
#include <QAbstractItemView>   // for NoEditTriggers, SingleSelection
#include <algorithm>

#include "ViewConfigurationWidget.h"
#include "ui_ViewConfigurationWidget.h"
#include "ScanProtocolUtil.h"

// Keep namespaces local
namespace ESP = Etrek::ScanProtocol;
namespace ESPDE = Etrek::ScanProtocol::Data::Entity;
using ViewEntity = ESPDE::View;

// ---- helpers: stringify optionals/enums ----
template <typename E>
static QString optEnumToText(const std::optional<E>& v) {
    return v.has_value()
        ? ESP::ScanProtocolUtil::toString(v.value())
        : QString();
}

static QString optIntToText(const std::optional<int>& v) {
    return v.has_value() ? QString::number(v.value()) : QString();
}

// ---- ctor / dtor ----
ViewConfigurationWidget::ViewConfigurationWidget(const QVector<ViewEntity>& views, QWidget* parent)
    : QWidget(parent)
    , m_parameters(views)
    , ui(new Ui::ViewConfigurationWidget)
{
    ui->setupUi(this);
    setStile();

    // build left list and right-side static/populated combos
    buildViewList();
    buildBodyPartComboFromViews();
    buildAuxCombosFromViews();

    ui->viewListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->viewListView->setSelectionMode(QAbstractItemView::SingleSelection);

    // connect selection
    connect(ui->viewListView->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &ViewConfigurationWidget::onViewSelectionChanged);

    // select first row
    if (m_viewModel && m_viewModel->rowCount() > 0) {
        const QModelIndex idx = m_viewModel->index(0, 0);
        ui->viewListView->setCurrentIndex(idx);
        onViewSelectionChanged(idx, {});
    }
}

ViewConfigurationWidget::~ViewConfigurationWidget()
{
    delete ui;
}

// ---- list building ----
void ViewConfigurationWidget::buildViewList()
{
    if (!m_viewModel) {
        m_viewModel = new QStandardItemModel(this);
        ui->viewListView->setModel(m_viewModel);
    }
    else {
        m_viewModel->clear();
    }
    m_viewById.clear();

    // Sort: Region.DisplayOrder, Region.Name, BodyPart.Name, View.Name
    QVector<ViewEntity> sorted = m_parameters;
    std::sort(sorted.begin(), sorted.end(),
        [](const ViewEntity& a, const ViewEntity& b) {
            int aDisp = a.BodyPart.Region.DisplayOrder;
            int bDisp = b.BodyPart.Region.DisplayOrder;
            if (aDisp != bDisp) return aDisp < bDisp;

            int rn = QString::localeAwareCompare(a.BodyPart.Region.Name, b.BodyPart.Region.Name);
            if (rn != 0) return rn < 0;

            int bp = QString::localeAwareCompare(a.BodyPart.Name, b.BodyPart.Name);
            if (bp != 0) return bp < 0;

            return QString::localeAwareCompare(a.Name, b.Name) < 0;
        });

    m_viewModel->setColumnCount(1);
    m_viewModel->setHorizontalHeaderLabels({ tr("View") });

    for (const auto& v : sorted) {
        auto* item = new QStandardItem(QStringLiteral("%1 — %2")
            .arg(v.BodyPart.Name.trimmed(), v.Name.trimmed()));
        item->setEditable(false);
        item->setData(v.Id, ViewIdRole);
        m_viewModel->appendRow(item);
        m_viewById.insert(v.Id, v);
    }
}

// ---- body part and auxiliary combos ----
void ViewConfigurationWidget::buildBodyPartComboFromViews()
{
    QSet<QString> dedup; // id#name key
    QVector<ESPDE::BodyPart> parts; parts.reserve(m_parameters.size());

    for (const auto& v : m_parameters) {
        const ESPDE::BodyPart& bp = v.BodyPart;
        const QString key = QString::number(bp.Id) + u'#' + bp.Name.toLower().trimmed();
        if (dedup.contains(key)) continue;
        dedup.insert(key);
        parts.push_back(bp);
    }

    std::sort(parts.begin(), parts.end(),
        [](const ESPDE::BodyPart& a, const ESPDE::BodyPart& b) {
            if (a.Region.DisplayOrder != b.Region.DisplayOrder)
                return a.Region.DisplayOrder < b.Region.DisplayOrder;
            int rn = QString::localeAwareCompare(a.Region.Name, b.Region.Name);
            if (rn != 0) return rn < 0;
            return QString::localeAwareCompare(a.Name, b.Name) < 0;
        });

    auto* cb = ui->bodyPartComboBox;
    cb->clear();
    for (const auto& bp : parts) {
        cb->addItem(bp.Name, bp.Id); // display name, store id
    }
}

void ViewConfigurationWidget::buildAuxCombosFromViews()
{
    auto seed = [](QComboBox* cb, const QSet<QString>& values) {
        for (const QString& v : values) {
            const QString t = v.trimmed();
            if (t.isEmpty()) continue;
            if (ViewConfigurationWidget::findComboIndexByText(cb, t) < 0)
                cb->addItem(t);
        }
        };

    QSet<QString> processes, positionerPrograms, labelPositions, labelMarks;
    QSet<QString> viewPositions, projectionProfiles, orientationsRow, orientationsCol;
    QSet<QString> iconPaths;
    QSet<QString> collX, collY; // UI has X/Y; View.CollimatorSize is "WxH"
    QSet<QString> patientPositions;
    QSet<QString> imageLaterality;
    QSet<QString> rotations;

    for (const auto& v : m_parameters) {
        processes.insert(v.ImageProcessingAlgorithm);
        positionerPrograms.insert(v.PositionName);

        labelPositions.insert(optEnumToText(v.LabelPosition));
        labelMarks.insert(optEnumToText(v.LabelMark));

        viewPositions.insert(optEnumToText(v.ViewPosition));
        projectionProfiles.insert(ESP::ScanProtocolUtil::toString(v.ProjectionProfile));

        orientationsRow.insert(optEnumToText(v.PatientOrientationRow));
        orientationsCol.insert(optEnumToText(v.PatientOrientationCol));

        iconPaths.insert(v.IconFileLocation);

        if (!v.CollimatorSize.trimmed().isEmpty()) {
            const QStringList wh = v.CollimatorSize.split('x', Qt::SkipEmptyParts);
            if (wh.size() == 2) {
                collX.insert(wh.at(0).trimmed());
                collY.insert(wh.at(1).trimmed());
            }
        }

        patientPositions.insert(optEnumToText(v.PatientPosition));
        imageLaterality.insert(optEnumToText(v.ImageLaterality));
        rotations.insert(optIntToText(v.ImageRotate));
    }

    seed(ui->imageProcessComboBox, processes);
    seed(ui->positionerProgramComboBox, positionerPrograms);
    seed(ui->labelPositionComboBox, labelPositions);
    seed(ui->labelMarkComboBox, labelMarks);
    seed(ui->viewPositionComboBox, viewPositions);
    seed(ui->projectionProfileComboBox, projectionProfiles);
    seed(ui->patientOrientationRowComboBox, orientationsRow);
    seed(ui->patientOrientationColumnComboBox, orientationsCol);
    seed(ui->collimatorSizeXComboBox, collX);
    seed(ui->collimatorSizeYComboBox, collY);
    seed(ui->patientPositionComboBox, patientPositions);
    seed(ui->imageLateralityComboBox, imageLaterality);
    seed(ui->imageRotationComboBox, rotations);
}

// ---- selection -> form ----
void ViewConfigurationWidget::onViewSelectionChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
    if (!current.isValid()) return;
    const int vid = current.data(ViewIdRole).toInt();
    const auto it = m_viewById.constFind(vid);
    if (it == m_viewById.constEnd()) return;

    loadViewToForm(it.value());
    emit viewSelected(vid);
}

void ViewConfigurationWidget::loadViewToForm(const ViewEntity& v)
{
    ui->viewNameLineEdit->setText(v.Name);
    ui->viewDescriptionLineEdit->setText(v.Description);
    ui->iconLineEdit->setText(v.IconFileLocation);

    int bpIndex = ui->bodyPartComboBox->findData(v.BodyPart.Id);
    if (bpIndex < 0) bpIndex = findComboIndexByText(ui->bodyPartComboBox, v.BodyPart.Name);
    if (bpIndex >= 0) ui->bodyPartComboBox->setCurrentIndex(bpIndex);

    ViewConfigurationWidget::setComboByText(
        ui->patientPositionComboBox, optEnumToText(v.PatientPosition));
    ViewConfigurationWidget::setComboByText(
        ui->projectionProfileComboBox, ESP::ScanProtocolUtil::toString(v.ProjectionProfile));
    ViewConfigurationWidget::setComboByText(
        ui->patientOrientationRowComboBox, optEnumToText(v.PatientOrientationRow));
    ViewConfigurationWidget::setComboByText(
        ui->patientOrientationColumnComboBox, optEnumToText(v.PatientOrientationCol));
    ViewConfigurationWidget::setComboByText(
        ui->viewPositionComboBox, optEnumToText(v.ViewPosition));
    ViewConfigurationWidget::setComboByText(
        ui->imageLateralityComboBox, optEnumToText(v.ImageLaterality));
    ViewConfigurationWidget::setComboByText(
        ui->imageRotationComboBox, optIntToText(v.ImageRotate));

    const QString cs = v.CollimatorSize.trimmed();
    const QStringList wh = cs.split('x', Qt::SkipEmptyParts);
    if (wh.size() == 2) {
        setComboByText(ui->collimatorSizeXComboBox, wh.at(0).trimmed());
        setComboByText(ui->collimatorSizeYComboBox, wh.at(1).trimmed());
    }
    else {
        ui->collimatorSizeXComboBox->setCurrentIndex(0);
        ui->collimatorSizeYComboBox->setCurrentIndex(0);
    }

    setComboByText(ui->imageProcessComboBox, v.ImageProcessingAlgorithm);
    setComboByText(ui->labelMarkComboBox, optEnumToText(v.LabelMark));
    setComboByText(ui->labelPositionComboBox, optEnumToText(v.LabelPosition));
    setComboByText(ui->positionerProgramComboBox, v.PositionName);

    ui->rotateHorizontallyCheckBox->setChecked(v.ImageHorizontalFlip);
}

// ---- tiny utils ----
int ViewConfigurationWidget::findComboIndexByText(QComboBox* cb, const QString& text, Qt::CaseSensitivity cs)
{
    if (!cb) return -1;
    for (int i = 0; i < cb->count(); ++i)
        if (QString::compare(cb->itemText(i), text, cs) == 0)
            return i;
    return -1;
}

void ViewConfigurationWidget::setComboByText(QComboBox* cb, const QString& text)
{
    if (!cb) return;
    if (text.isEmpty()) { cb->setCurrentIndex(0); return; }
    const int idx = findComboIndexByText(cb, text);
    if (idx >= 0) cb->setCurrentIndex(idx);
}

void ViewConfigurationWidget::setStile()
{
    // --- reusable snippets ---


    const char* lineCss =
        "QLineEdit {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QLineEdit:focus { border-color: rgb(160,160,160); }";

    const char* comboCss =
        "QComboBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 28px 2px 6px;" /* room for arrow */
        "}"
        "QComboBox:focus { border-color: rgb(160,160,160); }"

        /* drop-down button area */
        "QComboBox::drop-down {"
        "  width:20px; border-left:1px solid rgb(120,120,120); margin:0;"
        "}"

        /* the arrow itself */
        "QComboBox::down-arrow {"
        "  image: url(:/Images/Asset/Icon/chevron-down.svg);"
        "  width: 10px; height: 10px;"
        "}"
        "QComboBox::down-arrow:disabled {"
        "  image: url(:/Images/Asset/Icon/chevron-down.svg);"
        "  /* optional: reduce opacity via a lighter icon if you have one */"
        "}"

        /* popup list */
        "QComboBox QAbstractItemView {"
        "  border:1px solid rgb(120,120,120); background:rgb(74,74,74);"
        "  selection-background-color: rgb(92,92,92);"
        "}";

    const char* spinCss =
        "QSpinBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QSpinBox:focus { border-color: rgb(160,160,160); }";

    const char* dateCss =
        "QDateEdit {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QDateEdit:focus { border-color: rgb(160,160,160); }"
        "QDateEdit::drop-down { width:20px; border-left:1px solid rgb(120,120,120); margin:0; }";

    const char* groupCss =
        "QGroupBox {"
        "  border:1px solid rgb(120,120,120); border-radius:6px; margin-top:18px; }"
        "QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 6px; }";

    // Subtle, clean checkbox style (new)
    const char* checkCss =
        "QCheckBox { spacing: 8px; }"
        "QCheckBox::indicator { width:14px; height:16px; }"
        "QCheckBox::indicator:unchecked {"
        "  border:1px solid rgb(120,120,120); background:rgb(74,74,74); border-radius:3px; }"
        "QCheckBox::indicator:unchecked:hover { border-color: rgb(160,160,160); }"
        "QCheckBox::indicator:checked {"
        "  border:1px solid rgb(160,160,160); background:rgb(100,100,100); border-radius:3px; }";

    const char* tabCss =
        "QTabWidget::pane { border:1px solid rgb(120,120,120); border-radius:6px; padding-top:4px; }"
        "QTabBar::tab { padding:6px 12px; }"
        "QTabBar::tab:selected { background: rgb(92,92,92); }";

    const char* buttonCss =
        "QPushButton {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background:rgb(74,74,74);"
        "  min-height:20px;"
        "  padding:4px 12px;"
        "  color: rgb(230,230,230);"
        "}"
        "QPushButton:hover {"
        "  border-color: rgb(160,160,160);"
        "  background:rgb(92,92,92);"
        "}"
        "QPushButton:pressed {"
        "  background:rgb(64,64,64);"
        "}"
        "QPushButton:focus {"
        "  border-color: rgb(180,180,180);"
        "}"
        "QPushButton:disabled {"
        "  border-color: rgb(80,80,80);"
        "  background:rgb(60,60,60);"
        "  color: rgb(130,130,130);"
        "}";

    const char* listCss =
        "QListView{"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"   // contrast with the container bg
        "}";

    // --- apply styles ---
    ui->viewListGroupBox->setStyleSheet(groupCss);
    ui->viewDetailGroupBox->setStyleSheet(groupCss);
    // LineEdits
    ui->viewNameLineEdit->setStyleSheet(lineCss);
    ui->viewDescriptionLineEdit->setStyleSheet(lineCss);
    ui->iconLineEdit->setStyleSheet(lineCss);
    // ComboBoxes
    ui->bodyPartComboBox->setStyleSheet(comboCss);
    ui->patientPositionComboBox->setStyleSheet(comboCss);
    ui->projectionProfileComboBox->setStyleSheet(comboCss);
    ui->patientOrientationRowComboBox->setStyleSheet(comboCss);
    ui->patientOrientationColumnComboBox->setStyleSheet(comboCss);
    ui->viewPositionComboBox->setStyleSheet(comboCss);
    ui->imageLateralityComboBox->setStyleSheet(comboCss);
    ui->imageRotationComboBox->setStyleSheet(comboCss);
    ui->collimatorSizeXComboBox->setStyleSheet(comboCss);
    ui->collimatorSizeYComboBox->setStyleSheet(comboCss);
    ui->imageProcessComboBox->setStyleSheet(comboCss);
    ui->labelMarkComboBox->setStyleSheet(comboCss);
    ui->labelPositionComboBox->setStyleSheet(comboCss);
    ui->positionerProgramComboBox->setStyleSheet(comboCss);

    //--- checkbox ---
    ui->rotateHorizontallyCheckBox->setStyleSheet(checkCss);

    //--- buttons ---
    ui->iconSelectFileBtn->setStyleSheet(buttonCss);
    ui->addViewBtn->setStyleSheet(buttonCss);
    ui->deleteViewBtn->setStyleSheet(buttonCss);

    //--- listview ---
    ui->viewListView->setStyleSheet(listCss);
}
