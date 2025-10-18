#include "ConnectionSetupDelegate.h"

namespace Etrek::Device::Delegate
{
	ConnectionSetupDelegate::ConnectionSetupDelegate(QWidget* widget, QObject* parent)
		: QObject(parent), m_widget(widget)
	{
	}

	ConnectionSetupDelegate::~ConnectionSetupDelegate()
	{
	}
	QString Etrek::Device::Delegate::ConnectionSetupDelegate::name() const
	{
		return QString();
	}
	void ConnectionSetupDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}
	void ConnectionSetupDelegate::apply()
	{
	}
	void ConnectionSetupDelegate::accept()
	{
	}
	void ConnectionSetupDelegate::reject()
	{
	}
}

