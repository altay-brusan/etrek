#include "ConnectionSetupDelegate.h"

namespace Etrek::Device::Delegate
{
	ConnectionSetupDelegate::ConnectionSetupDelegate(
		QWidget* widget,
		std::weak_ptr<Etrek::Context::IContextManager> contextManager,
		QObject* parent)
		: QObject(parent), m_widget(widget), m_contextManager(contextManager)
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

