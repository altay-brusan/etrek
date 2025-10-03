#ifndef SYSTEMSETTINGPAGEDELEGATE_H
#define SYSTEMSETTINGPAGEDELEGATE_H

#include <QObject>
#include <QPointer>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"
#include "SystemSettingPage.h"

class SystemSettingPageDelegate : 
	public QObject,
	public IDelegate,
	public IPageAction
{
	Q_OBJECT
	Q_INTERFACES(IDelegate IPageAction)

public:
	SystemSettingPageDelegate(SystemSettingPage* page,QObject *parent);
	
	QString name() const override;
	void attachDelegates(const QVector<QObject*>& delegates) override;
	SystemSettingPage* GetPage() const;
	~SystemSettingPageDelegate();

signals:
	void closeSettings();
public slots:
	void apply() override;
	void accept() override;
	void reject() override;

private:
	SystemSettingPage* m_page;
	QVector<QPointer<QObject>> delegates;
};


#endif // SYSTEMSETTINGPAGEDELEGATE_H
