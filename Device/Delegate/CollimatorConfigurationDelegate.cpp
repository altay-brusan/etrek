#include "CollimatorConfigurationDelegate.h"
#include "CollimatorConfigurationWidget.h"
#include "DeviceRepository.h"

namespace Etrek::Device::Delegate
{
	using Etrek::Device::Repository::DeviceRepository;

	CollimatorConfigurationDelegate::CollimatorConfigurationDelegate(
		CollimatorConfigurationWidget* widget,
		std::shared_ptr<DeviceRepository> repository,
		std::weak_ptr<Etrek::Context::IContextManager> contextManager,
		QObject* parent)
		: QObject(parent), m_widget(widget), m_repository(repository), m_contextManager(contextManager)
	{
	}

	CollimatorConfigurationDelegate::~CollimatorConfigurationDelegate()
	{
	}

	QString CollimatorConfigurationDelegate::name() const
	{
		return QStringLiteral("CollimatorConfigurationDelegate");
	}

	void CollimatorConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
		Q_UNUSED(delegates);
	}

	void CollimatorConfigurationDelegate::apply()
	{
		// TODO: Implement when CollimatorConfigurationWidget has getter methods
		// Example: auto collimator = m_widget->getCollimatorData();
		// m_repository->updateCollimator(collimator);
	}

	void CollimatorConfigurationDelegate::accept()
	{
		apply();
	}

	void CollimatorConfigurationDelegate::reject()
	{
	}
}

