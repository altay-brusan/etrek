#include "ProcedureConfigurationWidget.h"
#include "ui_ProcedureConfigurationWidget.h"

#include <QHeaderView>
#include <QStandardItem>
#include <QItemSelectionModel>
#include <QVariant>

#include "BodyPart.h"
#include "AnatomicRegion.h"
#include "Procedure.h"
#include "View.h"
#include "ScanProtocolUtil.h"

using namespace Etrek::ScanProtocol::Data::Entity;
using namespace Etrek::ScanProtocol;

static constexpr int kIdRole = Qt::UserRole + 1;

ProcedureConfigurationWidget::ProcedureConfigurationWidget(const QVector<Procedure>& procedures,
    const QVector<BodyPart>& bodyParts,
    const QVector<AnatomicRegion>& anatomicRegions,
    const QVector<View>& views,
    QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ProcedureConfigurationWidget)
    , m_procedures(procedures)
    , m_views(views)
    , m_bodyParts(bodyParts)
    , m_anatomicRegions(anatomicRegions)
{
    ui->setupUi(this);
    initStyles();
    buildModels();
    populateCombosOnce();
    populateAllViewsModel();
    populateProcedureTable();
    bindSignals();

    // Initial state: no procedure selected -> disable detail group interaction
    ui->procedureDetailsGroupBox->setEnabled(false);
    ui->removeViewBtn->setEnabled(false);
    ui->addViewBtn->setEnabled(false);
}

ProcedureConfigurationWidget::~ProcedureConfigurationWidget()
{
    delete ui;
    delete m_procModel;
    delete m_allViewsModel;
    delete m_selectedViewsModel;
}


/* ------------------------------ build / populate --------------------------- */

void ProcedureConfigurationWidget::buildModels()
{
    // Procedures table: two columns (Procedure, Body Part)
    m_procModel = new QStandardItemModel(this);
    m_procModel->setColumnCount(2);
    m_procModel->setHorizontalHeaderLabels({ tr("Procedure"), tr("Body Part") });
    ui->procedureListTableView->setModel(m_procModel);

    auto* hh = ui->procedureListTableView->horizontalHeader();
    hh->setStretchLastSection(true);
    hh->setSectionResizeMode(0, QHeaderView::Stretch);          // Procedure
    hh->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Body Part

    ui->procedureListTableView->verticalHeader()->setVisible(false);
    ui->procedureListTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->procedureListTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->procedureListTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->procedureListTableView->setShowGrid(false);


    // All views
    m_allViewsModel = new QStandardItemModel(this);
    m_allViewsModel->setColumnCount(1);
    ui->allViewListView->setModel(m_allViewsModel);
    ui->allViewListView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Selected views
    m_selectedViewsModel = new QStandardItemModel(this);
    m_selectedViewsModel->setColumnCount(1);
    ui->selectedViewListView->setModel(m_selectedViewsModel);
    ui->selectedViewListView->setSelectionMode(QAbstractItemView::SingleSelection);
}


void ProcedureConfigurationWidget::populateCombosOnce()
{
    // ----- Body parts (nullable) -----
    ui->bodyPartComboBox->clear();
    // index 0: blank / none
    ui->bodyPartComboBox->addItem(QString());
    ui->bodyPartComboBox->setItemData(0, QVariant(), kIdRole);

    for (const auto& bp : m_bodyParts) {
        const int idx = ui->bodyPartComboBox->count();
        ui->bodyPartComboBox->addItem(bp.Name);
        ui->bodyPartComboBox->setItemData(idx, bp.Id, kIdRole); // store id in kIdRole
    }

    // ----- Regions (nullable) -----
    ui->anatomicRegionComboBox->clear();
    ui->anatomicRegionComboBox->addItem(QString());
    ui->anatomicRegionComboBox->setItemData(0, QVariant(), kIdRole);

    for (const auto& ar : m_anatomicRegions) {
        const int idx = ui->anatomicRegionComboBox->count();
        ui->anatomicRegionComboBox->addItem(ar.Name);
        ui->anatomicRegionComboBox->setItemData(idx, ar.Id, kIdRole);
    }

    // Print orientation / format are hard-coded in .ui; we only set current index later.
}

