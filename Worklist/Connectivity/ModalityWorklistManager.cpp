#include <QDebug>
#include <QMetaObject>
#include <QTimer>

#include "ModalityWorklistManager.h"
#include "RisConnectionSetting.h"
#include "WorklistRepository.h"
#include "WorklistEntry.h"
#include "WorklistProfile.h"
#include "WorklistQueryService.h"
#include "RisProcedureMappingService.h"



namespace Etrek::Worklist::Connectivity
{
    using namespace Etrek::Core::Data::Model;
    using namespace Etrek::Worklist::Repository;
    using namespace Etrek::Worklist::Data::Entity;


    ModalityWorklistManager::ModalityWorklistManager(std::shared_ptr<WorklistRepository> repository,
        std::shared_ptr<RisConnectionSetting> settings,
        std::shared_ptr<Etrek::Worklist::Service::RisProcedureMappingService> mappingService,
        QObject* parent)
        : QObject(parent),
        m_repository(repository),
        settings(settings),
        m_mappingService(std::move(mappingService))
    {
        m_findTimer = new QTimer(this);
        m_findTimer->setInterval(m_refreshPeriodMs);

        m_echoTimer = new QTimer(this);
        m_echoTimer->setInterval(30000);  // 30 seconds send ECHO message

        m_findTimer = new QTimer(this);
        m_findTimer->setInterval(m_refreshPeriodMs);
    }

    void ModalityWorklistManager::onAboutToCloseApplication()
    {
        qDebug() << "[INFO] ModalityWorklistManager: Application is closing, stopping worklist query.";
        stopWorklistQueryFromRis();
        this->deleteLater();  // Safely delete this object
    }

    ModalityWorklistManager::~ModalityWorklistManager()
    {
        qDebug() << "[INFO] ModalityWorklistManager destructor executed.";

        stopWorklistQueryFromRis();

        if (m_queryThread) {
            m_queryThread->deleteLater();
            m_queryThread = nullptr;
        }
    }

    void ModalityWorklistManager::setActiveProfile(const WorklistProfile& profile)
    {
        m_profile = profile;

        prepareQueryService();  // Only sets up thread/service safely
    }

    void ModalityWorklistManager::changeQueryRisServerPeriod(int period)
    {
        m_refreshPeriodMs = period;
        if (m_findTimer) {
            m_findTimer->setInterval(m_refreshPeriodMs);
        }
    }

    void ModalityWorklistManager::prepareQueryService()
    {
        stopWorklistQueryFromRis();  // Clean previous threads & service

        if (m_profile.Id < 0) {
            auto result = m_repository->getProfiles();
            if (!result.isSuccess) {
                qDebug() << "[ERROR] No default profile found.";
                throw std::runtime_error("No profile is set and default profile is not accessible.");
            }
        }

        auto tagResult = m_repository->getTagsByProfile(m_profile.Id);
        if (!tagResult.isSuccess) {
            qDebug() << "[ERROR] Failed to load tags for profile.";
            return;
        }

        m_queryService = std::make_unique<WorklistQueryService>();
        m_queryService->setSettings(settings);
        m_queryService->setPresentationContext(m_profile.Context);
        m_queryService->setWorklistTags(tagResult.value);

        m_queryThread = new QThread(this);
        m_queryService->moveToThread(m_queryThread);

        // Connect the trigger only after thread is fully ready
        connect(m_queryThread, &QThread::started, this, &ModalityWorklistManager::performWorklistQuery);

        connect(this, &ModalityWorklistManager::QueryRequested, m_queryService.get(), [this]() {
            const auto entries = m_queryService->getWorklistEntries();
            QMetaObject::invokeMethod(this, "handleNewQueryResults", Qt::QueuedConnection,
                Q_ARG(QList<WorklistEntry>, entries));
            });


        m_queryThread->start();

    }

    void ModalityWorklistManager::startWorklistQueryFromRis()
    {
        if (!m_findTimer->isActive()) {
            connect(m_findTimer, &QTimer::timeout, this, &ModalityWorklistManager::performWorklistQuery);
            m_findTimer->start();
        }
        if (!m_echoTimer->isActive()) {
            connect(m_echoTimer, &QTimer::timeout, this, [this]() {
                if (!m_isFindRunning && m_queryService) {
                    m_isFindRunning = true;  // 🔹 mark channel as busy
                    qDebug() << "[INFO] Sending periodic C-ECHO...";
                    m_queryService->echoRis();
                    m_isFindRunning = false; // 🔹 release channel after echo is done
                }
                });
            m_echoTimer->start();
        }
        qDebug() << "[INFO] RIS Query Timers started.";
        // DO NOT trigger PerformWorklistQuery directly; wait for thread start signal
    }

    QList<WorklistEntry> ModalityWorklistManager::getEntities()
    {
        if (m_queryService)
            return m_queryService->getWorklistEntries();
        return {};
    }

