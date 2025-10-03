#include "DapConfigurationDelegate.h"
#include "DapConfigurationDelegate.h"
#include "DapConfigurationDelegate.h"
#include "DapConfigurationDelegate.h"
#include "DapConfigurationDelegate.h"
#include "DapConfigurationDelegate.h"

namespace Etrek::Device::Delegate
{
	DapConfigurationDelegate::DapConfigurationDelegate(QWidget* widget, QObject* parent)
		: QObject(parent), m_widget(widget)
	{
	}

	DapConfigurationDelegate::~DapConfigurationDelegate()
	{
	}

	QString DapConfigurationDelegate::name() const
	{
		return QString();
	}

	void DapConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}

	void DapConfigurationDelegate::apply()
	{
	}

	void DapConfigurationDelegate::accept()
	{
	}

	void DapConfigurationDelegate::reject()
	{
	}


}
