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
#include "RisConnectionSetting.h"
#include <QMutex>

namespace Etrek::Worklist::Connectivity 
{
    namespace lg = Etrek::Core::Log;
    namespace mdl = Etrek::Core::Data::Model;
    namespace ent = Etrek::Worklist::Data::Entity;
	namespace glob = Etrek::Core::Globalization;

    class WorklistQueryServiceTest;



    class WorklistQueryService : public QObject {


        Q_OBJECT

            friend class WorklistQueryServiceTest;

    public:
        explicit WorklistQueryService(QObject* parent = nullptr);
        WorklistQueryService(const WorklistQueryService&) = delete;
        WorklistQueryService(WorklistQueryService&& other) noexcept;

        ~WorklistQueryService();

        void setWorklistTags(const QList<ent::DicomTag>& tags);
        void setIdentifierTags(const QList<ent::DicomTag>& identifiers);
        void setPresentationContext(const ent::WorklistPresentationContext& context);
        void setSettings(std::shared_ptr<mdl::RisConnectionSetting> settings);

        // Explicitly prepare the DICOM association (add contexts + negotiate)
        Etrek::Specification::Result<QString> prepareAssociation();

        // Release association explicitly
        Etrek::Specification::Result<QString> releaseAssociation();

        QList<ent::WorklistEntry> getWorklistEntries();
        Etrek::Specification::Result<QString> echoRis();

    signals:
        void worklistEntriesReceived(const QList<ent::WorklistEntry>& worklistEntries);

    private:
        std::unique_ptr<DcmDataset> createWorklistQuery(const QList<ent::DicomTag>& queryTags) noexcept;


        Etrek::Specification::Result<QString> initNetwork();
        Etrek::Specification::Result<QString> negotiateTheAssociation();

        // Adds a single presentation context (for echo or find)
        Etrek::Specification::Result<int> addPresentationContextForOperation(const QString& operation);

        void setupTheConnectionParameters();
        bool isConnected();

        ent::WorklistEntry parseDatasetToWorklist(DcmDataset* dataset, const QVector<ent::DicomTag>& dicomTags);

        void reconnect();

        // Members
        QList<ent::DicomTag> m_identifierTags;
        QList<ent::DicomTag> m_worklistTags;
        std::shared_ptr<mdl::RisConnectionSetting> m_settings = nullptr;
        ent::WorklistPresentationContext m_presentationContext;
        glob::TranslationProvider* translator;
        QMutex m_scuMutex;
        std::unique_ptr<DcmSCU> m_dcmScu;
        std::shared_ptr<lg::AppLogger> logger;
    };

}
#endif // WORKLISTQUERYSERVICE_H
