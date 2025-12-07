#ifndef EXPOSUREAPPLICATIONCONTROLWIDGET_H
#define EXPOSUREAPPLICATIONCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class ExposureApplicationControlWidget;
}

enum class ApplicationMode {
    Maintenance,
    Diagnostic
};

class ExposureApplicationControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExposureApplicationControlWidget(QWidget *parent = nullptr);
    ~ExposureApplicationControlWidget();

    /**
     * @brief Sets the application mode.
     * @param mode The application mode to select (Maintenance or Diagnostic).
     */
    void setApplicationMode(ApplicationMode mode);

    /**
     * @brief Gets the currently selected application mode.
     * @return The selected application mode.
     */
    ApplicationMode getApplicationMode() const;

signals:
    /**
     * @brief Emitted when the application mode changes.
     * @param mode The newly selected application mode.
     */
    void applicationModeChanged(ApplicationMode mode);

private:
    Ui::ExposureApplicationControlWidget *ui;
};

#endif // EXPOSUREAPPLICATIONCONTROLWIDGET_H
