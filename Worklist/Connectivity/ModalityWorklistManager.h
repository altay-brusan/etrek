#ifndef RISMANAGER_H
#define RISMANAGER_H

#include <QObject>
#include <QList>
#include <QThread>
#include <QTimer>
#include "WorklistRepository.h"
#include "WorklistQueryService.h"
#include "WorklistProfile.h"
#include <QElapsedTimer>

using namespace Etrek::Worklist::Repository;


namespace Etrek::Worklist::Connectivity
{

    class ModalityWorklistManager : public QObject
    {
        Q_OBJECT

        public:
            explicit ModalityWorklistManager(std::shared_ptr<WorklistRepository> repository,
                std::shared_ptr<RisConnectionSetting> settings,
                QObject* parent = nullptr);

            void setActiveProfile(const WorklistProfile& profile);
            void changeQueryRisServerPeriod(int period);
            void startWorklistQueryFromRis();
            void stopWorklistQueryFromRis();
            QList<WorklistEntry> getEntities();
            ~ModalityWorklistManager();

        signals:
            void QueryRequested();  // For cross-thread execution

	    public slots:

			void onAboutToCloseApplication();

        private slots:
            void handleNewQueryResults(const QList<WorklistEntry>& entries);
            void performWorklistQuery();  // Called when the thread is ready or timer ticks

        private:
            void prepareQueryService();  // Sets up thread & service safely

            std::shared_ptr<WorklistRepository> m_repository;
            std::shared_ptr<RisConnectionSetting> settings;

            std::unique_ptr<WorklistQueryService> m_queryService;
            WorklistProfile m_profile;

            QThread* m_queryThread = nullptr;
            QTimer* m_echoTimer = nullptr;
            QTimer* m_findTimer = nullptr;
            bool m_isFindRunning = false;
            int m_refreshPeriodMs = 300000;  // Default 5 minutes
    };

}


#endif // RISMANAGER_H
