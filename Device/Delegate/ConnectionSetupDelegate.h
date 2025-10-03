#pragma once

#include <QObject>
#include <QWidget>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Device::Delegate
{
	class ConnectionSetupDelegate :
	public QObject,
	public IDelegate, 
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		ConnectionSetupDelegate(QWidget* widget, QObject* parent);

		// Inherited via IDelegate
		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~ConnectionSetupDelegate();
	private:
		QWidget* m_widget = nullptr;

		// Inherited via IPageAction
		void apply() override;
		void accept() override;
		void reject() override;
	};
}
