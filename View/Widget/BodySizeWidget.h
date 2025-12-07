#ifndef BODYSIZEWIDGET_H
#define BODYSIZEWIDGET_H

#include <QWidget>

namespace Ui {
class BodySizeWidget;
}

enum class PatientSize {
    Infant,
    Small,
    Normal,
    Large
};

class BodySizeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BodySizeWidget(QWidget *parent = nullptr);
    ~BodySizeWidget();

    /**
     * @brief Sets the current patient size selection.
     * @param size The patient size to select.
     */
    void setPatientSize(PatientSize size);

    /**
     * @brief Gets the currently selected patient size.
     * @return The selected patient size.
     */
    PatientSize getPatientSize() const;

signals:
    /**
     * @brief Emitted when the patient size selection changes.
     * @param size The newly selected patient size.
     */
    void patientSizeChanged(PatientSize size);

private:
    Ui::BodySizeWidget *ui;
};

#endif // BODYSIZEWIDGET_H
