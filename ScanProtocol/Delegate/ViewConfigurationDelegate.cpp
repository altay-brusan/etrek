#include "ViewConfigurationDelegate.h"
#include "ViewConfigurationWidget.h"
#include "ScanProtocolRepository.h"
#include <QDebug>

namespace Etrek::ScanProtocol::Delegate
{
	using Etrek::ScanProtocol::Repository::ScanProtocolRepository;

	ViewConfigurationDelegate::ViewConfigurationDelegate(
		ViewConfigurationWidget* widget,
		std::shared_ptr<ScanProtocolRepository> repository,
		QObject* parent)
		: QObject(parent)
		, m_widget(widget)
		, m_repository(std::move(repository))
	{
	}

	ViewConfigurationDelegate::~ViewConfigurationDelegate()
	{
	}

	QString ViewConfigurationDelegate::name() const
	{
		return QStringLiteral("View Configuration");
	}

	void ViewConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
		Q_UNUSED(delegates)
	}

	void ViewConfigurationDelegate::apply()
	{
		saveAllViews();
	}

	void ViewConfigurationDelegate::accept()
	{
		saveAllViews();
	}

	void ViewConfigurationDelegate::reject()
	{
		// No action needed - changes are discarded
	}

	bool ViewConfigurationDelegate::saveAllViews()
	{
		if (!m_widget || !m_repository) {
			qWarning() << "[ViewConfigurationDelegate] Cannot save - widget or repository is null";
			return false;
		}

		const auto views = m_widget->getViews();
		qDebug() << "[ViewConfigurationDelegate] Saving" << views.size() << "views";

		bool allSuccess = true;
		for (const auto& view : views) {
			if (view.Id > 0) {
				// Existing view - update
				auto result = m_repository->updateView(view);
				if (!result.isSuccess) {
					qWarning() << "[ViewConfigurationDelegate] Failed to update view"
							   << view.Id << ":" << result.message;
					allSuccess = false;
				}
			} else {
				// New view - create
				auto result = m_repository->createView(view);
				if (!result.isSuccess) {
					qWarning() << "[ViewConfigurationDelegate] Failed to create view:"
							   << result.message;
					allSuccess = false;
				}
			}
		}

		if (allSuccess) {
			qDebug() << "[ViewConfigurationDelegate] All views saved successfully";
		}
		return allSuccess;
	}

}
