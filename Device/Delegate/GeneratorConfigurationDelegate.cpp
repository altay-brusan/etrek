#include "GeneratorConfigurationDelegate.h"

namespace Etrek::Device::Delegate
{
	GeneratorConfigurationDelegate::GeneratorConfigurationDelegate(GeneratorConfigurationWidget* widget, std::shared_ptr<DeviceRepository> repository, QObject* parent)
		: QObject(parent), m_widget(widget), m_repository(repository)
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


