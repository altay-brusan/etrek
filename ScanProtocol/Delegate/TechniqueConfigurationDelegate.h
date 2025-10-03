#ifndef TECHNIQUECONFIGURATIONDELEGATE_H
#define TECHNIQUECONFIGURATIONDELEGATE_H

#include <QHash>
#include <memory>

#include "IDelegate.h"
#include "IPageAction.h"

#include "TechniqueParameter.h"
#include "TechniqueConfigurationWidget.h"
#include "ScanProtocolRepository.h"

using namespace Etrek::ScanProtocol::Data::Entity;
using namespace Etrek::ScanProtocol::Repository;
namespace Etrek::ScanProtocol::Delegate {

    class TechniqueConfigurationDelegate :
        public QObject,
        public IDelegate,
        public IPageAction
    {
        Q_OBJECT
            Q_INTERFACES(IDelegate IPageAction)

    public:
        TechniqueConfigurationDelegate(TechniqueConfigurationWidget* widget,
            std::shared_ptr<ScanProtocolRepository> repo,
            QObject* parent = nullptr);
        ~TechniqueConfigurationDelegate() override;

        QString name() const override;
        void attachDelegates(const QVector<QObject*>&) override;

    private:
        void apply() override;
        void accept() override;
        void reject() override;

        TechniqueConfigurationWidget* m_widget = nullptr; // global-scope class
        std::shared_ptr<ScanProtocolRepository> m_repo;

        // Cache for diffing deletes:
        QHash<QString, Etrek::ScanProtocol::Data::Entity::TechniqueParameter> m_lastPersistedByKey;

        static QString makeKey(int bodyPartId,
            Etrek::ScanProtocol::TechniqueProfile p,
            Etrek::ScanProtocol::BodySize s);
        void refreshPersistedCacheFor(int bodyPartId);
        bool saveCurrentBodyPart();
    };

} // namespace Etrek::ScanProtocol::Delegate


#endif // !TECHNIQUECONFIGURATIONDELEGATE_H