void ProcedureConfigurationWidget::populateAllViewsModel()
{
    m_allViewsModel->removeRows(0, m_allViewsModel->rowCount());
    for (const auto& v : m_views) {
        auto* it = new QStandardItem(v.Name);
        it->setData(v.Id, kIdRole);
        it->setEditable(false);
        m_allViewsModel->appendRow(it);
    }
}

void ProcedureConfigurationWidget::populateProcedureTable()
{
    m_procModel->removeRows(0, m_procModel->rowCount());

    QVector<Procedure> sorted = m_procedures;
    std::sort(sorted.begin(), sorted.end(), [](const Procedure& a, const Procedure& b) {
        const int da = a.Region ? a.Region->DisplayOrder : std::numeric_limits<int>::max();
        const int db = b.Region ? b.Region->DisplayOrder : std::numeric_limits<int>::max();
        if (da != db) return da < db;

        const int rn = QString::localeAwareCompare(a.Region ? a.Region->Name : QString(),
            b.Region ? b.Region->Name : QString());
        if (rn != 0) return rn < 0;

        const int bn = QString::localeAwareCompare(a.BodyPart ? a.BodyPart->Name : QString(),
            b.BodyPart ? b.BodyPart->Name : QString());
        if (bn != 0) return bn < 0;

        return QString::localeAwareCompare(a.Name, b.Name) < 0;
        });

    for (const auto& p : sorted) {
        const QString bodyName = p.BodyPart ? p.BodyPart->Name : QString();

        auto* nameItem = new QStandardItem(p.Name);     // col 0: Procedure
        auto* bodyItem = new QStandardItem(bodyName);   // col 1: Body Part

        nameItem->setData(p.Id, kIdRole);
        bodyItem->setData(p.Id, kIdRole);
        nameItem->setEditable(false);
        bodyItem->setEditable(false);

        QList<QStandardItem*> rowItems;
        rowItems << nameItem << bodyItem;               // order: Procedure, Body Part
        m_procModel->appendRow(rowItems);
    }

    if (m_procModel->rowCount() > 0) {
        const QModelIndex idx = m_procModel->index(0, 0);
        ui->procedureListTableView->selectionModel()->select(
            idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        ui->procedureListTableView->selectionModel()->setCurrentIndex(
            idx, QItemSelectionModel::Current | QItemSelectionModel::Rows);
        onProcedureTableCurrentChanged(idx, QModelIndex{});
    }
}



/* ------------------------------- helpers ----------------------------------- */

int ProcedureConfigurationWidget::findComboIndexById(QComboBox* cb, int id) const
{
    if (!cb) return -1;
    for (int i = 0; i < cb->count(); ++i) {
        const QVariant v = cb->itemData(i, kIdRole);
        if (v.isValid() && !v.isNull() && v.toInt() == id)
            return i;
    }
    return -1;
}


int ProcedureConfigurationWidget::findViewIndexById(const QVector<View>& src, int id) const
{
    for (int i = 0; i < src.size(); ++i)
        if (src[i].Id == id) return i;
    return -1;
}

/* ------------------------------- refresh UI -------------------------------- */

void ProcedureConfigurationWidget::refreshDetailsForRow(int row)
{
    if (row < 0 || row >= m_procedures.size()) {
        ui->procedureDetailsGroupBox->setEnabled(false);
        ui->nameLineEdit->clear();
        ui->codeLineEdit->clear();
        ui->codeSchemaLineEdit->clear();
        ui->codeMeaningLineEdit->clear();
        ui->trueSizeCheckBox->setChecked(false);
        ui->bodyPartComboBox->setCurrentIndex(-1);
        ui->anatomicRegionComboBox->setCurrentIndex(-1);
        ui->printOrientationComboBox->setCurrentIndex(0); // blank
        ui->printFormatComboBox->setCurrentIndex(0);      // blank
        return;
    }

    ui->procedureDetailsGroupBox->setEnabled(true);

    Procedure& p = m_procedures[row];

    ui->nameLineEdit->setText(p.Name);
    ui->codeLineEdit->setText(p.CodeValue);
    ui->codeSchemaLineEdit->setText(p.CodingSchema);
    ui->codeMeaningLineEdit->setText(p.CodeMeaning);
    ui->trueSizeCheckBox->setChecked(p.IsTrueSize);

    // BodyPart / Region (nullable)
    {
        // BodyPart
        int bpIdx = 0; // default to blank
        if (p.BodyPart.has_value()) {
            const int bpId = p.BodyPart->Id;
            const int idx = findComboIndexById(ui->bodyPartComboBox, bpId);
            if (idx >= 0) bpIdx = idx;
        }
        ui->bodyPartComboBox->setCurrentIndex(bpIdx);

        // Region
        int arIdx = 0; // default to blank
        if (p.Region.has_value()) {
            const int arId = p.Region->Id;
            const int idx = findComboIndexById(ui->anatomicRegionComboBox, arId);
            if (idx >= 0) arIdx = idx;
        }
        ui->anatomicRegionComboBox->setCurrentIndex(arIdx);
    }


    // Print Orientation (items: "", "Landscape", "Portrait")
    {
        QString s = p.PrintOrientation
            ? ScanProtocolUtil::toString(*p.PrintOrientation)  // enum -> QString
            : QString();                                       // empty -> index 0
        int idx = ui->printOrientationComboBox->findText(s, Qt::MatchFixedString);
        ui->printOrientationComboBox->setCurrentIndex(idx >= 0 ? idx : 0);
    }

    // Print Format (items: "", "STANDARD\1,1", "STANDARD\1,2", "STANDARD\2,1", "STANDARD\2,2")
    {
        QString s = p.PrintFormat
            ? ScanProtocolUtil::toString(*p.PrintFormat)       // enum -> QString
            : QString();
        int idx = ui->printFormatComboBox->findText(s, Qt::MatchFixedString);
        ui->printFormatComboBox->setCurrentIndex(idx >= 0 ? idx : 0);
    }


    // Enable/disable buttons based on current selections
    onAllViewsSelectionChanged();
    onSelectedViewsSelectionChanged();
}

void ProcedureConfigurationWidget::refreshSelectedViewsForRow(int row)
{
    m_selectedViewsModel->removeRows(0, m_selectedViewsModel->rowCount());
    if (row < 0 || row >= m_procedures.size()) return;

    const auto& vlist = m_procedures[row].Views;
    for (const auto& v : vlist) {
        auto* it = new QStandardItem(v.Name);
        it->setData(v.Id, kIdRole);
        it->setEditable(false);
        m_selectedViewsModel->appendRow(it);
    }

    // After repopulating, update button enabled states
    onSelectedViewsSelectionChanged();
}

/* -------------------------------- signals ---------------------------------- */

void ProcedureConfigurationWidget::bindSignals()
{
    // Selection change in procedures table
    connect(ui->procedureListTableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
        this, &ProcedureConfigurationWidget::onProcedureTableCurrentChanged);

    // Lists selection
    connect(ui->allViewListView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &ProcedureConfigurationWidget::onAllViewsSelectionChanged);
    connect(ui->selectedViewListView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &ProcedureConfigurationWidget::onSelectedViewsSelectionChanged);

    // Buttons
    connect(ui->addViewBtn, &QPushButton::clicked, this, &ProcedureConfigurationWidget::onAddView);
    connect(ui->removeViewBtn, &QPushButton::clicked, this, &ProcedureConfigurationWidget::onRemoveView);

    // Detail editors
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this, &ProcedureConfigurationWidget::onNameChanged);
    connect(ui->codeLineEdit, &QLineEdit::textChanged, this, &ProcedureConfigurationWidget::onCodeChanged);
    connect(ui->codeSchemaLineEdit, &QLineEdit::textChanged, this, &ProcedureConfigurationWidget::onCodingSchemaChanged);
    connect(ui->codeMeaningLineEdit, &QLineEdit::textChanged, this, &ProcedureConfigurationWidget::onCodeMeaningChanged);
    connect(ui->trueSizeCheckBox, &QCheckBox::toggled, this, &ProcedureConfigurationWidget::onTrueSizeToggled);
    connect(ui->bodyPartComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ProcedureConfigurationWidget::onBodyPartChanged);
    connect(ui->anatomicRegionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ProcedureConfigurationWidget::onRegionChanged);
    connect(ui->printOrientationComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ProcedureConfigurationWidget::onPrintOrientationChanged);
    connect(ui->printFormatComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ProcedureConfigurationWidget::onPrintFormatChanged);
}

