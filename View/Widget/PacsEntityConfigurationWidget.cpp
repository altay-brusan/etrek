#include "PacsEntityConfigurationWidget.h"
#include "ui_PacsEntityConfigurationWidget.h"

#include <QComboBox>
#include <QListView>
#include <QItemSelectionModel>
#include <QRegularExpression>
#include <QMessageBox>
#include <QSpinBox>
#include <QValidator>
#include <QToolTip>
#include <QPoint>
#include <QEvent>


bool PacsEntityConfigurationWidget::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == ui->hostIPLineEdit && ev->type() == QEvent::FocusOut) {
        const bool ok = ui->hostIPLineEdit->hasAcceptableInput();

        ui->hostIPLineEdit->setProperty("invalid", !ok);
        ui->hostIPLineEdit->style()->unpolish(ui->hostIPLineEdit);
        ui->hostIPLineEdit->style()->polish(ui->hostIPLineEdit);

        if (!ok) {
            // Friendly hint near the field
            QToolTip::showText(
                ui->hostIPLineEdit->mapToGlobal(QPoint(0, ui->hostIPLineEdit->height())),
                tr("Please enter a valid IPv4 address (e.g. 192.168.1.10)."),
                ui->hostIPLineEdit
            );
            // Re-focus after the event completes
            QMetaObject::invokeMethod(ui->hostIPLineEdit, "setFocus", Qt::QueuedConnection);
            return true; // swallow this FocusOut
        }
    }
    return QWidget::eventFilter(obj, ev);
}


class Ipv4Validator : public QValidator {
public:
    explicit Ipv4Validator(QObject* parent = nullptr) : QValidator(parent) {}

    State validate(QString& input, int& pos) const override {
        Q_UNUSED(pos);
        const QString s = input.trimmed();
        if (s.isEmpty()) return Intermediate;           // allow typing

        const QStringList parts = s.split('.', Qt::KeepEmptyParts);
        if (parts.size() > 4) return Invalid;

        bool allComplete = (parts.size() == 4);
        for (const QString& seg : parts) {
            if (seg.isEmpty()) { allComplete = false; continue; } // partial typing → Intermediate
            // digits only
            for (QChar ch : seg) if (!ch.isDigit()) return Invalid;
            // length
            if (seg.size() > 3) return Invalid;
            // no leading zeros like "01" (allow single "0")
            if (seg.size() > 1 && seg.startsWith('0')) return Invalid;

            bool ok = false;
            int v = seg.toInt(&ok, 10);
            if (!ok || v < 0 || v > 255) return Invalid;
        }

        return allComplete ? Acceptable : Intermediate;
    }
};


