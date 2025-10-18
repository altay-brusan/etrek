#ifndef WORKFLOWCONFIGURATIONWIDGET_H
#define WORKFLOWCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include "GeneralEquipment.h"
#include "Institution.h"
#include "EnvironmentSetting.h"

namespace Ui {
class WorkflowConfigurationWidget;
}

class WorkflowConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WorkflowConfigurationWidget(const Etrek::Device::Data::Entity::EnvironmentSetting& environmentSetting,const QVector<Etrek::Device::Data::Entity::GeneralEquipment>& equipments, const QVector<Etrek::Device::Data::Entity::Institution> & institutes, QWidget *parent = nullptr);
    ~WorkflowConfigurationWidget();

private:
    void setStile();
    void populateFromFirstEquipment();
    void populateEnvironmentSettingsUI();
    Ui::WorkflowConfigurationWidget *ui;
    QVector<Etrek::Device::Data::Entity::GeneralEquipment>   m_equipments;      // all equipments
	QVector<Etrek::Device::Data::Entity::Institution>       m_institutions;    // all institutions
	Etrek::Device::Data::Entity::EnvironmentSetting		  m_currentSettings; // current settings
};

#endif // WORKFLOWCONFIGURATIONWIDGET_H