/* ------------------------------ slots / handlers --------------------------- */

void ProcedureConfigurationWidget::onProcedureTableCurrentChanged(
    const QModelIndex& current, const QModelIndex& /*previous*/)
{
    int procIndex = -1;
    if (current.isValid()) {
        // We stored the procedure id on BOTH columns in kIdRole
        const int procId = current.sibling(current.row(), 0).data(kIdRole).toInt();
        procIndex = findProcedureIndexById(procId);
    }

    m_currentProcRow = procIndex;
    refreshDetailsForRow(m_currentProcRow);
    refreshSelectedViewsForRow(m_currentProcRow);
}

int ProcedureConfigurationWidget::findProcedureIndexById(int id) const
{
    for (int i = 0; i < m_procedures.size(); ++i)
        if (m_procedures[i].Id == id) return i;
    return -1;
}

void ProcedureConfigurationWidget::onAllViewsSelectionChanged()
{
    bool enable = false;
    if (m_currentProcRow >= 0) {
        const auto sel = ui->allViewListView->selectionModel()->selectedIndexes();
        enable = !sel.isEmpty();
        if (enable) {
            // Prevent duplicates: disable if already in selected list
            const int viewId = sel.first().data(kIdRole).toInt();
            enable = (findViewIndexById(m_procedures[m_currentProcRow].Views, viewId) < 0);
        }
    }
    ui->addViewBtn->setEnabled(enable);
}

