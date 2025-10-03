#ifndef DETECTORCONFIGURATIONWIDGET_H
#define DETECTORCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include "Detector.h"


using namespace Etrek::Device::Data::Entity;
namespace Ui {
class DetectorConfigurationWidget;
}

class DetectorConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorConfigurationWidget(const QVector<Detector>& nodes, QWidget *parent = nullptr);
    ~DetectorConfigurationWidget();

private:
    Ui::DetectorConfigurationWidget *ui;
    QVector<Detector> m_nodes;
};

#endif // DETECTORCONFIGURATIONWIDGET_H
