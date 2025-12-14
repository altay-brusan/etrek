#ifndef SYSTEMSETTINGPAGEBUILDER_H
#define SYSTEMSETTINGPAGEBUILDER_H

#include "DelegateParameter.h"
#include "IWidgetDelegateBuilder.h"
#include "SystemSettingPage.h"
#include "SystemSettingPageDelegate.h"
#include <memory>

namespace Etrek::Application::Delegate {
class SystemSettingPageBuilder
    : public IWidgetDelegateBuilder<SystemSettingPage,
                                    SystemSettingPageDelegate> {
public:
  SystemSettingPageBuilder();
  ~SystemSettingPageBuilder();

  std::pair<SystemSettingPage *, SystemSettingPageDelegate *>
  build(const DelegateParameter &params, QWidget *parentWidget = nullptr,
        QObject *parentDelegate = nullptr) override;
};
} // namespace Etrek::Application::Delegate

#endif // SYSTEMSETTINGPAGEBUILDER_H