void ProcedureConfigurationWidget::onSelectedViewsSelectionChanged()
{
    bool enable = false;
    if (m_currentProcRow >= 0) {
        const auto sel = ui->selectedViewListView->selectionModel()->selectedIndexes();
        enable = !sel.isEmpty();
    }
    ui->removeViewBtn->setEnabled(enable);
}

void ProcedureConfigurationWidget::onAddView()
{
    if (m_currentProcRow < 0) return;
    const auto sel = ui->allViewListView->selectionModel()->selectedIndexes();
    if (sel.isEmpty()) return;

    const int viewId = sel.first().data(kIdRole).toInt();
    if (findViewIndexById(m_procedures[m_currentProcRow].Views, viewId) >= 0)
        return; // already added

    // Find view object in m_views
    const int idx = findViewIndexById(m_views, viewId);
    if (idx < 0) return;

    m_procedures[m_currentProcRow].Views.push_back(m_views[idx]);

    // Update UI
    auto* it = new QStandardItem(m_views[idx].Name);
    it->setData(viewId, kIdRole);
    it->setEditable(false);
    m_selectedViewsModel->appendRow(it);

    onAllViewsSelectionChanged();
}

void ProcedureConfigurationWidget::onRemoveView()
{
    if (m_currentProcRow < 0) return;
    const auto sel = ui->selectedViewListView->selectionModel()->selectedIndexes();
    if (sel.isEmpty()) return;

    const int row = sel.first().row();
    const int viewId = sel.first().data(kIdRole).toInt();

    // Remove from model
    m_selectedViewsModel->removeRow(row);

    // Remove from data
    auto& vec = m_procedures[m_currentProcRow].Views;
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i].Id == viewId) { vec.remove(i); break; }
    }

    onSelectedViewsSelectionChanged();
}

/* --------------------------- write-back handlers --------------------------- */

