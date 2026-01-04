#include "ProcedureConfigurationDelegate.h"
#include "ProcedureConfigurationWidget.h"
#include "ScanProtocolRepository.h"
#include <QDebug>

namespace Etrek::ScanProtocol::Delegate
{
	using Etrek::ScanProtocol::Repository::ScanProtocolRepository;

	ProcedureConfigurationDelegate::ProcedureConfigurationDelegate(
		ProcedureConfigurationWidget* widget,
		std::shared_ptr<ScanProtocolRepository> repository,
		QObject* parent)
		: QObject(parent)
		, m_widget(widget)
		, m_repository(std::move(repository))
	{
	}

	ProcedureConfigurationDelegate::~ProcedureConfigurationDelegate()
	{
	}

	QString ProcedureConfigurationDelegate::name() const
	{
		return QStringLiteral("Procedure Configuration");
	}

	void ProcedureConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
		Q_UNUSED(delegates)
	}

	void ProcedureConfigurationDelegate::apply()
	{
		saveAllProcedures();
	}

	void ProcedureConfigurationDelegate::accept()
	{
		saveAllProcedures();
	}

	void ProcedureConfigurationDelegate::reject()
	{
		// No action needed - changes are discarded
	}

	bool ProcedureConfigurationDelegate::saveAllProcedures()
	{
		if (!m_widget || !m_repository) {
			qWarning() << "[ProcedureConfigurationDelegate] Cannot save - widget or repository is null";
			return false;
		}

		const auto procedures = m_widget->getProcedures();
		qDebug() << "[ProcedureConfigurationDelegate] Saving" << procedures.size() << "procedures";

		bool allSuccess = true;
		for (const auto& proc : procedures) {
			if (proc.Id > 0) {
				// Existing procedure - update
				auto result = m_repository->updateProcedure(proc);
				if (!result.isSuccess) {
					qWarning() << "[ProcedureConfigurationDelegate] Failed to update procedure"
							   << proc.Id << ":" << result.message;
					allSuccess = false;
				}
			} else {
				// New procedure - create
				auto result = m_repository->createProcedure(proc);
				if (!result.isSuccess) {
					qWarning() << "[ProcedureConfigurationDelegate] Failed to create procedure:"
							   << result.message;
					allSuccess = false;
				}
			}
		}

		if (allSuccess) {
			qDebug() << "[ProcedureConfigurationDelegate] All procedures saved successfully";
		}
		return allSuccess;
	}

}