    void ModalityWorklistManager::stopWorklistQueryFromRis()
    {
        // Stop the periodic echo timer
        if (m_echoTimer && m_echoTimer->isActive()) {
            m_echoTimer->stop();
        }

        // Stop the periodic find timer
        if (m_findTimer && m_findTimer->isActive()) {
            m_findTimer->stop();
        }

        // Stop the query service if it's running
        if (m_queryThread && m_queryThread->isRunning()) {
            // Gracefully stop the query thread
            m_queryThread->quit();
            m_queryThread->wait();
        }

        // Reset the query service
        m_queryService.reset();

        // Clean up the query thread if it exists
        if (m_queryThread) {
            delete m_queryThread; // Manually delete the thread object
            m_queryThread = nullptr;
        }
    }

    void ModalityWorklistManager::performWorklistQuery()
    {
        if (m_isFindRunning) {
            qDebug() << "[WARN] C-FIND skipped: Previous request still running.";
            return;
        }

        if (m_queryService && m_queryThread && m_queryThread->isRunning()) {
            qDebug() << "[INFO] Performing RIS query...";
            m_isFindRunning = true;
            emit QueryRequested();
        }
        else {
            qDebug() << "[WARN] QueryService or thread not ready.";
        }
    }

    void ModalityWorklistManager::handleNewQueryResults(const QList<WorklistEntry>& entries)
    {
        m_isFindRunning = false;
        for (const auto& entry : entries) {
            auto existing = m_repository->getWorklistEntry(entry, m_profile);
            if (!existing.isSuccess) {
                WorklistEntry remapedEntry = entry;
                remapedEntry.Profile = m_profile;
                remapedEntry.Source = Source::RIS;
                remapedEntry.Status = ProcedureStepStatus::PENDING;
                remapedEntry.CreatedAt = QDateTime::currentDateTime();

                // Attempt to map the procedure code to an internal view
                if (m_mappingService) {
                    QString procedureCode = extractProcedureCode(entry);
                    QString codeMeaning = extractProcedureCodeMeaning(entry);
                    QString codingScheme = extractCodingScheme(entry);
                    QString connectionName = settings->getConnectionName();

                    if (!procedureCode.isEmpty()) {
                        auto mappingResult = m_mappingService->mapProcedureCode(
                            connectionName, procedureCode, codingScheme, codeMeaning);

                        if (mappingResult.success) {
                            qDebug() << "[ModalityWorklistManager] Procedure code" << procedureCode
                                     << "mapped to view:" << mappingResult.viewName
                                     << "(ID:" << mappingResult.viewId << ")";
                            // TODO: Store mappingResult.viewId in the worklist entry
                            // This requires extending WorklistEntry entity in Epic 5
                        } else {
                            qDebug() << "[ModalityWorklistManager]" << mappingResult.warningMessage;
                        }
                    }
                }

                m_repository->createWorklistEntry(remapedEntry);
            }
            else {
                // TODO: this section is partially tested.
                // It is commented out, the logic may need to be rivisited.
                // WorklistEntry remapedEntry = entry;
                // remapedEntry.Id = existing.value.Id;
                // remapedEntry.Profile = existing.value.Profile;
                // remapedEntry.Attributes = entry.Attributes; // always override the tag values with new items
                // remapedEntry.Source = Source::RIS;  // TODO: check if always overrite the source to ris
                // remapedEntry.Status = ProcedureStepStatus::PENDING; // TODO: check if status always should be pending
                // remapedEntry.UpdatedAt = QDateTime::currentDateTime();
                // m_repository->UpdateWorklistEntry(remapedEntry);
            }
        }
    }

    QString ModalityWorklistManager::extractProcedureCode(const WorklistEntry& entry) const
    {
        // Look for Scheduled Protocol Code Sequence (0040,0008) > Code Value (0008,0100)
        // The tag may be stored with parent reference or as a flat attribute
        for (const auto& attr : entry.Attributes) {
            // Code Value tag: (0008,0100) - may be nested under (0040,0008)
            if (attr.Tag.GroupHex == 0x0008 && attr.Tag.ElementHex == 0x0100) {
                if (!attr.TagValue.isEmpty()) {
                    return attr.TagValue;
                }
            }
        }
        return QString();
    }

    QString ModalityWorklistManager::extractProcedureCodeMeaning(const WorklistEntry& entry) const
    {
        // Look for Code Meaning (0008,0104)
        for (const auto& attr : entry.Attributes) {
            if (attr.Tag.GroupHex == 0x0008 && attr.Tag.ElementHex == 0x0104) {
                if (!attr.TagValue.isEmpty()) {
                    return attr.TagValue;
                }
            }
        }
        return QString();
    }

    QString ModalityWorklistManager::extractCodingScheme(const WorklistEntry& entry) const
    {
        // Look for Coding Scheme Designator (0008,0102)
        for (const auto& attr : entry.Attributes) {
            if (attr.Tag.GroupHex == 0x0008 && attr.Tag.ElementHex == 0x0102) {
                if (!attr.TagValue.isEmpty()) {
                    return attr.TagValue;
                }
            }
        }
        return QString();
    }

}
