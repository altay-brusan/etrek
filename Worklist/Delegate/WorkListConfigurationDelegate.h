#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Worklist::Repository { class WorklistRepository; }
class WorkListConfigurationWidget;

namespace Etrek::Worklist::Delegate
{
	namespace rpo = Etrek::Worklist::Repository;

	class WorkListConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		WorkListConfigurationDelegate(
			WorkListConfigurationWidget* widget,
			std::shared_ptr<rpo::WorklistRepository> repository,
			QObject* parent);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~WorkListConfigurationDelegate();
	private:
		WorkListConfigurationWidget* m_widget = nullptr;
		std::shared_ptr<rpo::WorklistRepository> m_repository;

		void apply() override;
		void accept() override;
		void reject() override;
	};
}

