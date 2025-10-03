#ifndef WORKFLOWCONFIGURATIONWIDGET_H
#define WORKFLOWCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include "GeneralEquipment.h"
#include "Institution.h"
#include "EnvironmentSetting.h"

using namespace Etrek::Device::Data::Entity;

namespace Ui {
class WorkflowConfigurationWidget;
}

class WorkflowConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WorkflowConfigurationWidget(const EnvironmentSetting& environmentSetting,const QVector<GeneralEquipment>& equipments, const QVector<Institution> & institutes, QWidget *parent = nullptr);
    ~WorkflowConfigurationWidget();

private:
    void setStile();
    void populateFromFirstEquipment();
    void populateEnvironmentSettingsUI();
    Ui::WorkflowConfigurationWidget *ui;
    QVector<GeneralEquipment>   m_equipments;      // all equipments
	QVector<Institution>       m_institutions;    // all institutions
	EnvironmentSetting		  m_currentSettings; // current settings
};

#endif // WORKFLOWCONFIGURATIONWIDGET_H
