#pragma once

#include <QObject>
#include <QWidget>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::ScanProtocol::Delegate
{
	class ViewConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)
	public:
		ViewConfigurationDelegate(QWidget* widget, QObject* parent);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~ViewConfigurationDelegate();

	private:

		QWidget* m_widget = nullptr;
		void apply() override;
		void accept() override;
		void reject() override;
	};

}
