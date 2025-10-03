#include <QVBoxLayout>
#include <QStandardItem>
#include <QSet>
#include <algorithm>

#include "TechniqueConfigurationWidget.h"
#include "ui_TechniqueConfigurationWidget.h"
#include "OrientationDetailWidget.h"


using namespace Etrek::ScanProtocol;
using namespace Etrek::ScanProtocol::Data::Entity;

TechniqueConfigurationWidget::TechniqueConfigurationWidget(
    const QVector<TechniqueParameter>& parameters,
    QWidget* parent)
    : QWidget(parent)
    , m_parameters(parameters)
    , ui(new Ui::TechniqueConfigurationWidget)
{
    ui->setupUi(this);
    initStyles();

    // Create 4 orientation widgets ONCE and place them
    auto* apPa = new OrientationDetailWidget(TechniqueProfile::AP_PA, &m_parameters, this);
    auto* lat = new OrientationDetailWidget(TechniqueProfile::LAT, &m_parameters, this);
    auto* obl = new OrientationDetailWidget(TechniqueProfile::OBL, &m_parameters, this);
    auto* axial = new OrientationDetailWidget(TechniqueProfile::AXIAL, &m_parameters, this);

    auto place = [](QWidget* ph, QWidget* w) {
        auto* l = new QVBoxLayout(ph);
        l->setContentsMargins(0, 0, 0, 0);
        l->addWidget(w);
        };
    place(ui->appaOrientationDetailPlaceHolder, apPa);
    place(ui->latOrientationDetailPlaceHolder, lat);
    place(ui->oblOrientationDetailPlaceHolder, obl);
    place(ui->axialOrientationDetailPlaceHolder, axial);

    // Wire them to our signal
    connect(this, &TechniqueConfigurationWidget::bodyPartSelected, apPa, &OrientationDetailWidget::onBodyPartChanged);
    connect(this, &TechniqueConfigurationWidget::bodyPartSelected, lat, &OrientationDetailWidget::onBodyPartChanged);
    connect(this, &TechniqueConfigurationWidget::bodyPartSelected, obl, &OrientationDetailWidget::onBodyPartChanged);
    connect(this, &TechniqueConfigurationWidget::bodyPartSelected, axial, &OrientationDetailWidget::onBodyPartChanged);

    // Build the left list once
    buildBodyPartList();
    ui->bodyPartListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->bodyPartListView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Connect selection -> emit bodyPartSelected
    connect(ui->bodyPartListView->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &TechniqueConfigurationWidget::onBodyPartSelectionChanged);

    // Select first and broadcast
    if (m_bodyPartModel && m_bodyPartModel->rowCount() > 0) {
        const QModelIndex idx = m_bodyPartModel->index(0, 0);
        ui->bodyPartListView->setCurrentIndex(idx);
        emit bodyPartSelected(idx.data(BodyPartIdRole).toInt());
    }
}


TechniqueConfigurationWidget::~TechniqueConfigurationWidget()
{
    delete ui;
}

void TechniqueConfigurationWidget::buildBodyPartList()
{
    if (!m_bodyPartModel) {
        m_bodyPartModel = new QStandardItemModel(this);
        ui->bodyPartListView->setModel(m_bodyPartModel);
    }
    else {
        m_bodyPartModel->clear();
    }
    m_bodyPartById.clear();

    // collect unique body parts by Id (fallback to case-insensitive name)
    QSet<int>     seenIds;
    QSet<QString> seenNames;

    QVector<BodyPart> uniqueParts;
    uniqueParts.reserve(m_parameters.size());

    for (const auto& tp : m_parameters) {
        const BodyPart& bp = tp.BodyPart;
        if (bp.Id > 0) {
            if (seenIds.contains(bp.Id)) continue;
            seenIds.insert(bp.Id);
            uniqueParts.push_back(bp);
        }
        else {
            const QString key = bp.Name.trimmed().toLower();
            if (key.isEmpty() || seenNames.contains(key)) continue;
            seenNames.insert(key);
            uniqueParts.push_back(bp);
        }
    }

    // sort by Region.DisplayOrder, Region.Name, BodyPart.Name
    std::sort(uniqueParts.begin(), uniqueParts.end(),
        [](const BodyPart& a, const BodyPart& b) {
            if (a.Region.DisplayOrder != b.Region.DisplayOrder)
                return a.Region.DisplayOrder < b.Region.DisplayOrder;
            const int rn = QString::localeAwareCompare(a.Region.Name, b.Region.Name);
            if (rn != 0) return rn < 0;
            return QString::localeAwareCompare(a.Name, b.Name) < 0;
        });

    // populate model
    m_bodyPartModel->setColumnCount(1);
    m_bodyPartModel->setHorizontalHeaderLabels({ tr("Body Part") });

    for (const auto& bp : uniqueParts) {
        auto* item = new QStandardItem(bp.Name);
        item->setEditable(false);
        item->setData(bp.Id, BodyPartIdRole);
        m_bodyPartModel->appendRow(item);
        if (bp.Id > 0) m_bodyPartById.insert(bp.Id, bp);
    }
}

void TechniqueConfigurationWidget::onBodyPartSelectionChanged(
    const QModelIndex& current, const QModelIndex& /*previous*/)
{
    if (!current.isValid()) return;
    emit bodyPartSelected(current.data(BodyPartIdRole).toInt());
}

void TechniqueConfigurationWidget::initStyles()
{
    const char* tabCss =
        "QTabWidget#orientationTabWidget::pane { "
        "  border: 2px solid rgb(120,120,120); top: -1px; } "
        "QTabWidget#orientationTabWidget QTabBar::tab { "
        "  background-color: rgb(83, 83, 83); color: rgb(208, 208, 208); "
        "  padding: 6px; border: none; } "
        "QTabWidget#orientationTabWidget QTabBar::tab:selected { "
        "  background-color: rgb(60, 60, 60); color: rgb(255, 255, 255); }";

    const char* groupCss =
        "QGroupBox { border:1px solid rgb(120,120,120); border-radius:6px; margin-top:18px; }"
        "QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 6px; }";

    const char* listCss =
        "QListView { border: 1px solid rgb(120,120,120); border-radius: 4px; background: rgb(74,74,74); }";

    ui->orientationTabWidget->setStyleSheet(tabCss);
    ui->bodyPartListGroupBox->setStyleSheet(groupCss);
    ui->bodyPartListView->setStyleSheet(listCss);

}

int TechniqueConfigurationWidget::currentBodyPartId() const
{
    const auto idx = ui->bodyPartListView->currentIndex();
    return idx.isValid() ? idx.data(BodyPartIdRole).toInt() : -1;
}

BodyPart TechniqueConfigurationWidget::currentBodyPart() const
{
    return m_bodyPartById.value(currentBodyPartId());
}

QVector<TechniqueParameter>
TechniqueConfigurationWidget::collectCurrentBodyPartParameters() const
{
    QVector<TechniqueParameter> out;
    const auto bp = currentBodyPart();
    if (bp.Id <= 0) return out;

    auto collectFrom = [&](QWidget* ph) {
        if (auto ow = ph->findChild<OrientationDetailWidget*>())
            out += ow->collectForBodyPart(bp);
        };
    collectFrom(ui->appaOrientationDetailPlaceHolder);
    collectFrom(ui->latOrientationDetailPlaceHolder);
    collectFrom(ui->oblOrientationDetailPlaceHolder);
    collectFrom(ui->axialOrientationDetailPlaceHolder);
    return out;
}