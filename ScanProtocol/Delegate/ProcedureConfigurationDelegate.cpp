#include "ProcedureConfigurationDelegate.h"
#include "ProcedureConfigurationDelegate.h"
#include "ProcedureConfigurationDelegate.h"
#include "ProcedureConfigurationDelegate.h"
#include "ProcedureConfigurationDelegate.h"
#include "ProcedureConfigurationDelegate.h"

namespace Etrek::ScanProtocol::Delegate
{
	ProcedureConfigurationDelegate::ProcedureConfigurationDelegate(QWidget* widget, QObject* parent)
		: QObject(parent), m_widget(widget)
	{
	}

	ProcedureConfigurationDelegate::~ProcedureConfigurationDelegate()
	{
	}

	QString ProcedureConfigurationDelegate::name() const
	{
		return QString();
	}

	void ProcedureConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}

	void ProcedureConfigurationDelegate::apply()
	{
	}

	void ProcedureConfigurationDelegate::accept()
	{
	}

	void ProcedureConfigurationDelegate::reject()
	{
	}


}
