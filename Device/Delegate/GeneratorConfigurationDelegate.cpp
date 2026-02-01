#include "GeneratorConfigurationDelegate.h"

namespace Etrek::Device::Delegate
{
	using Etrek::Device::Repository::DeviceRepository;

	GeneratorConfigurationDelegate::GeneratorConfigurationDelegate(
		GeneratorConfigurationWidget* widget,
		std::shared_ptr<DeviceRepository> repository,
		std::weak_ptr<Etrek::Context::IContextManager> contextManager,
		QObject* parent)
		: QObject(parent), m_widget(widget), m_repository(repository), m_contextManager(contextManager)
	{
	}

	GeneratorConfigurationDelegate::~GeneratorConfigurationDelegate()
	{
	}
	QString GeneratorConfigurationDelegate::name() const
	{
		return QString();
	}
	void GeneratorConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}
	void GeneratorConfigurationDelegate::apply()
	{
	}
	void GeneratorConfigurationDelegate::accept()
	{
	}
	void GeneratorConfigurationDelegate::reject()
	{
	}
}


