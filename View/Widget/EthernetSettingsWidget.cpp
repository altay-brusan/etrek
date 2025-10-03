#include <QValidator>
#include <QWidget>
#include <QStyle>
#include <QEvent>
#include <QToolTip>
#include "EthernetSettingsWidget.h"
#include "ui_EthernetSettingsWidget.h"


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

EthernetSettingsWidget::EthernetSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EthernetSettingsWidget)
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

    ui->ipAddressLineEdit->setStyleSheet(lineCss);

    // ---------- IPv4 validation on Host IP ----------
    ui->ipAddressLineEdit->setValidator(new Ipv4Validator(ui->ipAddressLineEdit));
    ui->ipAddressLineEdit->installEventFilter(this);
    connect(ui->ipAddressLineEdit, &QLineEdit::textChanged, this, [this](const QString&) {
        setInvalid(ui->ipAddressLineEdit, false);
        });

}

EthernetSettingsWidget::~EthernetSettingsWidget()
{
    delete ui;
}

void EthernetSettingsWidget::setInvalid(QWidget* w, bool invalid)
{
    w->setProperty("invalid", invalid);
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
}

bool EthernetSettingsWidget::eventFilter(QObject* obj, QEvent* ev)
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