PacsEntityConfigurationWidget::PacsEntityConfigurationWidget(const QVector<PacsNode>& nodes, QWidget* parent)
    : QWidget(parent),
    ui(new Ui::PacsEntityConfigurationWidget),
    m_nodes(nodes),
    m_archiveModel(new QStandardItemModel(this)),
    m_mppsModel(new QStandardItemModel(this))
{
    ui->setupUi(this);

    // ----- minimal styles -----
    const char* lineCss =
        "QLineEdit {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background:rgb(74,74,74);"
        "  min-height:28px;"
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
        "  background:rgb(74,74,74); min-height:28px; padding:2px 6px; }"
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
        "  min-height:28px;"
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
        "  background:rgb(74,74,74); min-height:28px; padding:2px 28px 2px 6px;" /* room for arrow */
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

    ui->hostNameLineEdit->setStyleSheet(lineCss);
    ui->hostIPLineEdit->setStyleSheet(lineCss);
    ui->calledApplicationEntityLineEdit->setStyleSheet(lineCss);
    ui->callingApplicationEntityLineEdit->setStyleSheet(lineCss);

    ui->hostPortSpinBox->setStyleSheet(spinCss);

    // Ensure correct range/default (also set in .ui)
    ui->hostPortSpinBox->setRange(1, 65535);
    ui->hostPortSpinBox->setValue(100);


    ui->hostIPLineEdit->setValidator(new Ipv4Validator(ui->hostIPLineEdit));
    ui->hostIPLineEdit->installEventFilter(this);

    // --- IP Validator ---
    // Clear red while typing; set red if still invalid on finish
    connect(ui->hostIPLineEdit, &QLineEdit::textChanged, this, [this](const QString&) {
        ui->hostIPLineEdit->setProperty("invalid", false);
        ui->hostIPLineEdit->style()->unpolish(ui->hostIPLineEdit);
        ui->hostIPLineEdit->style()->polish(ui->hostIPLineEdit);
        });

    connect(ui->hostIPLineEdit, &QLineEdit::editingFinished, this, [this]() {
        const bool ok = ui->hostIPLineEdit->hasAcceptableInput();
        ui->hostIPLineEdit->setProperty("invalid", !ok);
        ui->hostIPLineEdit->style()->unpolish(ui->hostIPLineEdit);
        ui->hostIPLineEdit->style()->polish(ui->hostIPLineEdit);
        });

    ui->archiveNodesListView->setStyleSheet(listCss);
    ui->mppsNodesListView->setStyleSheet(listCss);

	ui->defaultConnectionCheckBox->setStyleSheet(checkCss);
	ui->entityTypeComboBox->setStyleSheet(comboCss);
    
    ui->testConnectionBtn->setStyleSheet(buttonCss);
    ui->addBtn->setStyleSheet(buttonCss);
    ui->removeBtn->setStyleSheet(buttonCss);
    ui->updateBtn->setStyleSheet(buttonCss);


    // optional: give the group box a visible border too
    ui->pacsEntityListGroupBox->setStyleSheet(
        "QGroupBox#pacsEntityListGroupBox {"
        "  border: 1px solid rgb(120,120,120); border-radius: 6px; margin-top: 18px;"
        "}"
        "QGroupBox#pacsEntityListGroupBox::title {"
        "  subcontrol-origin: margin; left: 10px; padding: 0 6px;"
        "}"
    );
    
    ui->entityDetailListGroupBox->setStyleSheet(
        "QGroupBox#entityDetailListGroupBox {"
        "  border: 1px solid rgb(120,120,120); border-radius: 6px; margin-top: 18px;"
        "}"
        "QGroupBox#entityDetailListGroupBox::title {"
        "  subcontrol-origin: margin; left: 10px; padding: 0 6px;"
        "}"
    );


    // Prepare UI
    populateEntityTypes();
    buildNodeIndex();
    buildModels();
    bindListViews();

    connect(ui->entityTypeComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &PacsEntityConfigurationWidget::onEntityTypeChanged);

    connect(ui->testConnectionBtn, &QPushButton::clicked,
        this, &PacsEntityConfigurationWidget::onTestConnectionClicked);

    // Default to first entity type if any
    if (ui->entityTypeComboBox->count() > 0)
        onEntityTypeChanged(ui->entityTypeComboBox->currentIndex());
}

PacsEntityConfigurationWidget::~PacsEntityConfigurationWidget()
{
    delete ui;
}


auto mark = [](QLineEdit* e, const char* state) {  // state: "error" | "warning" | "success" | nullptr
    e->setProperty("error", false);
    e->setProperty("warning", false);
    e->setProperty("success", false);
    if (state) e->setProperty(state, true);
    e->style()->unpolish(e); e->style()->polish(e); e->update();
    };


// ------- UI population --------

void PacsEntityConfigurationWidget::populateEntityTypes()
{
    ui->entityTypeComboBox->clear();
    const auto names = PacsEntityTypeUtils::allStrings();
    for (const auto& name : names) {
        const auto t = PacsEntityTypeUtils::parse(name);
        ui->entityTypeComboBox->addItem(name, QVariant::fromValue(t));
    }
}

void PacsEntityConfigurationWidget::buildNodeIndex()
{
    m_indexById.clear();
    for (int i = 0; i < m_nodes.size(); ++i)
        m_indexById.insert(m_nodes[i].Id, i);
}

QString PacsEntityConfigurationWidget::nodeDisplay(const PacsNode& n)
{
    const QString name = n.HostName.trimmed();
    return name.isEmpty() ? QStringLiteral("<unnamed>") : name;
}


void PacsEntityConfigurationWidget::buildModels()
{
    m_archiveModel->clear();
    m_mppsModel->clear();

    // Optional: set headers
    m_archiveModel->setHorizontalHeaderLabels({ QStringLiteral("Archive Nodes") });
    m_mppsModel->setHorizontalHeaderLabels({ QStringLiteral("MPPS Nodes") });

    for (const auto& n : m_nodes) {
        auto* item = new QStandardItem(nodeDisplay(n));
        item->setEditable(false);
        item->setData(n.Id, Qt::UserRole + 1);

        if (n.Type == PacsEntityType::Archive) {
            m_archiveModel->appendRow(item);
        }
        else if (n.Type == PacsEntityType::MPPS) {
            m_mppsModel->appendRow(item);
        }
        else {
            delete item; // unknown type
        }
    }
}

void PacsEntityConfigurationWidget::bindListViews()
{
    // ListView setup
    ui->archiveNodesListView->setModel(m_archiveModel);
    ui->archiveNodesListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->archiveNodesListView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->mppsNodesListView->setModel(m_mppsModel);
    ui->mppsNodesListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mppsNodesListView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Selection changed -> load form
    connect(ui->archiveNodesListView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &PacsEntityConfigurationWidget::onArchiveSelectionChanged);

    connect(ui->mppsNodesListView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &PacsEntityConfigurationWidget::onMppsSelectionChanged);
}

