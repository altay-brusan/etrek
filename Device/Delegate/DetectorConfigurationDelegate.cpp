#include "DetectorConfigurationDelegate.h"
#include "DetectorConfigurationDelegate.h"
#include "DetectorConfigurationDelegate.h"
#include "DetectorConfigurationDelegate.h"
#include "DetectorConfigurationDelegate.h"
#include "DetectorConfigurationDelegate.h"

namespace Etrek::Device::Delegate
{
	DetectorConfigurationDelegate::DetectorConfigurationDelegate(QWidget* widget, QObject* parent)
		: QObject(parent), m_widget(widget)
	{
	}

	DetectorConfigurationDelegate::~DetectorConfigurationDelegate()
	{
	}

	QString DetectorConfigurationDelegate::name() const
	{
		return QString();
	}

	void DetectorConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}

	void DetectorConfigurationDelegate::apply()
	{
	}

	void DetectorConfigurationDelegate::accept()
	{
	}

	void DetectorConfigurationDelegate::reject()
	{
	}


}
