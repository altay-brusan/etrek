#include "PacsEntityConfigurationDelegate.h"
#include "PacsEntityConfigurationWidget.h"
#include "PacsNodeRepository.h"

namespace Etrek::Pacs::Delegate
{
	using Etrek::Pacs::Repository::PacsNodeRepository;

	PacsEntityConfigurationDelegate::PacsEntityConfigurationDelegate(
		PacsEntityConfigurationWidget* widget,
		std::shared_ptr<PacsNodeRepository> repository,
		QObject* parent)
		: QObject(parent), m_widget(widget), m_repository(repository)
	{
	}

	PacsEntityConfigurationDelegate::~PacsEntityConfigurationDelegate()
	{
	}

	QString PacsEntityConfigurationDelegate::name() const
	{
		return QStringLiteral("PacsEntityConfigurationDelegate");
	}

	void PacsEntityConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
		Q_UNUSED(delegates);
	}

	void PacsEntityConfigurationDelegate::apply()
	{
		// TODO: Implement when PacsEntityConfigurationWidget has getter methods
		// Example: auto nodes = m_widget->getPacsNodes();
		// for (const auto& node : nodes) {
		//     m_repository->updatePacsNode(node);
		// }
	}

	void PacsEntityConfigurationDelegate::accept()
	{
		apply();
	}

	void PacsEntityConfigurationDelegate::reject()
	{
	}
}


