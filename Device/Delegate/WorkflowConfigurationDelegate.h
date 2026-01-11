#ifndef WORKFLOWCONFIGURATIONDELEGATE_H
#define WORKFLOWCONFIGURATIONDELEGATE_H

#include <QWidget>
#include <QObject>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Device::Repository { class DeviceRepository; }
class WorkflowConfigurationWidget;

namespace Etrek::Device::Delegate
{
	namespace rpo = Etrek::Device::Repository;

	class WorkflowConfigurationDelegate :
		public QObject,
		public IDelegate,
		public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		WorkflowConfigurationDelegate(
			WorkflowConfigurationWidget* widget,
			std::shared_ptr<rpo::DeviceRepository> repository,
			QObject* parent = nullptr);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~WorkflowConfigurationDelegate();

	private:
		WorkflowConfigurationWidget* m_widget = nullptr;
		std::shared_ptr<rpo::DeviceRepository> m_repository;

		void apply() override;
		void accept() override;
		void reject() override;
	};
}

#endif // WORKFLOWCONFIGURATIONDELEGATE_H
