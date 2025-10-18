#ifndef TECHNIQUECONFIGURATIONDELEGATE_H
#define TECHNIQUECONFIGURATIONDELEGATE_H

#include <QHash>
#include <memory>

#include "IDelegate.h"
#include "IPageAction.h"

#include "TechniqueParameter.h"
#include "TechniqueConfigurationWidget.h"
#include "ScanProtocolRepository.h"

namespace Etrek::ScanProtocol::Delegate {

    namespace sp = Etrek::ScanProtocol;
    namespace ent = Etrek::ScanProtocol::Data::Entity;
    namespace repo = Etrek::ScanProtocol::Repository;

    class TechniqueConfigurationDelegate :
        public QObject,
        public IDelegate,
        public IPageAction
    {
        Q_OBJECT
            Q_INTERFACES(IDelegate IPageAction)

    public:
        TechniqueConfigurationDelegate(TechniqueConfigurationWidget* widget,
            std::shared_ptr<repo::ScanProtocolRepository> repo,
            QObject* parent = nullptr);
        ~TechniqueConfigurationDelegate() override;

        QString name() const override;
        void attachDelegates(const QVector<QObject*>&) override;

    private:
        void apply() override;
        void accept() override;
        void reject() override;

        TechniqueConfigurationWidget* m_widget = nullptr; // global-scope class
        std::shared_ptr<repo::ScanProtocolRepository> m_repo;

        // Cache for diffing deletes:
        QHash<QString, ent::TechniqueParameter> m_lastPersistedByKey;

        static QString makeKey(int bodyPartId,
            sp::TechniqueProfile p,
            sp::BodySize s);
        void refreshPersistedCacheFor(int bodyPartId);
        bool saveCurrentBodyPart();
    };

} // namespace Etrek::ScanProtocol::Delegate


#endif // !TECHNIQUECONFIGURATIONDELEGATE_H

