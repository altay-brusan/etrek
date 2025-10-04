#include "WorklistQueryService.h"
#include "AppLoggerFactory.h"
#include "DcmtkQtUtils.h"
#include "MessageKey.h"
#include "MappingProfile.h"

WorklistQueryService::WorklistQueryService(QObject* parent)
    : QObject(parent)
{
    translator = &TranslationProvider::Instance();
    AppLoggerFactory factory(LoggerProvider::Instance(), translator);

    logger = factory.CreateLogger("WorklistQueryService");


    m_dcmScu = std::make_unique<DcmSCU>();
}

WorklistQueryService::WorklistQueryService(WorklistQueryService&& other) noexcept
    : m_identifierTags(std::move(other.m_identifierTags)),
      m_worklistTags(std::move(other.m_worklistTags)),
      m_settings(std::move(other.m_settings)),
      m_presentationContext(other.m_presentationContext),
      m_dcmScu(std::move(other.m_dcmScu)),
      translator(other.translator),
      logger(other.logger)
{
}

WorklistQueryService::~WorklistQueryService()
{
    if (m_dcmScu) {
        m_dcmScu->releaseAssociation();
        m_dcmScu->freeNetwork();
    }
}

void WorklistQueryService::setWorklistTags(const QList<DicomTag>& tags)
{
    m_worklistTags = tags;
}

void WorklistQueryService::setIdentifierTags(const QList<DicomTag>& ids)
{
    m_identifierTags = ids;
}

void WorklistQueryService::setPresentationContext(const WorklistPresentationContext& context)
{
    m_presentationContext = context;
}

void WorklistQueryService::setSettings(std::shared_ptr<RisConnectionSetting> settings)
{
    m_settings = settings;
    setupTheConnectionParameters();
}

Etrek::Specification::Result<QString> WorklistQueryService::prepareAssociation()
{
     //QMutexLocker locker(&m_scuMutex);

    if (!m_dcmScu) {
        return Etrek::Specification::Result<QString>::Failure("DICOM SCU not initialized.");
    }

    // Release any existing association to start fresh
    m_dcmScu->releaseAssociation();

    // Now add contexts
    auto addEcho = addPresentationContextForOperation("C-ECHO");
    if (!addEcho.isSuccess)
    {
        QString message = "Failed to add C-ECHO context: " + addEcho.message;
        qDebug()<<message;
        return Etrek::Specification::Result<QString>::Failure(addEcho.message);
    }
    auto addFind = addPresentationContextForOperation("C-FIND");
    if (!addFind.isSuccess)
    {
        QString message = "Failed to add C-FIND context: " + addFind.message;
        qDebug()<<message;
        return Etrek::Specification::Result<QString>::Failure(addFind.message);
    }
    auto initNet = initNetwork();
    if (!initNet.isSuccess)
        return initNet;

    auto negotiate = negotiateTheAssociation();
    if (!negotiate.isSuccess)
        return negotiate;

    auto echo = echoRis();
    if (!echo.isSuccess)
        return echo;

    auto message = translator->getInfoMessage(RIS_ASSOCIATION_NEGOTIATION_SUCCEED);
    return Etrek::Specification::Result<QString>::Success(message);
}

Etrek::Specification::Result<QString> WorklistQueryService::releaseAssociation()
{
    if (!m_dcmScu)
        return Etrek::Specification::Result<QString>::Failure("DICOM SCU not initialized.");

    OFCondition cond = m_dcmScu->releaseAssociation();
    if (cond.bad()) {
        QString err = translator->getErrorMessage(RIS_RELEASE_CONNECTION_FAILED).arg(cond.text());
        logger->LogError(err);
        return Etrek::Specification::Result<QString>::Failure(err);
    }

    return Etrek::Specification::Result<QString>::Success(translator->getInfoMessage(RIS_RELEASE_CONNECTION_SUCCEED));
}