void ProcedureConfigurationWidget::onNameChanged(const QString& v)
{
    if (m_currentProcRow < 0) return;
    m_procedures[m_currentProcRow].Name = v;

    // Update the currently selected row’s "Procedure" cell (column 0)
    const auto cur = ui->procedureListTableView->selectionModel()->currentIndex();
    if (cur.isValid()) {
        const QModelIndex procCol0 = cur.sibling(cur.row(), 0);
        m_procModel->setData(procCol0, v);
    }
}




void ProcedureConfigurationWidget::onCodeChanged(const QString& v)
{
    if (m_currentProcRow < 0) return;
    m_procedures[m_currentProcRow].CodeValue = v;
}

void ProcedureConfigurationWidget::onCodingSchemaChanged(const QString& v)
{
    if (m_currentProcRow < 0) return;
    m_procedures[m_currentProcRow].CodingSchema = v;
}

void ProcedureConfigurationWidget::onCodeMeaningChanged(const QString& v)
{
    if (m_currentProcRow < 0) return;
    m_procedures[m_currentProcRow].CodeMeaning = v;
}

void ProcedureConfigurationWidget::onBodyPartChanged(int idx)
{
    if (m_currentProcRow < 0) return;

    // idx 0 => blank
    if (idx <= 0) {
        m_procedures[m_currentProcRow].BodyPart.reset();
        return;
    }

    const QVariant v = ui->bodyPartComboBox->itemData(idx, kIdRole); // <- use kIdRole
    if (!v.isValid() || v.isNull()) {
        m_procedures[m_currentProcRow].BodyPart.reset();
        return;
    }

    const int bpId = v.toInt();
    for (const auto& bp : m_bodyParts) {
        if (bp.Id == bpId) {
            m_procedures[m_currentProcRow].BodyPart = bp;
            break;
        }
    }
}

void ProcedureConfigurationWidget::onRegionChanged(int idx)
{
    if (m_currentProcRow < 0) return;

    if (idx <= 0) {
        m_procedures[m_currentProcRow].Region.reset();
        return;
    }

    const QVariant v = ui->anatomicRegionComboBox->itemData(idx, kIdRole); // <- use kIdRole
    if (!v.isValid() || v.isNull()) {
        m_procedures[m_currentProcRow].Region.reset();
        return;
    }

    const int arId = v.toInt();
    for (const auto& ar : m_anatomicRegions) {
        if (ar.Id == arId) {
            m_procedures[m_currentProcRow].Region = ar;
            break;
        }
    }
}

void ProcedureConfigurationWidget::onPrintOrientationChanged(int idx)
{
    if (m_currentProcRow < 0) return;

    // idx 0 = blank => NULL in DB / std::nullopt in model
    if (idx <= 0) {
        m_procedures[m_currentProcRow].PrintOrientation.reset();
        return;
    }

    const QString text = ui->printOrientationComboBox->itemText(idx);
    if (auto e = ScanProtocolUtil::parsePrintOrientation(text)) {
        m_procedures[m_currentProcRow].PrintOrientation = *e;
    }
    else {
        // Fallback: clear if unexpected string
        m_procedures[m_currentProcRow].PrintOrientation.reset();
    }
}


void ProcedureConfigurationWidget::onPrintFormatChanged(int idx)
{
    if (m_currentProcRow < 0) return;

    // idx 0 = blank => NULL in DB
    if (idx <= 0) {
        m_procedures[m_currentProcRow].PrintFormat.reset();
        return;
    }

    const QString text = ui->printFormatComboBox->itemText(idx);
    if (auto e = ScanProtocolUtil::parsePrintFormat(text)) {
        m_procedures[m_currentProcRow].PrintFormat = *e;
    }
    else {
        // fallback if somehow unparsable
        m_procedures[m_currentProcRow].PrintFormat.reset();
    }
}

void ProcedureConfigurationWidget::onTrueSizeToggled(bool on)
{
    if (m_currentProcRow < 0) return;
    m_procedures[m_currentProcRow].IsTrueSize = on;
}


