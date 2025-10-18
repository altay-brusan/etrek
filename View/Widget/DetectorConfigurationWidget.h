#ifndef DETECTORCONFIGURATIONWIDGET_H
#define DETECTORCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include "Detector.h"

namespace Ui {
class DetectorConfigurationWidget;
}

class DetectorConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorConfigurationWidget(const QVector<Etrek::Device::Data::Entity::Detector>& nodes, QWidget *parent = nullptr);
    ~DetectorConfigurationWidget();

private:
    Ui::DetectorConfigurationWidget *ui;
    QVector<Etrek::Device::Data::Entity::Detector> m_nodes;
};

#endif // DETECTORCONFIGURATIONWIDGET_H
