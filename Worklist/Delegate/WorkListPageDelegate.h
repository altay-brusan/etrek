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
#include "DateTimeSpan.h"
#include "IDelegate.h"
#include "IPageAction.h"

using namespace Etrek::Worklist::Repository;

namespace Etrek::Worklist::Delegate {

    class WorkListPageDelegate :
	public QObject,
    public IDelegate,
    public IPageAction 
    {
        Q_OBJECT
        Q_INTERFACES(IDelegate IPageAction)

    public:
        explicit WorkListPageDelegate(WorkListPage* ui, std::shared_ptr<WorklistRepository> repository, QObject* parent = nullptr);
        
        QString name() const override;
        void attachDelegates(const QVector<QObject*>& delegates) override;

		~WorkListPageDelegate();

    private slots:
        void onFilterDateRangeChanged(const DateTimeSpan& date);
        void onSourceChanged(const QString& source);
        void onClearFilters();
        void onSearchChanged();
       
        void onEntryCreated(const WorklistEntry& entry);
        void onEntryUpdated(const WorklistEntry& entry);
        void onEntryDeleted(int entryId);

        void onSearchName(const QString& name);
        void onSearchPatientId(const QString& patientId);
        void onSearchAcquisionNo(const QString& acquisionNo);
        void onSearchPatientDate(const QDate& acquisionNo);
        void onSearchStudyId(const QString& studyId);


    private:
        void applyFilters();
        void applySearch();
        void loadWorklistData(const QList<WorklistEntry>& entries);
        void onClearSearch();
 

        WorkListPage* ui;
        QList<DicomTag> getDisplayTagList() const;
        QList<QStandardItem*> createRowForEntry(const WorklistEntry& entry) const;
        QPointer<QStandardItemModel> baseModel;
        QPointer<QSortFilterProxyModel> proxyModel;
        std::shared_ptr<WorklistRepository> repository;

        void apply() override;
        void accept() override;
        void reject() override;
    };
}


#endif // WORKLISTPAGECONTROLLER_H
