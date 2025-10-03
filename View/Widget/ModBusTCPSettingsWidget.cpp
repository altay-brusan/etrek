#include <QValidator>
#include <QWidget>
#include <QStyle>
#include <QComboBox>
#include <QItemSelectionModel>
#include <QRegularExpression>
#include <QMessageBox>
#include <QSpinBox>
#include <QValidator>
#include <QToolTip>
#include <QPoint>
#include <QEvent>

#include "ModBusTCPSettingsWidget.h"
#include "ui_ModBusTCPSettingsWidget.h"

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


ModBusTCPSettingsWidget::ModBusTCPSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModBusTCPSettingsWidget)
{
    ui->setupUi(this);

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
        "QLineEdit#ipAddressLineEdit[invalid=\"true\"] {"
        "  border:1px solid rgb(200,80,80);"
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

    const char* spinCss =
        "QSpinBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:28px; padding:2px 6px; }"
        "QSpinBox:focus { border-color: rgb(160,160,160); }";


    ui->ipAddressLineEdit->setStyleSheet(lineCss);
    ui->portSpinBox->setStyleSheet(spinCss);
    ui->unitIdLineEdit->setStyleSheet(lineCss);

    ui->portSpinBox->setRange(1, 65535);
    ui->portSpinBox->setValue(100);

    // --- IP Validator ---
    // Clear red while typing; set red if still invalid on finish
    ui->ipAddressLineEdit->setValidator(new Ipv4Validator(ui->ipAddressLineEdit));
    ui->ipAddressLineEdit->installEventFilter(this);
    connect(ui->ipAddressLineEdit, &QLineEdit::textChanged, this, [this](const QString&) {
        ui->ipAddressLineEdit->setProperty("invalid", false);
        ui->ipAddressLineEdit->style()->unpolish(ui->ipAddressLineEdit);
        ui->ipAddressLineEdit->style()->polish(ui->ipAddressLineEdit);
        });
    // after you connect textChanged (which clears the red)
    connect(ui->ipAddressLineEdit, &QLineEdit::editingFinished, this, [this] {
        const bool ok = ui->ipAddressLineEdit->hasAcceptableInput();
        ui->ipAddressLineEdit->setProperty("invalid", !ok);
        ui->ipAddressLineEdit->style()->unpolish(ui->ipAddressLineEdit);
        ui->ipAddressLineEdit->style()->polish(ui->ipAddressLineEdit);
        ui->ipAddressLineEdit->update();
        });



}

ModBusTCPSettingsWidget::~ModBusTCPSettingsWidget()
{
    delete ui;
}

void ModBusTCPSettingsWidget::setInvalid(QWidget* w, bool invalid)
{
    w->setProperty("invalid", invalid);
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
}


bool ModBusTCPSettingsWidget::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == ui->ipAddressLineEdit && ev->type() == QEvent::FocusOut) {
        const bool ok = ui->ipAddressLineEdit->hasAcceptableInput();

        ui->ipAddressLineEdit->setProperty("invalid", !ok);
        ui->ipAddressLineEdit->style()->unpolish(ui->ipAddressLineEdit);
        ui->ipAddressLineEdit->style()->polish(ui->ipAddressLineEdit);

        if (!ok) {
            // Friendly hint near the field
            QToolTip::showText(
                ui->ipAddressLineEdit->mapToGlobal(QPoint(0, ui->ipAddressLineEdit->height())),
                tr("Please enter a valid IPv4 address (e.g. 192.168.1.10)."),
                ui->ipAddressLineEdit
            );
            // Re-focus after the event completes
            QMetaObject::invokeMethod(ui->ipAddressLineEdit, "setFocus", Qt::QueuedConnection);
            return true; // swallow this FocusOut
        }
    }
    return QWidget::eventFilter(obj, ev);
}
