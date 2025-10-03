#include <QValidator>
#include <QEvent>
#include <QToolTip>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QListView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QItemSelectionModel>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "WorkListConfigurationWidget.h"
#include "ui_WorkListConfigurationWidget.h"




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

WorkListConfigurationWidget::WorkListConfigurationWidget(const QList<DicomTag>& dicomTags, QWidget* parent)
    : QWidget(parent)
    , m_dicomTags(dicomTags)
    , ui(new Ui::WorkListConfigurationWidget)
{
    ui->setupUi(this);

    // Scope styles to this widget (keeps your existing dark palette)
    if (objectName().isEmpty())
        setObjectName("WorkListConfigurationWidget");

    // ---------- Styles ----------
    const char* lineCss =
        "QLineEdit {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "  width: 26px;"
        "}"
        /* red border for invalid inputs (applies to any QLineEdit with property invalid=true) */
        "QLineEdit[invalid=\"true\"] {"
        "  border: 1px solid rgb(200,80,80);"
        "}";

    const char* spinCss =
        "QSpinBox{"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "}";

    const char* listCss =
        "QListView{"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "}";

    const char* checkCss =
        "QCheckBox { spacing: 8px; }"
        "QCheckBox::indicator { width:16px; height:16px; }"
        "QCheckBox::indicator:unchecked {"
        "  border:1px solid rgb(120,120,120); background:rgb(74,74,74); border-radius:3px; }"
        "QCheckBox::indicator:unchecked:hover { border-color: rgb(160,160,160); }"
        "QCheckBox::indicator:checked {"
        "  border:1px solid rgb(160,160,160); background:rgb(54,69,79); border-radius:3px; }";

    const char* groupCss =
        "QGroupBox {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 6px;"
        "  margin-top: 18px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 10px;"
        "  padding: 0 6px;"
        "}";

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


	ui->updateTagBtn->setStyleSheet(buttonCss);
    ui->testConnectionBtn->setStyleSheet(buttonCss);

    ui->modalityComboBox->setStyleSheet(comboCss);
	ui->scopeComboBox->setStyleSheet(comboCss);

    ui->patientIDCheckBox->setStyleSheet(checkCss);
    ui->accessionNumberCheckBox->setStyleSheet(checkCss);
    ui->otherPatientIDCheckBox->setStyleSheet(checkCss);
    ui->admissionIDCheckBox->setStyleSheet(checkCss);
    ui->scheduledProcedureStepIDCheckBox->setStyleSheet(checkCss);
    ui->requestedProcedureIDCheckBox->setStyleSheet(checkCss);
    ui->requestedProcedureCodeValueCheckBox->setStyleSheet(checkCss);
    ui->studyInstanceUIDCheckBox->setStyleSheet(checkCss);
    ui->enableWorkListCheckBox->setStyleSheet(checkCss);
    ui->ignoreEchoFailCheckBox->setStyleSheet(checkCss);

    ui->hostNameLineEdit->setStyleSheet(lineCss);
    ui->hostIPLineEdit->setStyleSheet(lineCss);
    ui->localAETitleLineEdit->setStyleSheet(lineCss);
    ui->remoteAETitleLineEdit->setStyleSheet(lineCss);
    ui->groupNameLineEdit->setStyleSheet(lineCss);
    ui->dicomGroupLineEdit->setStyleSheet(lineCss);
    ui->dicomElementLineEdit->setStyleSheet(lineCss);
    ui->parentNodeGroupLineEdit->setStyleSheet(lineCss);
    ui->parentNodeELementLineEdit->setStyleSheet(lineCss);

    ui->hostPortSpinBox->setStyleSheet(spinCss);
    ui->fieldListView->setStyleSheet(listCss);
    ui->worklistConnectionGroupBox->setStyleSheet(groupCss);
    ui->worklistGroupBox->setStyleSheet(groupCss);
    ui->workListConfigurationGroupBox->setStyleSheet(groupCss);
    ui->workListFieldGroupBox->setStyleSheet(groupCss);

    // ---------- Port spinbox ----------
    ui->hostPortSpinBox->setRange(1, 65535);
    ui->hostPortSpinBox->setValue(100);

    // ---------- IPv4 validation on Host IP ----------
    ui->hostIPLineEdit->setValidator(new Ipv4Validator(ui->hostIPLineEdit));
    ui->hostIPLineEdit->installEventFilter(this);
    connect(ui->hostIPLineEdit, &QLineEdit::textChanged, this, [this](const QString&) {
        setInvalid(ui->hostIPLineEdit, false);
        });

    // ---------- Build the list (DisplayName) ----------
    auto* model = new QStandardItemModel(ui->fieldListView);
    model->setColumnCount(1);
    for (const auto& t : m_dicomTags) {
        const QString label = t.DisplayName.isEmpty() ? t.Name : t.DisplayName;
        auto* item = new QStandardItem(label);
        item->setEditable(false);
        model->appendRow(item);
    }
    ui->fieldListView->setModel(model);
    ui->fieldListView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->fieldListView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // ---------- Details are editable ----------
    ui->groupNameLineEdit->setReadOnly(false);
    ui->dicomGroupLineEdit->setReadOnly(false);
    ui->dicomElementLineEdit->setReadOnly(false);
    ui->parentNodeGroupLineEdit->setReadOnly(false);
    ui->parentNodeELementLineEdit->setReadOnly(false);

    // Hex validators (accept optional 0x + 1..4 hex digits). Also normalizes on editingFinished.
    applyHexValidators();

    // ---------- Selection → showRow ----------
    auto* sel = ui->fieldListView->selectionModel();
    connect(sel, &QItemSelectionModel::currentChanged, this,
        [this](const QModelIndex& current, const QModelIndex&) {
            const int row = current.isValid() ? current.row() : -1;
            if (row == m_currentRow) return;
            m_currentRow = row;
            showRow(m_currentRow);
        });

    // Initialize selection
    if (model->rowCount() > 0) {
        ui->fieldListView->setCurrentIndex(model->index(0, 0)); // triggers showRow
    }
    else {
        showRow(-1);
    }

    // ---------- Update button ----------
    connect(ui->updateTagBtn, &QPushButton::clicked,
        this, &WorkListConfigurationWidget::onUpdateTagClicked);

    // Assuming mainVerticalLayout has 2 child layouts or widgets stacked vertically
    ui->mainVerticalLayout->setStretch(0, 2);  // first row gets 1 part
    ui->mainVerticalLayout->setStretch(1, 3);  // second row gets 3 parts

}

