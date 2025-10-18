#ifndef RISMANAGER_H
#define RISMANAGER_H

#include <QObject>
#include <QList>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>

#include "WorklistProfile.h"


namespace Etrek::Worklist::Repository
{
    class WorklistRepository;
}

namespace Etrek::Worklist::Data::Entity
{
    class WorklistEntry;
}

namespace Etrek::Core::Data::Model
{
    class RisConnectionSetting;
}

namespace Etrek::Worklist::Connectivity
{

    class WorklistQueryService;

    class ModalityWorklistManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ModalityWorklistManager(std::shared_ptr<Etrek::Worklist::Repository::WorklistRepository> repository,
            std::shared_ptr<Etrek::Core::Data::Model::RisConnectionSetting> settings,
            QObject* parent = nullptr);

        void setActiveProfile(const Etrek::Worklist::Data::Entity::WorklistProfile& profile);
        void changeQueryRisServerPeriod(int period);
        void startWorklistQueryFromRis();
        void stopWorklistQueryFromRis();
        QList<Etrek::Worklist::Data::Entity::WorklistEntry> getEntities();
        ~ModalityWorklistManager();

    signals:
        void QueryRequested();  // For cross-thread execution

    public slots:

        void onAboutToCloseApplication();

    private slots:
        void handleNewQueryResults(const QList<Etrek::Worklist::Data::Entity::WorklistEntry>& entries);
        void performWorklistQuery();  // Called when the thread is ready or timer ticks

    private:
        void prepareQueryService();  // Sets up thread & service safely

        std::shared_ptr<Etrek::Worklist::Repository::WorklistRepository> m_repository;
        std::shared_ptr<Etrek::Core::Data::Model::RisConnectionSetting> settings;

        std::unique_ptr<WorklistQueryService> m_queryService;
        Etrek::Worklist::Data::Entity::WorklistProfile m_profile;

        QThread* m_queryThread = nullptr;
        QTimer* m_echoTimer = nullptr;
        QTimer* m_findTimer = nullptr;
        bool m_isFindRunning = false;
        int m_refreshPeriodMs = 300000;  // Default 5 minutes
    };

}


#endif // RISMANAGER_H
