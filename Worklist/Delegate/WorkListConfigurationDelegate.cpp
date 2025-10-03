#include "WorkListConfigurationDelegate.h"
#include "WorkListConfigurationDelegate.h"
#include "WorkListConfigurationDelegate.h"
#include "WorkListConfigurationDelegate.h"
#include "WorkListConfigurationDelegate.h"
#include "WorkListConfigurationDelegate.h"

namespace Etrek::Worklist::Delegate 
{
	WorkListConfigurationDelegate::WorkListConfigurationDelegate(QWidget* widget, QObject* parent)
		: QObject(parent), m_widget(widget)
	{
	}

	WorkListConfigurationDelegate::~WorkListConfigurationDelegate()
	{
	}
	QString Etrek::Worklist::Delegate::WorkListConfigurationDelegate::name() const
	{
		return QString();
	}
	void Etrek::Worklist::Delegate::WorkListConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}
	void Etrek::Worklist::Delegate::WorkListConfigurationDelegate::apply()
	{
	}
	void Etrek::Worklist::Delegate::WorkListConfigurationDelegate::accept()
	{
	}
	void Etrek::Worklist::Delegate::WorkListConfigurationDelegate::reject()
	{
	}
}
