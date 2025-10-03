#include "WorkflowConfigurationDelegate.h"

WorkflowConfigurationDelegate::WorkflowConfigurationDelegate(QWidget* widget, QObject *parent)
	: QObject{ parent }, m_widget(widget)
{}

QString WorkflowConfigurationDelegate::name() const
{
	return QString();
}

void WorkflowConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
{
}

void WorkflowConfigurationDelegate::apply()
{
}

void WorkflowConfigurationDelegate::accept()
{
}

void WorkflowConfigurationDelegate::reject()
{
}
