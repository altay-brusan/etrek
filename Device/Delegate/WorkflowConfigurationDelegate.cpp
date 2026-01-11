#include "WorkflowConfigurationDelegate.h"
#include "WorkflowConfigurationWidget.h"
#include "DeviceRepository.h"

namespace Etrek::Device::Delegate
{
	using Etrek::Device::Repository::DeviceRepository;

	WorkflowConfigurationDelegate::WorkflowConfigurationDelegate(
		WorkflowConfigurationWidget* widget,
		std::shared_ptr<DeviceRepository> repository,
		QObject* parent)
		: QObject(parent), m_widget(widget), m_repository(repository)
	{
	}

	WorkflowConfigurationDelegate::~WorkflowConfigurationDelegate()
	{
	}

	QString WorkflowConfigurationDelegate::name() const
	{
		return QStringLiteral("WorkflowConfigurationDelegate");
	}

	void WorkflowConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
		Q_UNUSED(delegates);
	}

	void WorkflowConfigurationDelegate::apply()
	{
		// TODO: Implement when WorkflowConfigurationWidget has getter methods
		// Example: auto settings = m_widget->getEnvironmentSettings();
		// m_repository->updateEnvironmentSettings(settings);
	}

	void WorkflowConfigurationDelegate::accept()
	{
		apply();
	}

	void WorkflowConfigurationDelegate::reject()
	{
	}
}
