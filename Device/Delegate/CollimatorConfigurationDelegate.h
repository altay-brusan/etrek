#pragma once

#include <QObject>
#include <QWidget>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Device::Delegate
{
	class CollimatorConfigurationDelegate :
		public QObject,
		public IDelegate,
		public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		CollimatorConfigurationDelegate(QWidget* widget, QObject* parent);

		QString name() const override;		
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~CollimatorConfigurationDelegate();
	private:


		void apply() override;
		void accept() override;
		void reject() override;



		QWidget* m_widget = nullptr;
	};

}

