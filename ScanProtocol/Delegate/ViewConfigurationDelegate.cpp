#include "ViewConfigurationDelegate.h"
#include "ViewConfigurationDelegate.h"
#include "ViewConfigurationDelegate.h"
#include "ViewConfigurationDelegate.h"
#include "ViewConfigurationDelegate.h"
#include "ViewConfigurationDelegate.h"

namespace Etrek::ScanProtocol::Delegate
{
	ViewConfigurationDelegate::ViewConfigurationDelegate(QWidget* widget, QObject* parent)
		: QObject(parent), m_widget(widget)
	{
	}

	ViewConfigurationDelegate::~ViewConfigurationDelegate()
	{
	}

	QString Delegate::ViewConfigurationDelegate::name() const
	{
		return QString();
	}

	void Delegate::ViewConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}

	void Delegate::ViewConfigurationDelegate::apply()
	{
	}

	void Delegate::ViewConfigurationDelegate::accept()
	{
	}

	void Delegate::ViewConfigurationDelegate::reject()
	{
	}

}
