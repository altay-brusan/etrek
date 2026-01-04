#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::ScanProtocol::Repository { class ScanProtocolRepository; }
class ProcedureConfigurationWidget;

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
		ProcedureConfigurationDelegate(
			ProcedureConfigurationWidget* widget,
			std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> repository,
			QObject* parent);

		QString name() const override;
		void attachDelegates(const QVector<QObject*>& delegates) override;

		~ProcedureConfigurationDelegate();

	private:
		ProcedureConfigurationWidget* m_widget;
		std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> m_repository;

		// Inherited via IPageAction
		void apply() override;
		void accept() override;
		void reject() override;

		bool saveAllProcedures();
	};

}