void ProcedureConfigurationWidget::initStyles()
{
    // ----- minimal styles -----
    const char* lineCss =
        "QLineEdit {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background:rgb(74,74,74);"
        "  min-height:20px;"
        "  padding:2px 6px;"
        "   width:26px; "
        "}"
        "QLineEdit:focus {"
        "  border-color: rgb(160,160,160);"
        "}"
        /* invalid IP cue (red border when property invalid=true) */
        "QLineEdit#hostIPLineEdit[invalid=\"true\"] {"
        "  border:1px solid rgb(200,80,80);"
        "}";

    const char* spinCss =
        "QSpinBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QSpinBox:focus { border-color: rgb(160,160,160); }";

    const char* listCss =
        "QListView {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"   // contrast with the container bg
        "}";

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

    const char* checkCss =
        "QCheckBox { spacing: 8px; }"
        "QCheckBox::indicator { width:16px; height:16px; }"
        "QCheckBox::indicator:unchecked {"
        "  border:1px solid rgb(120,120,120); background:rgb(74,74,74); border-radius:3px; }"
        "QCheckBox::indicator:unchecked:hover { border-color: rgb(160,160,160); }"
        "QCheckBox::indicator:checked {"
        "  border:1px solid rgb(160,160,160); background:rgb(54,69,79); border-radius:3px; }";

    const char* comboCss =
        "QComboBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 28px 2px 6px;" /* room for arrow */
        "}"
        "QComboBox:focus { border-color: rgb(160,160,160); }"

        /* drop-down button area */
        "QComboBox::drop-down {"
        "  width:26px; border-left:1px solid rgb(120,120,120); margin:0;"
        "}"

        /* the arrow itself */
        "QComboBox::down-arrow {"
        "  image: url(:/Images/Asset/Icon/chevron-down.svg);"
        "  width: 12px; height: 12px;"
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

    const char* groupCss =
        "QGroupBox {"
        "  border:1px solid rgb(120,120,120); border-radius:6px; margin-top:18px; }"
        "QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 6px; }";

    const char* tableCss =
        "QTableView {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "  gridline-color: transparent;"
        /* Selection palette (applies to the whole view) */
        "  selection-background-color: rgb(45,110,190);"
        "  selection-color: rgb(235,235,235);"
        "}"
        /* Remove the cell focus rect so only the row highlight shows */
        "QTableView::item:focus { outline: none; }"
        /* Optional: no per-item override, let the palette above drive it */
        /* Header */
        "QHeaderView::section {"
        "  background-color: rgb(64,64,64);"
        "  color: rgb(220,220,220);"
        "  padding: 4px 8px;"
        "  border: 1px solid rgb(100,100,100);"
        "  font-weight: bold;"
        "}"
        "QHeaderView { border: none; }";

    ui->procedureListGroupBox->setStyleSheet(groupCss);
    ui->procedureDetailsGroupBox->setStyleSheet(groupCss);
    ui->allViewGroupBox->setStyleSheet(groupCss);
    ui->selectedViewGroupBox->setStyleSheet(groupCss);
    ui->procedureViewGroupBox->setStyleSheet(groupCss);
	ui->procedureListTableView->setStyleSheet(tableCss);
    ui->procedureListTableView->setFocusPolicy(Qt::NoFocus);


    ui->nameLineEdit->setStyleSheet(lineCss);
    ui->codeLineEdit->setStyleSheet(lineCss);
    ui->codeSchemaLineEdit->setStyleSheet(lineCss);
    ui->codeMeaningLineEdit->setStyleSheet(lineCss);

    ui->bodyPartComboBox->setStyleSheet(comboCss);
    ui->anatomicRegionComboBox->setStyleSheet(comboCss);
    ui->printOrientationComboBox->setStyleSheet(comboCss);
    ui->printFormatComboBox->setStyleSheet(comboCss);

    ui->AddProcedureBtn->setStyleSheet(buttonCss);
    ui->deleteBtn->setStyleSheet(buttonCss);

    ui->trueSizeCheckBox->setStyleSheet(checkCss);


    ui->allViewListView->setStyleSheet(listCss);
    ui->selectedViewListView->setStyleSheet(listCss);
}
