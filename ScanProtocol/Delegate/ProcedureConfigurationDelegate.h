#pragma once

#include <QObject>
#include <QWidget>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::ScanProtocol::Delegate
{
	class ProcedureConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		ProcedureConfigurationDelegate(QWidget* widget, QObject* parent);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~ProcedureConfigurationDelegate();
	private:
		QWidget* m_widget;


		// Inherited via IPageAction
		void apply() override;
		void accept() override;
		void reject() override;
	};

}