#ifndef SYSTEMSETTINGPAGEDELEGATE_H
#define SYSTEMSETTINGPAGEDELEGATE_H

#include <QObject>
#include <QPointer>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"
#include "SystemSettingPage.h"

namespace Etrek::Context {
    class IContextManager;
}

namespace Etrek::Application::Delegate {
class SystemSettingPageDelegate : public QObject,
                                  public IDelegate,
                                  public IPageAction {
  Q_OBJECT
  Q_INTERFACES(IDelegate IPageAction)

public:
  SystemSettingPageDelegate(
      SystemSettingPage *page,
      std::weak_ptr<Etrek::Context::IContextManager> contextManager,
      QObject *parent);

  QString name() const override;
  void attachDelegates(const QVector<QObject *> &delegates) override;
  SystemSettingPage *getPage() const;
  ~SystemSettingPageDelegate();

signals:
  void closeSettings();

public slots:
  void apply() override;
  void accept() override;
  void reject() override;

private slots:
  void onPageLoaded();

private:
  void setupConnections();

  SystemSettingPage *m_page;
  std::weak_ptr<Etrek::Context::IContextManager> m_contextManager;
  QVector<QPointer<QObject>> delegates;
};
} // namespace Etrek::Application::Delegate

#endif // SYSTEMSETTINGPAGEDELEGATE_H
