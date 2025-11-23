#ifndef WORKLISTPAGE_H
#define WORKLISTPAGE_H

#include <QWidget>
#include <QTableView>
#include "IWorklistRepository.h"
#include <QAbstractItemModel>
#include "DateTimeSpan.h"

namespace Ui {
class WorkListPage;
}



class WorkListPage : public QWidget
{
    Q_OBJECT

public:
    explicit WorkListPage(std::shared_ptr<Etrek::Worklist::Repository::IWorklistRepository> repository,QWidget *parent = nullptr);
    void setProxyModel(QAbstractItemModel* model);
    void closePage();
    QTableView* getWorklistTableView() const;
    ~WorkListPage();

signals:
    void addNewPatient();
    void updatePatient();
    void filterDateSpanChanged(const DateTimeSpan& date);
	void filterSourceChanged(const QString& source);
    void clearAllFilters();
    void clearAllSearch();

    void closeWorklistPage();

    void searchName(const QString& name);
    void searchPatientId(const QString& patientId);
    void searchAcquisionNo(const QString& acquisionNo);
    void searchPatientDate(const QDate& acquisionNo);
    void searchStudyId(const QString& studyId);

private:
    void clearAllFilterBtnClicked();
    void clearAllSearchBtnClicked();
    void setStile();
    Ui::WorkListPage *ui;
};

#endif // WORKLISTPAGE_H
