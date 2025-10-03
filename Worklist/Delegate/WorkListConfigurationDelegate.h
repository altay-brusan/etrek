#pragma once

#include <QObject>
#include <QWidget>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Worklist::Delegate {
	class WorkListConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)


	public:
		WorkListConfigurationDelegate(QWidget* widget, QObject* parent);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~WorkListConfigurationDelegate();
	private:

		QWidget* m_widget = nullptr;
		void apply() override;
		void accept() override;
		void reject() override;
	};
}