QList<WorklistEntry> WorklistQueryService::getWorklistEntries()
{
    QMutexLocker locker(&m_scuMutex);
    QList<WorklistEntry> worklistEntries;

    // Ensure association is ready and connected
    if (!isConnected()) {
        auto prepareResult = prepareAssociation();
        if (!prepareResult.isSuccess) {
            qDebug() << "PrepareAssociation failed:" << prepareResult.message;
            return worklistEntries;
        }
    }

    // Build query dataset
    std::unique_ptr<DcmDataset> query = createWorklistQuery(m_worklistTags);

    if (m_presentationContext.Id == -1) {
        qDebug() << "Presentation context not set!";
        return worklistEntries;
    }

    // Use the known presentation number from added contexts (C-FIND is usually context id 3)
    // This is a convention in DCMTK: presentation contexts get odd IDs starting from 1,
    // so C-ECHO is 1, C-FIND is 3, etc.
    const int presentationNumber = 3;

    OFList<QRResponse*> responses;
    OFCondition cond = m_dcmScu->sendFINDRequest(presentationNumber, query.get(), &responses);

    if (cond.good()) {
        for (auto rsp : responses) {
            if (rsp->m_status == STATUS_Success || rsp->m_status == STATUS_Pending) {
                if (rsp->m_dataset) {

                    // Convert full dataset to string for debugging
                    //std::ostringstream stream;
                    //rsp->m_dataset->print(stream, DCMTypes::PF_shortenLongTagValues | DCMTypes::PF_showTreeStructure);
                    //QString datasetString = QString::fromStdString(stream.str());
                    //logger->Log(datasetString,LogLevel::Info);

                    // Set a breakpoint on this or inspect in logs
                    //qDebug() << "Received DICOM dataset:\n" << datasetString;

                    WorklistEntry entry = parseDatasetToWorklist(rsp->m_dataset, m_worklistTags);
                    worklistEntries.append(entry);
                }
            }
            delete rsp;
        }
    }
    else {
        QString err = translator->getErrorMessage(RIS_C_FIND_FAILED).arg(cond.text());
        logger->LogError(err);
        qDebug() << "Error sending C-FIND request:" << cond.text();
    }

    return worklistEntries;
}

QString dumpDataset(DcmDataset* dataset)
{
    std::ostringstream out;
    dataset->print(out);
    return QString::fromStdString(out.str());
}

std::unique_ptr<DcmDataset> WorklistQueryService::createWorklistQuery(const QList<DicomTag>& queryTags) noexcept
{
    auto query = std::make_unique<DcmDataset>();
    QMap<DcmTagKey, DcmItem*> sequenceMap;

    for (const auto& tag : queryTags) {
        if (!tag.IsActive)
            continue;

        DcmTagKey tagKey(tag.GroupHex, tag.ElementHex);

        // If tag has a parent sequence
        if (tag.PgroupHex != 0x0000 || tag.PelementHex != 0x0000) {
            DcmTagKey parentKey(tag.PgroupHex, tag.PelementHex);

            // If we don't yet have an item for this sequence, create and insert it
            if (!sequenceMap.contains(parentKey)) {
                auto* sequence = new DcmSequenceOfItems(parentKey);
                auto* item = new DcmItem();
                sequence->insert(item);
                query->insert(sequence);
                sequenceMap[parentKey] = item;
            }

            // Insert tag into the corresponding sequence item
            sequenceMap[parentKey]->putAndInsertString(tagKey, "");
        } else {
            // Top-level tag
            query->putAndInsertString(tagKey, "");
        }
    }

    //QString queryText = dumpDataset(query.get());
    //logger->Log(queryText,LogLevel::Debug);

    return query;
}


Etrek::Specification::Result<QString> WorklistQueryService::echoRis()
{
    if (!isConnected()) {
        setupTheConnectionParameters();
    }

    OFCondition cond = m_dcmScu->sendECHORequest(1);
    if (cond.bad()) {
        QString err = translator->getErrorMessage(RIS_C_ECHO_FAILED).arg(cond.text());
        logger->LogError(err);
        return Etrek::Specification::Result<QString>::Failure(err);
    }

    QString msg = translator->getInfoMessage(RIS_C_ECHO_SUCCEED);
    return Etrek::Specification::Result<QString>::Success(msg);
}

Etrek::Specification::Result<QString> WorklistQueryService::initNetwork()
{
    OFCondition cond = m_dcmScu->initNetwork();
    if (cond.bad()) {
        QString err = translator->getErrorMessage(RIS_NETWORK_INIT_FAILED).arg(cond.text());
        logger->LogError(err);
        return Etrek::Specification::Result<QString>::Failure(err);
    }
    QString msg = translator->getInfoMessage(RIS_NETWORK_INIT_SUCCEED);
    return Etrek::Specification::Result<QString>::Success(msg);
}

