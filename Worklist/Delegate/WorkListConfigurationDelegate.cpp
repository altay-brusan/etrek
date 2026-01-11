#include "WorkListConfigurationDelegate.h"
#include "WorkListConfigurationWidget.h"
#include "WorklistRepository.h"

namespace Etrek::Worklist::Delegate
{
	using Etrek::Worklist::Repository::WorklistRepository;

	WorkListConfigurationDelegate::WorkListConfigurationDelegate(
		WorkListConfigurationWidget* widget,
		std::shared_ptr<WorklistRepository> repository,
		QObject* parent)
		: QObject(parent), m_widget(widget), m_repository(repository)
	{
	}

	WorkListConfigurationDelegate::~WorkListConfigurationDelegate()
	{
	}

	QString WorkListConfigurationDelegate::name() const
	{
		return QStringLiteral("WorkListConfigurationDelegate");
	}

	void WorkListConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
		Q_UNUSED(delegates);
	}

	void WorkListConfigurationDelegate::apply()
	{
		// TODO: Implement when WorkListConfigurationWidget has getter methods
		// Example: auto tags = m_widget->getTags();
		// for (const auto& tag : tags) {
		//     m_repository->updateTag(tag);
		// }
	}

	void WorkListConfigurationDelegate::accept()
	{
		apply();
	}

	void WorkListConfigurationDelegate::reject()
	{
	}
}
