#ifndef RISPROCEDUREMAPPINGCONFIGURATIONDELEGATE_H
#define RISPROCEDUREMAPPINGCONFIGURATIONDELEGATE_H

#include <QObject>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Worklist::Repository { class RisProcedureMappingRepository; }
class RisProcedureMappingConfigurationWidget;

namespace Etrek::Worklist::Delegate
{
    namespace rpo = Etrek::Worklist::Repository;

    class RisProcedureMappingConfigurationDelegate :
        public QObject,
        public IDelegate,
        public IPageAction
    {
        Q_OBJECT
        Q_INTERFACES(IDelegate IPageAction)

    public:
        RisProcedureMappingConfigurationDelegate(
            RisProcedureMappingConfigurationWidget* widget,
            std::shared_ptr<rpo::RisProcedureMappingRepository> repository,
            QObject* parent = nullptr);

        ~RisProcedureMappingConfigurationDelegate() override;

        QString name() const override;
        void attachDelegates(const QVector<QObject*>& delegates) override;

    private:
        RisProcedureMappingConfigurationWidget* m_widget = nullptr;
        std::shared_ptr<rpo::RisProcedureMappingRepository> m_repository;

        void apply() override;
        void accept() override;
        void reject() override;

        bool saveAllMappings();
    };
}

#endif // RISPROCEDUREMAPPINGCONFIGURATIONDELEGATE_H