void WorkListConfigurationWidget::applyHexValidators()
{
    // Optional 0x + 1..4 hex digits
    const QRegularExpression re(R"(^(?:0[xX])?[0-9A-Fa-f]{1,4}$)");
    auto* reqHex = new QRegularExpressionValidator(re, this);
    auto* optHex = new QRegularExpressionValidator(re, this);

    // Required hex: group & element
    ui->dicomGroupLineEdit->setValidator(reqHex);
    ui->dicomElementLineEdit->setValidator(reqHex);

    // Optional hex: parent group & element (can be empty)
    ui->parentNodeGroupLineEdit->setValidator(optHex);
    ui->parentNodeELementLineEdit->setValidator(optHex);

    // Normalize case/prefix on finish
    auto normalize = [this](QLineEdit* le, bool allowBlank) {
        connect(le, &QLineEdit::editingFinished, le, [this, le, allowBlank] {
            const QString t = le->text().trimmed();
            if (allowBlank && t.isEmpty()) { setInvalid(le, false); return; }
            uint16_t val{};
            if (parseHex(t, val)) {
                le->setText(formatHex(val));
                setInvalid(le, false);
            }
            else {
                setInvalid(le, true);
            }
            });
        connect(le, &QLineEdit::textChanged, le, [this, le] {
            // Clear error while typing if it could become valid
            setInvalid(le, false);
            });
        };

    normalize(ui->dicomGroupLineEdit, /*allowBlank*/false);
    normalize(ui->dicomElementLineEdit, /*allowBlank*/false);
    normalize(ui->parentNodeGroupLineEdit, /*allowBlank*/true);
    normalize(ui->parentNodeELementLineEdit, /*allowBlank*/true);
}

void WorkListConfigurationWidget::showRow(int row)
{
    // Discard any unsaved edits by reloading from model
    if (row < 0 || row >= m_dicomTags.size()) {
        ui->groupNameLineEdit->clear();
        ui->dicomGroupLineEdit->clear();
        ui->dicomElementLineEdit->clear();
        ui->parentNodeGroupLineEdit->clear();
        ui->parentNodeELementLineEdit->clear();
        setInvalid(ui->dicomGroupLineEdit, false);
        setInvalid(ui->dicomElementLineEdit, false);
        setInvalid(ui->parentNodeGroupLineEdit, false);
        setInvalid(ui->parentNodeELementLineEdit, false);
        return;
    }

    const auto& t = m_dicomTags.at(row);
    ui->groupNameLineEdit->setText(t.Name);
    ui->dicomGroupLineEdit->setText(formatHex(t.GroupHex));
    ui->dicomElementLineEdit->setText(formatHex(t.ElementHex));
    ui->parentNodeGroupLineEdit->setText(formatHex(t.PgroupHex, /*blankIfZero*/true));
    ui->parentNodeELementLineEdit->setText(formatHex(t.PelementHex, /*blankIfZero*/true));

    // clear error flags
    setInvalid(ui->dicomGroupLineEdit, false);
    setInvalid(ui->dicomElementLineEdit, false);
    setInvalid(ui->parentNodeGroupLineEdit, false);
    setInvalid(ui->parentNodeELementLineEdit, false);
}

