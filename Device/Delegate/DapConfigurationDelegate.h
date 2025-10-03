#pragma once

#include <QObject>
#include <QWidget>
#include "IDelegate.h"
#include "IPageAction.h"


namespace Etrek::Device::Delegate
{
	class DapConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction	
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		DapConfigurationDelegate(QWidget* widget, QObject* parent);
		
		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;


		~DapConfigurationDelegate();
	private:
		QWidget* m_widget;

		void apply() override;
		void accept() override;
		void reject() override;
	};

}

