// WorkListPageDelegate.h
#ifndef WORKLISTPAGEDELEGATE_H
#define WORKLISTPAGEDELEGATE_H

#include <QObject>
#include <QPointer>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QDateTime>
#include "WorklistRepository.h"
#include "WorkListPage.h"
#include "ScanProtocolRepository.h"
#include "DateTimeSpan.h"
#include "IDelegate.h"
#include "IPageAction.h"
#include "DicomRepository.h"

namespace Etrek::Worklist::Delegate {

    namespace repo = Etrek::Worklist::Repository;
    namespace ent = Etrek::Worklist::Data::Entity;

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
            QObject* parent = nullptr);
        
        QString name() const override;
        void attachDelegates(const QVector<QObject*>& delegates) override;

		~WorkListPageDelegate();
    
    signals:
        void closeWorklist();

    private slots:
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


        WorkListPage* ui;
        QList<ent::DicomTag> getDisplayTagList() const;
        QList<QStandardItem*> createRowForEntry(const ent::WorklistEntry& entry) const;
        QPointer<QStandardItemModel> baseModel;
        QPointer<QSortFilterProxyModel> proxyModel;
        std::shared_ptr<repo::WorklistRepository> repository;
        std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanRepository;
        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepository;
        std::shared_ptr<Etrek::Dicom::Repository::DicomTagRepository> dicomTagRepository;

        void apply() override;
        void accept() override;
        void reject() override;
    };
}


#endif // WORKLISTPAGECONTROLLER_H
