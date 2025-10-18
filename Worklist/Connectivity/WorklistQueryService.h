#ifndef WORKLISTQUERYSERVICE_H
#define WORKLISTQUERYSERVICE_H

#include <QVector>
#include <QList>
#include <QTimer>
#include <memory>
#include "RisConnectionSetting.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmnet/scu.h"
#include "Result.h"
#include "AppLogger.h"
#include "TranslationProvider.h"
#include "DicomTag.h"
#include "WorklistEntry.h"
#include "WorklistPresentationContext.h"
#include <QMutex>

namespace Etrek::Worklist::Connectivity 
{
    class WorklistQueryServiceTest;

    class WorklistQueryService : public QObject {


        Q_OBJECT

            friend class WorklistQueryServiceTest;

    public:
        explicit WorklistQueryService(QObject* parent = nullptr);
        WorklistQueryService(const WorklistQueryService&) = delete;
        WorklistQueryService(WorklistQueryService&& other) noexcept;

        ~WorklistQueryService();

        void setWorklistTags(const QList<Etrek::Worklist::Data::Entity::DicomTag>& tags);
        void setIdentifierTags(const QList<Etrek::Worklist::Data::Entity::DicomTag>& identifiers);
        void setPresentationContext(const Etrek::Worklist::Data::Entity::WorklistPresentationContext& context);
        void setSettings(std::shared_ptr<Etrek::Core::Data::Model::RisConnectionSetting> settings);

        // Explicitly prepare the DICOM association (add contexts + negotiate)
        Etrek::Specification::Result<QString> prepareAssociation();

        // Release association explicitly
        Etrek::Specification::Result<QString> releaseAssociation();

        QList<Etrek::Worklist::Data::Entity::WorklistEntry> getWorklistEntries();
        Etrek::Specification::Result<QString> echoRis();

    signals:
        void worklistEntriesReceived(const QList<Etrek::Worklist::Data::Entity::WorklistEntry>& worklistEntries);

    private:
        std::unique_ptr<DcmDataset> createWorklistQuery(const QList<Etrek::Worklist::Data::Entity::DicomTag>& queryTags) noexcept;


        Etrek::Specification::Result<QString> initNetwork();
        Etrek::Specification::Result<QString> negotiateTheAssociation();

        // Adds a single presentation context (for echo or find)
        Etrek::Specification::Result<int> addPresentationContextForOperation(const QString& operation);

        void setupTheConnectionParameters();
        bool isConnected();

        Etrek::Worklist::Data::Entity::WorklistEntry parseDatasetToWorklist(DcmDataset* dataset, const QVector<Etrek::Worklist::Data::Entity::DicomTag>& dicomTags);

        void reconnect();

        // Members
        QList<Etrek::Worklist::Data::Entity::DicomTag> m_identifierTags;
        QList<Etrek::Worklist::Data::Entity::DicomTag> m_worklistTags;
        std::shared_ptr<Etrek::Core::Data::Model::RisConnectionSetting> m_settings = nullptr;
        Etrek::Worklist::Data::Entity::WorklistPresentationContext m_presentationContext;
        Etrek::Core::Globalization::TranslationProvider* translator;
        QMutex m_scuMutex;
        std::unique_ptr<DcmSCU> m_dcmScu;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };

}
#endif // WORKLISTQUERYSERVICE_H