Etrek::Specification::Result<QString> WorklistQueryService::negotiateTheAssociation()
{
    OFCondition cond = m_dcmScu->negotiateAssociation();
    if (cond.bad()) {
        QString error = translator->getDebugMessage(RIS_ASSOCIATION_NEGOTIATION_FAILED).arg(cond.text());
        logger->LogError(error);
        return Etrek::Specification::Result<QString>::Failure(error);
    }
    QString msg = translator->getInfoMessage(RIS_ASSOCIATION_NEGOTIATION_SUCCEED);
    return Etrek::Specification::Result<QString>::Success(msg);
}

Etrek::Specification::Result<int> WorklistQueryService::addPresentationContextForOperation(const QString& operation)
{
    if (m_presentationContext.Id == -1) {
        QString message = translator->getErrorMessage(RIS_PRESENTATION_CONTEXT_NOT_SET_MSG);
        logger->LogError(message);
        return Etrek::Specification::Result<int>::Failure(message);
    }

    OFString sopClassUid;
    OFString transferSyntaxUid = QString_To_OFString(m_presentationContext.TransferSyntaxUid);

    if (operation == "C-ECHO") {
        sopClassUid = "1.2.840.10008.1.1";
    }
    else if (operation == "C-FIND") {
        sopClassUid = "1.2.840.10008.5.1.4.31";
    }
    else {
        auto message = translator->getErrorMessage(RIS_INVALID_OPERATION_SPECIFIED);
        logger->LogError(message);
        return Etrek::Specification::Result<int>::Failure(message);
    }

    OFList<OFString> transferSyntaxList;
    transferSyntaxList.push_back(transferSyntaxUid);

    OFCondition cond = m_dcmScu->addPresentationContext(sopClassUid, transferSyntaxList);
    if (cond.bad()) {
        QString message = translator->getDebugMessage(RIS_ADD_PRESENTATION_CONTEXT_FAILED).arg(cond.text());
        logger->LogDebug(message);
        return Etrek::Specification::Result<int>::Failure(message);
    }

    int presentationNumber = m_dcmScu->findPresentationContextID(sopClassUid, transferSyntaxUid);
    if (presentationNumber == 0)
        presentationNumber = 1; // fallback, but typically should be 1 or 3

    return Etrek::Specification::Result<int>::Success(presentationNumber);
}

void WorklistQueryService::setupTheConnectionParameters()
{
    QString callingAETitle = m_settings->getCallingAETitle();
    m_dcmScu->setAETitle(QString_To_OFString(callingAETitle));

    QString calledAETitle = m_settings->getCalledAETitle();
    m_dcmScu->setPeerAETitle(QString_To_OFString(calledAETitle));

    QString host = m_settings->getHostIP();
    m_dcmScu->setPeerHostName(QString_To_OFString(host));

    Uint16 port = Etrek::Worklist::Utility::IntToUint16(m_settings->getPort());
    m_dcmScu->setPeerPort(port);

    QString info = "host: " + m_settings->getHostIP() + ", port: " + QString::number(m_settings->getPort()) +
                   ", callingAE: " + m_settings->getCallingAETitle() + ", calledAE: " + m_settings->getCalledAETitle();
    logger->LogDebug(translator->getDebugMessage(RIS_CONNECTION_PARAMETERS_CHANGE_MSG).arg(info));
}

bool WorklistQueryService::isConnected()
{
    //QMutexLocker locker(&m_scuMutex);
    return m_dcmScu && m_dcmScu->isConnected();
}

WorklistEntry WorklistQueryService::parseDatasetToWorklist(DcmDataset* dataset, const QVector<DicomTag>& dicomTags)
{
    WorklistEntry entry;

    DcmItem* spsItem = nullptr;
    dataset->findAndGetSequenceItem(DCM_ScheduledProcedureStepSequence, spsItem, 0);

    for (const auto& tag : dicomTags) {
        DcmTagKey key(tag.GroupHex, tag.ElementHex);
        OFString raw;
        bool found = dataset->findAndGetOFStringArray(key, raw).good();

        if (!found && spsItem) {
            found = spsItem->findAndGetOFStringArray(key, raw).good();
        }

        if (found) {
            QString value = QString::fromStdString(raw.c_str());

            WorklistAttribute attr;
            attr.Tag = tag;
            attr.TagValue = value;
            entry.Attributes.append(attr);
        }
    }

    return entry;
}


void WorklistQueryService::reconnect()
{
    releaseAssociation();
    prepareAssociation();
}
