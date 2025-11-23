// WorkListPageDelegate.h
#ifndef WORKLISTPAGEDELEGATE_H
#define WORKLISTPAGEDELEGATE_H

#include <QObject>
#include <QPointer>
#include <QStandardItemModel>
#include "WorklistFilterProxyModel.h"
#include <QDateTime>
#include "WorklistRepository.h"
#include "WorkListPage.h"
#include "ScanProtocolRepository.h"
#include "DateTimeSpan.h"
#include "IDelegate.h"
#include "IPageAction.h"
#include "DicomRepository.h"
#include "DicomTagRepository.h"
#include "PatientModel.h"

namespace Etrek::Context {
    class IContextManager;
}

namespace Etrek::Application::Delegate {

    namespace repo = Etrek::Worklist::Repository;
    namespace ent = Etrek::Worklist::Data::Entity;
    namespace mdl = Etrek::ScanProtocol::Data::Model;

    class WorkListPageDelegate :
	public QObject,
    public IDelegate,
    public IPageAction
    {
        Q_OBJECT
        Q_INTERFACES(IDelegate IPageAction)

    public:
        explicit WorkListPageDelegate(WorkListPage* ui,
            std::shared_ptr<repo::WorklistRepository> repository,
            std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanRepository,
            std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepository,
            std::shared_ptr<Etrek::Dicom::Repository::DicomTagRepository> dicomTagRepository,
            std::weak_ptr<Etrek::Context::IContextManager> contextManager = std::weak_ptr<Etrek::Context::IContextManager>(),
            QObject* parent = nullptr);

        QString name() const override;
        void attachDelegates(const QVector<QObject*>& delegates) override;

		~WorkListPageDelegate();

    signals:
        void closeWorklist();
        /**
         * @brief Emitted when user double-clicks a worklist item to start examination.
         * @param entryId The ID of the selected worklist entry.
         */
        void startExamination(int entryId);

    private slots:
        void onWorklistItemDoubleClicked(int entryId);
        void onUpdatePatient();
        void onAddNewPatient();
        void onFilterDateRangeChanged(const DateTimeSpan& date);
        void onSourceChanged(const QString& source);
        void onClearFilters();
        void onSearchChanged();

        void onEntryCreated(const ent::WorklistEntry& entry);
        void onEntryUpdated(const ent::WorklistEntry& entry);
        void onEntryDeleted(int entryId);

        void onSearchName(const QString& name);
        void onSearchPatientId(const QString& patientId);
        void onSearchAcquisionNo(const QString& acquisionNo);
        void onSearchPatientDate(const QDate& acquisionNo);
        void onSearchStudyId(const QString& studyId);



    private:
        void applyFilters();
        void applySearch();
        void loadWorklistData(const QList<ent::WorklistEntry>& entries);
        void onClearSearch();
        mdl::PatientModel worklistEntryToPatientModel(const ent::WorklistEntry& entry) const;


        WorkListPage* ui;
        QList<ent::DicomTag> getDisplayTagList() const;
        QList<QStandardItem*> createRowForEntry(const ent::WorklistEntry& entry) const;
        QPointer<QStandardItemModel> baseModel;
        QPointer<WorklistFilterProxyModel> proxyModel;
        std::shared_ptr<repo::WorklistRepository> repository;
        std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanRepository;
        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepository;
        std::shared_ptr<Etrek::Dicom::Repository::DicomTagRepository> dicomTagRepository;
        std::weak_ptr<Etrek::Context::IContextManager> contextManager;

        void apply() override;
        void accept() override;
        void reject() override;
    };
}


#endif // WORKLISTPAGECONTROLLER_H
