#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Device::Repository { class DeviceRepository; }
class CollimatorConfigurationWidget;

namespace Etrek::Device::Delegate
{
	namespace rpo = Etrek::Device::Repository;

	class CollimatorConfigurationDelegate :
		public QObject,
		public IDelegate,
		public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		CollimatorConfigurationDelegate(
			CollimatorConfigurationWidget* widget,
			std::shared_ptr<rpo::DeviceRepository> repository,
			QObject* parent);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~CollimatorConfigurationDelegate();
	private:
		CollimatorConfigurationWidget* m_widget = nullptr;
		std::shared_ptr<rpo::DeviceRepository> m_repository;

		void apply() override;
		void accept() override;
		void reject() override;
	};
}

