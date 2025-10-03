#ifndef WORKLISTCONFIGURATIONWIDGET_H
#define WORKLISTCONFIGURATIONWIDGET_H

#include <QWidget>
#include "DicomTag.h"

using namespace Etrek::Worklist::Data::Entity;

namespace Ui {
class WorkListConfigurationWidget;
}

class WorkListConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WorkListConfigurationWidget(const QList<DicomTag>& dicomTags, QWidget* parent = nullptr);
    ~WorkListConfigurationWidget();

signals:
    // Emitted ONLY when user presses "Update Tag" button
    void tagUpdateRequested(const DicomTag& updated);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private slots:
    void onUpdateTagClicked();

private:
    Ui::WorkListConfigurationWidget* ui;
    QList<DicomTag> m_dicomTags;
    int m_currentRow = -1;

    // Helpers
    static QString formatHex(uint16_t v, bool blankIfZero = false);
    static bool parseHex(const QString& text, uint16_t& out); // accepts optional 0x / 0X
    void applyHexValidators();
    void showRow(int row);
    void setInvalid(QWidget* w, bool invalid);
};

#endif // WORKLISTCONFIGURATIONWIDGET_H
