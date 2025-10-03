#include "SystemSettingPageDelegate.h"
#include "SystemSettingPageDelegate.h"
#include "SystemSettingPage.h"
#include <QObject>

SystemSettingPageDelegate::SystemSettingPageDelegate(SystemSettingPage* page, QObject* parent)
	: QObject(parent), 	 
	  m_page(page)
{
    connect(page, &SystemSettingPage::saveSettings, this, &SystemSettingPageDelegate::accept);
    connect(page, &SystemSettingPage::applySettings, this, &SystemSettingPageDelegate::apply);
    connect(page, &SystemSettingPage::closeSettings, this, &SystemSettingPageDelegate::reject);
   

}



SystemSettingPageDelegate::~SystemSettingPageDelegate()
{}

QString SystemSettingPageDelegate::name() const
{
	return QString();
}

void SystemSettingPageDelegate::attachDelegates(const QVector<QObject*>& list)
{
    for (auto* o : list)
        if (!delegates.contains(o)) delegates.append(QPointer<QObject>(o));
}

SystemSettingPage* SystemSettingPageDelegate::GetPage() const
{
    return m_page;
}


void SystemSettingPageDelegate::apply()
{
    for (const auto& p : delegates) {
        if (!p) continue;                            // auto-nulled on delete
        if (auto* act = qobject_cast<IPageAction*>(p.data())) act->apply();
    }
}

void SystemSettingPageDelegate::accept()
{
    //m_page->closePage();
    // do save operation here
	emit closeSettings(); // Notify that settings are saved
}

void SystemSettingPageDelegate::reject()
{
}

