#include "PacsEntityConfigurationDelegate.h"
#include "PacsEntityConfigurationDelegate.h"
#include "PacsEntityConfigurationDelegate.h"
#include "PacsEntityConfigurationDelegate.h"
#include "PacsEntityConfigurationDelegate.h"
#include "PacsEntityConfigurationDelegate.h"

namespace Etrek::Pacs::Delegate
{
	PacsEntityConfigurationDelegate::PacsEntityConfigurationDelegate(QWidget* widget, QObject* parent)
		: QObject(parent), m_widget(widget)
	{
	}

	PacsEntityConfigurationDelegate::~PacsEntityConfigurationDelegate()
	{
	}

	QString PacsEntityConfigurationDelegate::name() const
	{
		return QString();
	}

	void PacsEntityConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}

	void PacsEntityConfigurationDelegate::apply()
	{
	}

	void PacsEntityConfigurationDelegate::accept()
	{
	}

	void PacsEntityConfigurationDelegate::reject()
	{
	}

}


