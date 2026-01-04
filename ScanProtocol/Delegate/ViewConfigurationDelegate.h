#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::ScanProtocol::Repository { class ScanProtocolRepository; }
class ViewConfigurationWidget;

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
		ViewConfigurationDelegate(
			ViewConfigurationWidget* widget,
			std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> repository,
			QObject* parent);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~ViewConfigurationDelegate();

	private:
		ViewConfigurationWidget* m_widget = nullptr;
		std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> m_repository;

		void apply() override;
		void accept() override;
		void reject() override;

		bool saveAllViews();
	};

}