void WorkListConfigurationWidget::onUpdateTagClicked()
{
    if (m_currentRow < 0 || m_currentRow >= m_dicomTags.size())
        return;

    // Read & validate
    DicomTag updated = m_dicomTags.at(m_currentRow);
    updated.Name = ui->groupNameLineEdit->text().trimmed();

    uint16_t g{}, e{}, pg{}, pe{};
    bool ok = true;

    if (!parseHex(ui->dicomGroupLineEdit->text(), g)) {
        setInvalid(ui->dicomGroupLineEdit, true); ok = false;
    }
    if (!parseHex(ui->dicomElementLineEdit->text(), e)) {
        setInvalid(ui->dicomElementLineEdit, true); ok = false;
    }

    const QString pgText = ui->parentNodeGroupLineEdit->text().trimmed();
    if (!pgText.isEmpty() && !parseHex(pgText, pg)) {
        setInvalid(ui->parentNodeGroupLineEdit, true); ok = false;
    }

    const QString peText = ui->parentNodeELementLineEdit->text().trimmed();
    if (!peText.isEmpty() && !parseHex(peText, pe)) {
        setInvalid(ui->parentNodeELementLineEdit, true); ok = false;
    }

    if (!ok) {
        // Optional: small tooltip near the first invalid field
        QWidget* firstBad =
            ui->dicomGroupLineEdit->property("invalid").toBool() ? static_cast<QWidget*>(ui->dicomGroupLineEdit) :
            ui->dicomElementLineEdit->property("invalid").toBool() ? static_cast<QWidget*>(ui->dicomElementLineEdit) :
            ui->parentNodeGroupLineEdit->property("invalid").toBool() ? static_cast<QWidget*>(ui->parentNodeGroupLineEdit) :
            ui->parentNodeELementLineEdit->property("invalid").toBool() ? static_cast<QWidget*>(ui->parentNodeELementLineEdit) :
            nullptr;

        if (firstBad) {
            QToolTip::showText(firstBad->mapToGlobal(QPoint(0, firstBad->height())),
                tr("Use hex: 0x0000–0xFFFF (parents may be empty)."),
                firstBad);
            firstBad->setFocus();
        }
        return; // do not emit signal
    }

    // Commit to updated tag
    updated.GroupHex = g;
    updated.ElementHex = e;
    updated.PgroupHex = pg;
    updated.PelementHex = pe;

    // Keep optional 0x formatting in UI normalized
    ui->dicomGroupLineEdit->setText(formatHex(g));
    ui->dicomElementLineEdit->setText(formatHex(e));
    ui->parentNodeGroupLineEdit->setText(formatHex(pg, /*blankIfZero*/true));
    ui->parentNodeELementLineEdit->setText(formatHex(pe, /*blankIfZero*/true));

    // (Optional) update our local cache so reselecting shows the new values
    m_dicomTags[m_currentRow] = updated;

    // (Optional) refresh list label if you want it to reflect Name when DisplayName is empty
    if (auto* model = qobject_cast<QStandardItemModel*>(ui->fieldListView->model())) {
        const QString label = m_dicomTags[m_currentRow].DisplayName.isEmpty()
            ? updated.Name
            : m_dicomTags[m_currentRow].DisplayName;
        model->setData(model->index(m_currentRow, 0), label);
    }

    // Emit the signal so your delegate/repo can persist it
    emit tagUpdateRequested(updated);
}


void WorkListConfigurationWidget::setInvalid(QWidget* w, bool invalid)
{
    w->setProperty("invalid", invalid);
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
}

QString WorkListConfigurationWidget::formatHex(uint16_t v, bool blankIfZero)
{
    if (blankIfZero && v == 0) return {};
    return QString("0x%1").arg(v, 4, 16, QLatin1Char('0')).toUpper();
}

// Accepts: "0x0010", "0010", "10", case-insensitive; trims spaces
bool WorkListConfigurationWidget::parseHex(const QString& text, uint16_t& out)
{
    QString t = text.trimmed();
    if (t.startsWith("0x", Qt::CaseInsensitive))
        t = t.mid(2);

    if (t.isEmpty() || t.size() > 4) return false;

    bool ok = false;
    const uint v = t.toUInt(&ok, 16);
    if (!ok || v > 0xFFFFu) return false;

    out = static_cast<uint16_t>(v);
    return true;
}



WorkListConfigurationWidget::~WorkListConfigurationWidget()
{
    delete ui;
}

bool WorkListConfigurationWidget::eventFilter(QObject* obj, QEvent* ev)
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