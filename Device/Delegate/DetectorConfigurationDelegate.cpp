#include "DetectorConfigurationDelegate.h"
#include "DetectorConfigurationWidget.h"
#include "DeviceRepository.h"

namespace Etrek::Device::Delegate
{
	using Etrek::Device::Repository::DeviceRepository;

	DetectorConfigurationDelegate::DetectorConfigurationDelegate(
		DetectorConfigurationWidget* widget,
		std::shared_ptr<DeviceRepository> repository,
		std::weak_ptr<Etrek::Context::IContextManager> contextManager,
		QObject* parent)
		: QObject(parent), m_widget(widget), m_repository(repository), m_contextManager(contextManager)
	{
	}

	DetectorConfigurationDelegate::~DetectorConfigurationDelegate()
	{
	}

	QString DetectorConfigurationDelegate::name() const
	{
		return QStringLiteral("DetectorConfigurationDelegate");
	}

	void DetectorConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
		Q_UNUSED(delegates);
	}

	void DetectorConfigurationDelegate::apply()
	{
		// TODO: Implement when DetectorConfigurationWidget has getter methods
		// Example: auto detectors = m_widget->getDetectors();
		// for (const auto& detector : detectors) {
		//     m_repository->updateDetector(detector);
		// }
	}

	void DetectorConfigurationDelegate::accept()
	{
		apply();
	}

	void DetectorConfigurationDelegate::reject()
	{
	}
}
