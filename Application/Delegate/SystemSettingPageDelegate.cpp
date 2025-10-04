#include "SystemSettingPageDelegate.h"
#include "SystemSettingPage.h"

namespace Etrek::Application::Delegate
{
    SystemSettingPageDelegate::SystemSettingPageDelegate(SystemSettingPage* page, QObject* parent)
        : QObject(parent),
        m_page(page)
    {
        connect(page, &SystemSettingPage::saveSettings, this, &SystemSettingPageDelegate::accept);
        connect(page, &SystemSettingPage::applySettings, this, &SystemSettingPageDelegate::apply);
        connect(page, &SystemSettingPage::closeSettings, this, &SystemSettingPageDelegate::reject);
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

    SystemSettingPage* SystemSettingPageDelegate::GetPage() const
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

