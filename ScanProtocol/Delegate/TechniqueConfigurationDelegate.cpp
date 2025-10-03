#include "TechniqueConfigurationDelegate.h"
#include "TechniqueConfigurationWidget.h"

using namespace Etrek::ScanProtocol;
using namespace Etrek::ScanProtocol::Data::Entity;
using namespace Etrek::ScanProtocol::Repository;

namespace Etrek::ScanProtocol::Delegate {

    TechniqueConfigurationDelegate::TechniqueConfigurationDelegate(
        ::TechniqueConfigurationWidget* widget,
        std::shared_ptr<ScanProtocolRepository> repo,
        QObject* parent)
        : QObject(parent), m_widget(widget), m_repo(std::move(repo))
    {
    }

    TechniqueConfigurationDelegate::~TechniqueConfigurationDelegate() = default;

    QString TechniqueConfigurationDelegate::name() const { return "Technique Configuration"; }
    void TechniqueConfigurationDelegate::attachDelegates(const QVector<QObject*>&) {}

    QString TechniqueConfigurationDelegate::makeKey(int bodyPartId, TechniqueProfile p, BodySize s)
    {
        return QString("%1|%2|%3").arg(bodyPartId).arg(int(p)).arg(int(s));
    }

    void TechniqueConfigurationDelegate::refreshPersistedCacheFor(int bodyPartId)
    {
        m_lastPersistedByKey.clear();

        const auto all = m_repo->getAllTechniqueParameters();
        if (!all.isSuccess) return;

        for (const auto& tp : all.value) {
            if (tp.BodyPart.Id != bodyPartId) continue;
            m_lastPersistedByKey.insert(makeKey(tp.BodyPart.Id, tp.Profile, tp.Size), tp);
        }
    }

    bool TechniqueConfigurationDelegate::saveCurrentBodyPart()
    {
        if (!m_widget || !m_repo) return false;

        const int bodyPartId = m_widget->currentBodyPartId();
        if (bodyPartId <= 0) return false;

        // 1) snapshot from view
        const auto current = m_widget->collectCurrentBodyPartParameters();

        // 2) map of current
        QHash<QString, TechniqueParameter> nowByKey;
        nowByKey.reserve(current.size());
        for (const auto& tp : current)
            nowByKey.insert(makeKey(tp.BodyPart.Id, tp.Profile, tp.Size), tp);

        // 3) load last persisted to compute deletes
        refreshPersistedCacheFor(bodyPartId);

        // 4) upsert
        bool ok = true;
        for (const auto& tp : current) {
            auto r = m_repo->upsertTechniqueParameter(tp);
            if (!r.isSuccess) ok = false;
        }

        // 5) delete removed
        for (auto it = m_lastPersistedByKey.cbegin(); it != m_lastPersistedByKey.cend(); ++it) {
            if (!nowByKey.contains(it.key())) {
                const auto& t = it.value();
                auto d = m_repo->deleteTechniqueParameter(t.BodyPart.Id, t.Size, t.Profile);
                if (!d.isSuccess) ok = false;
            }
        }

        // 6) refresh cache (avoid move-assign quirks)
        m_lastPersistedByKey = nowByKey; // or m_lastPersistedByKey.swap(nowByKey);
        return ok;
    }

    void TechniqueConfigurationDelegate::apply() { saveCurrentBodyPart(); }
    void TechniqueConfigurationDelegate::accept() { saveCurrentBodyPart(); }
    void TechniqueConfigurationDelegate::reject() {}

} // namespace