// ------- Selection handling --------

const PacsNode* PacsEntityConfigurationWidget::selectedArchiveNode() const
{
    const auto idxs = ui->archiveNodesListView->selectionModel()->selectedIndexes();
    if (idxs.isEmpty()) return nullptr;
    const int id = idxs.first().data(Qt::UserRole + 1).toInt();
    const int pos = m_indexById.value(id, -1);
    return (pos >= 0 && pos < m_nodes.size()) ? &m_nodes[pos] : nullptr;
}

const PacsNode* PacsEntityConfigurationWidget::selectedMppsNode() const
{
    const auto idxs = ui->mppsNodesListView->selectionModel()->selectedIndexes();
    if (idxs.isEmpty()) return nullptr;
    const int id = idxs.first().data(Qt::UserRole + 1).toInt();
    const int pos = m_indexById.value(id, -1);
    return (pos >= 0 && pos < m_nodes.size()) ? &m_nodes[pos] : nullptr;
}

void PacsEntityConfigurationWidget::onArchiveSelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/)
{
    Q_UNUSED(selected);
    loadNodeIntoForm(selectedArchiveNode());
}

void PacsEntityConfigurationWidget::onMppsSelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/)
{
    Q_UNUSED(selected);
    loadNodeIntoForm(selectedMppsNode());
}

// ------- Entity-type toggle --------

void PacsEntityConfigurationWidget::onEntityTypeChanged(int index)
{
    if (index < 0) return;

    const QVariant v = ui->entityTypeComboBox->itemData(index);
    const auto type = v.isValid()
        ? v.value<PacsEntityType>()
        : PacsEntityTypeUtils::parse(ui->entityTypeComboBox->itemText(index));

    // Enable/disable lists to guide the user
    const bool isArchive = (type == PacsEntityType::Archive);
    ui->archiveNodesListView->setEnabled(isArchive);
    ui->mppsNodesListView->setEnabled(!isArchive);

    // auto-select first item in the active list (if none selected)
    if (isArchive) {
        if (ui->archiveNodesListView->selectionModel()->selectedIndexes().isEmpty()
            && m_archiveModel->rowCount() > 0) {
            auto idx = m_archiveModel->index(0, 0);
            ui->archiveNodesListView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            loadNodeIntoForm(selectedArchiveNode());
        }
    }
    else {
        if (ui->mppsNodesListView->selectionModel()->selectedIndexes().isEmpty()
            && m_mppsModel->rowCount() > 0) {
            auto idx = m_mppsModel->index(0, 0);
            ui->mppsNodesListView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            loadNodeIntoForm(selectedMppsNode());
        }
    }

    if (m_archiveModel->rowCount() == 0 && m_mppsModel->rowCount() == 0)
        clearForm();
}

// ------- Form bind/unbind --------

void PacsEntityConfigurationWidget::loadNodeIntoForm(const PacsNode* n)
{
    if (!n) { clearForm(); return; }

    ui->hostNameLineEdit->setText(n->HostName);
    ui->hostIPLineEdit->setText(n->HostIp);
    ui->hostPortSpinBox->setValue(qBound(1, n->Port > 0 ? n->Port : 100, 65535));
    ui->calledApplicationEntityLineEdit->setText(n->CalledAet);
    ui->callingApplicationEntityLineEdit->setText(n->CallingAet);
}

void PacsEntityConfigurationWidget::clearForm()
{
    ui->hostNameLineEdit->clear();
    ui->hostIPLineEdit->clear();
    ui->hostIPLineEdit->setProperty("invalid", false);
    ui->hostPortSpinBox->setValue(100);
    ui->calledApplicationEntityLineEdit->clear();
    ui->callingApplicationEntityLineEdit->clear();
    ui->defaultConnectionCheckBox->setChecked(false);
}

// ------- Test button --------

void PacsEntityConfigurationWidget::onTestConnectionClicked()
{
    // Determine which list is active, get the selected node
    const int typeIdx = ui->entityTypeComboBox->currentIndex();
    const auto typeVar = ui->entityTypeComboBox->itemData(typeIdx);
    const auto type = typeVar.isValid()
        ? typeVar.value<PacsEntityType>()
        : PacsEntityTypeUtils::parse(ui->entityTypeComboBox->currentText());

    const PacsNode* sel = (type == PacsEntityType::Archive) ? selectedArchiveNode() : selectedMppsNode();
    if (!sel) {
        QMessageBox::information(this, tr("No Selection"), tr("Please select a PACS node to test."));
        return;
    }

    emit testConnectionRequested(*sel);
}
