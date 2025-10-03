#ifndef SYSTEMSETTINGPAGE_H
#define SYSTEMSETTINGPAGE_H

#include <QWidget>
#include <QBoxlayout>
#include <QCloseEvent>

namespace Ui {
class SystemSettingPage;
}

class SystemSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSettingPage(QWidget *parent = nullptr);
	void closePage();

	void setWorklistFieldConfigurationWidget(QWidget* widget);
	void setWorkflowConfigurationWidget(QWidget* widget);
    void setConnectionSetupWidget(QWidget* widget);
    void setImageCommentWidget(QWidget* widget);
	void setDetectorConfigurationWidget(QWidget* widget);
	void setGeneratorConfigurationWidget(QWidget* widget);
	void setDapConfigurationWidget(QWidget* widget);
	void setCollimatorConfigurationWidget(QWidget* widget);
	void setWorkListConfigurationWidget(QWidget* widget);
	void setPacsEntityConfigurationWidget(QWidget* widget);
	void setProcedureConfigurationWidget(QWidget* widget);
	void setViewConfigurationWidget(QWidget* widget);
	void setTechniqueConfigurationWidget(QWidget* widget);

    ~SystemSettingPage();

signals:
	void saveSettings();
	void applySettings();
	void closeSettings();

private:
    Ui::SystemSettingPage *ui;
	QVBoxLayout* m_worklistFieldConfigurationPlaceholder;
    QVBoxLayout* m_workflowConfigurationPlaceholder;
    QVBoxLayout* m_connectionSetupPlaceholder;
	QVBoxLayout* m_imageCommentPlaceholder;
	QVBoxLayout* m_detectorConfigurationPlaceholder;
    QVBoxLayout* m_generatorConfigurationPlaceholder;
    QVBoxLayout* m_dapConfigurationPlaceholder;
    QVBoxLayout* m_collimatorConfigurationPlaceholder;
	QVBoxLayout* m_workListConfigurationPlaceholder;
	QVBoxLayout* m_pacsEntityConfigurationPlaceholder;
	QVBoxLayout* m_procedureConfigurationPlaceholder;
	QVBoxLayout* m_viewConfigureationPlaceholder;
	QVBoxLayout* m_techniqueConfigurationPlaceholder;
	
};

#endif // SYSTEMSETTINGPAGE_H
