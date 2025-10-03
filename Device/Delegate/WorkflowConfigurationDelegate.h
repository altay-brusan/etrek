#ifndef WORKFLOWCONFIGURATIONDELEGATE_H
#define WORKFLOWCONFIGURATIONDELEGATE_H
#include <QWidget>
#include <QObject>
#include "IDelegate.h"
#include "IPageAction.h"

class WorkflowConfigurationDelegate : 
	public QObject,
	public IDelegate,
	public IPageAction
{
    Q_OBJECT
    Q_INTERFACES(IDelegate IPageAction)

public:
    explicit WorkflowConfigurationDelegate(QWidget* widget, QObject *parent = nullptr);

    QString name() const override;

    void attachDelegates(const QVector<QObject*>& delegates) override;



private:
	QWidget* m_widget = nullptr; // Pointer to the associated widget
    

    void apply() override;

    void accept() override;

    void reject() override;

};

#endif // WORKFLOWCONFIGURATIONDELEGATE_H
