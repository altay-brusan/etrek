#include "DapConfigurationDelegate.h"
#include "DapConfigurationWidget.h"
#include "DeviceRepository.h"

namespace Etrek::Device::Delegate
{
	using Etrek::Device::Repository::DeviceRepository;

	DapConfigurationDelegate::DapConfigurationDelegate(
		DapConfigurationWidget* widget,
		std::shared_ptr<DeviceRepository> repository,
		QObject* parent)
		: QObject(parent), m_widget(widget), m_repository(repository)
	{
	}

	DapConfigurationDelegate::~DapConfigurationDelegate()
	{
	}

	QString DapConfigurationDelegate::name() const
	{
		return QStringLiteral("DapConfigurationDelegate");
	}

	void DapConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
		Q_UNUSED(delegates);
	}

	void DapConfigurationDelegate::apply()
	{
		// TODO: Implement when DapConfigurationWidget has getter methods
		// Example: auto dapData = m_widget->getDapData();
		// m_repository->updateDap(dapData);
	}

	void DapConfigurationDelegate::accept()
	{
		apply();
	}

	void DapConfigurationDelegate::reject()
	{
	}
}
