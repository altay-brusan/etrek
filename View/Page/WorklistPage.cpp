#include <QStandardItemModel>
#include <qpointer.h>

#include "WorklistPage.h"
#include "IWorklistRepository.h"
#include "ui_WorklistPage.h"

using namespace Etrek::Worklist::Repository;

WorkListPage::WorkListPage(std::shared_ptr<IWorklistRepository> repository, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkListPage)
{
    ui->setupUi(this);

    auto emitDateSpan = [this]() {
        DateTimeSpan span;
        span.from = ui->filterStartDateEdit->dateTime();
        span.to = ui->filterDueDateEdit->dateTime();
        emit filterDateSpanChanged(span);
        };
    
        connect(ui->filterStartDateEdit, &QDateEdit::dateChanged, this, [emitDateSpan](const QDate&) {
        emitDateSpan();
        });
    
        connect(ui->filterDueDateEdit, &QDateEdit::dateChanged, this, [emitDateSpan](const QDate&) {
        emitDateSpan();
        });
      
        connect(ui->filterSourceLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {  
           emit filterSourceChanged(text);  
        });

        connect(ui->clearAllFilterFieldsBtn, &QPushButton::clicked, this, [this]() {
            clearAllFilterBtnClicked();
            emit clearAllFilters();
            });

        connect(ui->clearAllSearchFieldsBtn, &QPushButton::clicked, this, [this]() {
            clearAllFilterBtnClicked();
            emit clearAllFilters();
            });

        connect(ui->clearAllSearchFieldsBtn, &QPushButton::clicked, this, [this]() {
            clearAllFilterBtnClicked();
            emit clearAllSearch();
            });
    
        connect(ui->searchNameLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
            emit searchName(text);
            });

        connect(ui->searchPatientIdLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
            emit searchPatientId(text);
            });

        connect(ui->searchAcquisionNoLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
            emit searchAcquisionNo(text);
            });
        
        connect(ui->searchPatientBirthDateDateEdit, &QDateEdit::dateChanged, this, [this](const QDate& date) {
            emit searchPatientDate(date);
            });
        
        connect(ui->searchStudyIdTextEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
            emit searchStudyId(text);
            });
               
}

void WorkListPage::setProxyModel(QPointer<QStandardItemModel> proxyModel)
{
   ui->tableViewWorklist->setModel(proxyModel);
   ui->tableViewWorklist->setEditTriggers(QAbstractItemView::NoEditTriggers);
   ui->tableViewWorklist->horizontalHeader()->setStretchLastSection(true);
   ui->tableViewWorklist->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
   ui->tableViewWorklist->horizontalHeader()->setStretchLastSection(true);
}

WorkListPage::~WorkListPage()
{
    delete ui;
}

void WorkListPage::clearAllFilterBtnClicked()
{
     ui->filterStartDateEdit->clear();
     ui->filterDueDateEdit->clear();
     ui->filterSourceLineEdit->clear();

}

void WorkListPage::clearAllSearchBtnClicked()
{
	ui->searchNameLineEdit->clear();
	ui->searchPatientIdLineEdit->clear();
	ui->searchAcquisionNoLineEdit->clear();
	ui->searchPatientBirthDateDateEdit->clear();
	ui->searchStudyIdTextEdit->clear();


}
