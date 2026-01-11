#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Pacs::Repository { class PacsNodeRepository; }
class PacsEntityConfigurationWidget;

namespace Etrek::Pacs::Delegate
{
	namespace rpo = Etrek::Pacs::Repository;

	class PacsEntityConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		PacsEntityConfigurationDelegate(
			PacsEntityConfigurationWidget* widget,
			std::shared_ptr<rpo::PacsNodeRepository> repository,
			QObject* parent);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~PacsEntityConfigurationDelegate();
	private:
		PacsEntityConfigurationWidget* m_widget = nullptr;
		std::shared_ptr<rpo::PacsNodeRepository> m_repository;

		void apply() override;
		void accept() override;
		void reject() override;
	};
}

