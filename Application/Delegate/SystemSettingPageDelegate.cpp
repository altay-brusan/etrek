#include "SystemSettingPageDelegate.h"
#include "SystemSettingPage.h"

#include <QTimer>

namespace Etrek::Application::Delegate
{
    SystemSettingPageDelegate::SystemSettingPageDelegate(SystemSettingPage* page, QObject* parent)
        : QObject(parent),
        m_page(page)
    {
        // Deferred initialization to allow UI to settle first
        QTimer::singleShot(0, this, &SystemSettingPageDelegate::onPageLoaded);
    }

    void SystemSettingPageDelegate::onPageLoaded()
    {
        setupConnections();
    }

    void SystemSettingPageDelegate::setupConnections()
    {
        connect(m_page, &SystemSettingPage::saveSettings, this, &SystemSettingPageDelegate::accept);
        connect(m_page, &SystemSettingPage::applySettings, this, &SystemSettingPageDelegate::apply);
        connect(m_page, &SystemSettingPage::closeSettings, this, &SystemSettingPageDelegate::reject);
    }

    SystemSettingPageDelegate::~SystemSettingPageDelegate() = default;

    QString SystemSettingPageDelegate::name() const
    {
        return QString();
    }

    void SystemSettingPageDelegate::attachDelegates(const QVector<QObject*>& list)
    {
        for (auto* o : list)
        {
            if (!delegates.contains(o))
            {
                delegates.append(QPointer<QObject>(o));
            }
        }
    }

    SystemSettingPage* SystemSettingPageDelegate::getPage() const
    {
        return m_page;
    }

    void SystemSettingPageDelegate::apply()
    {
        for (const auto& delegatePtr : delegates)
        {
            if (!delegatePtr)
            {
                continue;
            }

            if (auto* action = qobject_cast<IPageAction*>(delegatePtr.data()))
            {
                action->apply();
            }
        }
    }

    void SystemSettingPageDelegate::accept()
    {
        emit closeSettings();
    }

    void SystemSettingPageDelegate::reject()
    {
    }
}

