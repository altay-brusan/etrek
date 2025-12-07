#ifndef EXPOSURECONTROLWIDGET_H
#define EXPOSURECONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class ExposureControlWidget;
}

class ExposureControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExposureControlWidget(QWidget *parent = nullptr);
    ~ExposureControlWidget();

    /**
     * @brief Sets the exposure parameters.
     * @param kvp Kilovolt peak value.
     * @param ma Milliamperes value.
     * @param time Exposure time in milliseconds.
     * @param mas Milliampere-seconds value.
     */
    void setExposureParameters(int kvp, int ma, int time, int mas);

    /**
     * @brief Sets individual parameter values.
     */
    void setKvp(int kvp);
    void setMa(int ma);
    void setTime(int time);
    void setMas(int mas);

    /**
     * @brief Gets current parameter values.
     */
    int getKvp() const;
    int getMa() const;
    int getTime() const;
    int getMas() const;

signals:
    /**
     * @brief Emitted when exposure parameters change.
     */
    void parametersChanged(int kvp, int ma, int time, int mas);

private:
    Ui::ExposureControlWidget *ui;
};

#endif // EXPOSURECONTROLWIDGET_H
