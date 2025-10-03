#pragma once

#include <QObject>
#include <QWidget>
#include "IDelegate.h"
#include "IPageAction.h"
#include "DeviceRepository.h"
#include "GeneratorConfigurationWidget.h"

using namespace Etrek::Device::Repository;

namespace Etrek::Device::Delegate
{
	class GeneratorConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		GeneratorConfigurationDelegate(GeneratorConfigurationWidget* widget, std::shared_ptr<DeviceRepository> repository,QObject* parent);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~GeneratorConfigurationDelegate();
	private:
		GeneratorConfigurationWidget* m_widget;
		std::shared_ptr<DeviceRepository> m_repository;

		// Inherited via IPageAction
		void apply() override;
		void accept() override;
		void reject() override;
	};

}