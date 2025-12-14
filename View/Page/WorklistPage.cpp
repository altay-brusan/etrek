#include <QStandardItemModel>
#include <qpointer.h>
#include <QComboBox>

#include "WorklistPage.h"
#include "IWorklistRepository.h"
#include "ui_WorklistPage.h"

using namespace Etrek::Worklist::Repository;

WorkListPage::WorkListPage(std::shared_ptr<IWorklistRepository> repository, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkListPage)
{
    ui->setupUi(this);
    setStile();

    // Set default date range: 1 year ago to today
    QDate today = QDate::currentDate();
    QDate oneYearAgo = today.addYears(-1);

    // filterDueDateEdit is the "From" field (start date)
    // filterStartDateEdit is the "To" field (end date)
    ui->filterDueDateEdit->setDate(oneYearAgo);
    ui->filterStartDateEdit->setDate(today);

    // Initialize source combo box with options
    ui->filterSourceComboBox->addItem("All");
    ui->filterSourceComboBox->addItem("LOCAL");
    ui->filterSourceComboBox->addItem("RIS");
    ui->filterSourceComboBox->setCurrentIndex(0); // Default to "All"

    // Lambda to emit date span when either date changes
    // Note: filterDueDateEdit = "From" date, filterStartDateEdit = "To" date
    auto emitDateSpan = [this]() {
        DateTimeSpan span;
        span.from = ui->filterDueDateEdit->dateTime();   // "From" field
        span.to = ui->filterStartDateEdit->dateTime();   // "To" field
        emit filterDateSpanChanged(span);
    };

    connect(ui->filterStartDateEdit, &QDateEdit::dateChanged, this, [emitDateSpan](const QDate&) {
        emitDateSpan();
    });

    connect(ui->filterDueDateEdit, &QDateEdit::dateChanged, this, [emitDateSpan](const QDate&) {
        emitDateSpan();
    });

    // Connect source combo box
    connect(ui->filterSourceComboBox, &QComboBox::currentTextChanged, this, [this](const QString& text) {
        emit filterSourceChanged(text);
    });

        connect(ui->clearAllFilterFieldsBtn, &QPushButton::clicked, this, [this]() {
            clearAllFilterBtnClicked();
            emit clearAllFilters();
            });

        connect(ui->clearAllSearchFieldsBtn, &QPushButton::clicked, this, [this]() {
            clearAllSearchBtnClicked();
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
        
        connect(ui->searchStudyNameTextEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
            emit searchStudyName(text);
            });

        // Emit signal for delegate to handle Add New Patient dialog
        connect(ui->addNewPatientBtn, &QPushButton::clicked, this, [this]() {
            emit addNewPatient();
        });

        // Emit signal for delegate to handle Update Patient dialog
        connect(ui->updatePatientBtn, &QPushButton::clicked, this, [this]() {
            emit updatePatient();
        });

        // Disable Update button by default (enabled when row is selected)
        if (ui->updatePatientBtn) {
            ui->updatePatientBtn->setEnabled(false);
        }

}

void WorkListPage::setStile()
{
    const char* lineCss =
        "QLineEdit {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QLineEdit:focus { border-color: rgb(160,160,160); }";

    const char* dateCss =
        "QDateEdit {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QDateEdit:focus { border-color: rgb(160,160,160); }"
        "QDateEdit::drop-down { width:20px; border-left:1px solid rgb(120,120,120); margin:0; }";

    const char* tabCss =
        "QTabBar::tab { "
        "background-color: rgb(83, 83, 83); "
        "color: rgb(208, 208, 208); "
        "} "
        "QTabBar::tab:selected { "
        "background-color: rgb(60, 60, 60); "
        "color: rgb(255, 255, 255); "
        "}";

    const char* buttonCss =
        "QPushButton {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background:rgb(74,74,74);"
        "  min-height:20px;"
        "  padding:4px 12px;"
        "  color: rgb(230,230,230);"
        "}"
        "QPushButton:hover { border-color: rgb(160,160,160); background:rgb(92,92,92); }"
        "QPushButton:pressed { background:rgb(64,64,64); }"
        "QPushButton:focus { border-color: rgb(180,180,180); }"
        "QPushButton:disabled { border-color: rgb(80,80,80); background:rgb(60,60,60); color: rgb(130,130,130); }";

    const char* groupCss =
        "QGroupBox {"
        "  border:1px solid rgb(120,120,120); border-radius:6px; margin-top:18px; }"
        "QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 6px; }";

    const char* tableCss =
        "QTableView {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background: rgb(54,54,54);"
        "  gridline-color: rgb(80,80,80);"
        "  selection-background-color: rgb(92,92,92);"
        "  selection-color: rgb(255,255,255);"
        "}"
        "QTableView::item:selected {"
        "  font-weight: bold;"
        "  color: rgb(255,255,255);"
        "}"
        "QHeaderView::section { background: rgb(64,64,64); padding:4px; border:1px solid rgb(90,90,90); }";

    const char* comboCss =
        "QComboBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QComboBox:focus { border-color: rgb(160,160,160); }"
        "QComboBox::drop-down { width:20px; border-left:1px solid rgb(120,120,120); margin:0; }"
        "QComboBox QAbstractItemView { background: rgb(74,74,74); selection-background-color: rgb(92,92,92); }";

    // Apply guarded (pointer-checked) to avoid crashes if UI changed
    if (ui->filterSourceComboBox) ui->filterSourceComboBox->setStyleSheet(comboCss);
    if (ui->searchNameLineEdit) ui->searchNameLineEdit->setStyleSheet(lineCss);
    if (ui->searchPatientIdLineEdit) ui->searchPatientIdLineEdit->setStyleSheet(lineCss);
    if (ui->searchAcquisionNoLineEdit) ui->searchAcquisionNoLineEdit->setStyleSheet(lineCss);
    if (ui->searchStudyNameTextEdit) ui->searchStudyNameTextEdit->setStyleSheet(lineCss);

    if (ui->filterStartDateEdit) ui->filterStartDateEdit->setStyleSheet(dateCss);
    if (ui->filterDueDateEdit) ui->filterDueDateEdit->setStyleSheet(dateCss);
    if (ui->searchPatientBirthDateDateEdit) ui->searchPatientBirthDateDateEdit->setStyleSheet(dateCss);

    if (ui->clearAllFilterFieldsBtn) ui->clearAllFilterFieldsBtn->setStyleSheet(buttonCss);
    if (ui->clearAllSearchFieldsBtn) ui->clearAllSearchFieldsBtn->setStyleSheet(buttonCss);

    if (ui->tableViewWorklist) ui->tableViewWorklist->setStyleSheet(tableCss);
	if (ui->tabWidgetQueueList) ui->tabWidgetQueueList->setStyleSheet(tabCss);
	
    if (ui->filterGroupBox) ui->filterGroupBox->setStyleSheet(groupCss);
    if (ui->searchGroupBox) ui->searchGroupBox->setStyleSheet(groupCss);
    if (ui->worklistGroupBox) ui->worklistGroupBox->setStyleSheet(groupCss);

    // Option 1: If you used the standard Close button in the QDialogButtonBox:
    if (auto btn = ui->buttonBox->button(QDialogButtonBox::Close)) {
        connect(btn, &QPushButton::clicked, this, [this]() {
            emit closeWorklistPage();
        });
    }

}

void WorkListPage::closePage()
{
    hide();  // or setVisible(false);
}

void WorkListPage::setProxyModel(QAbstractItemModel* model)
{
   ui->tableViewWorklist->setModel(model);
   ui->tableViewWorklist->setEditTriggers(QAbstractItemView::NoEditTriggers);
   ui->tableViewWorklist->horizontalHeader()->setStretchLastSection(true);
   ui->tableViewWorklist->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
   ui->tableViewWorklist->horizontalHeader()->setStretchLastSection(true);

   // Configure row selection behavior
   ui->tableViewWorklist->setSelectionBehavior(QAbstractItemView::SelectRows);
   ui->tableViewWorklist->setSelectionMode(QAbstractItemView::SingleSelection);

   // Connect double-click signal
   connect(ui->tableViewWorklist, &QTableView::doubleClicked, this, [this](const QModelIndex& index) {
       if (index.isValid()) {
           int entryId = index.data(Qt::UserRole).toInt();
           emit worklistItemDoubleClicked(entryId);
       }
   });
}

QTableView* WorkListPage::getWorklistTableView() const
{
    return ui->tableViewWorklist;
}

WorkListPage::~WorkListPage()
{
    delete ui;
}

void WorkListPage::clearAllFilterBtnClicked()
{
    // Reset to default: 1 year ago to today
    QDate today = QDate::currentDate();
    QDate oneYearAgo = today.addYears(-1);

    // filterDueDateEdit is the "From" field (start date)
    // filterStartDateEdit is the "To" field (end date)
    ui->filterDueDateEdit->setDate(oneYearAgo);
    ui->filterStartDateEdit->setDate(today);

    // Reset source filter to "All"
    ui->filterSourceComboBox->setCurrentIndex(0);
}

void WorkListPage::clearAllSearchBtnClicked()
{
    ui->searchNameLineEdit->clear();
    ui->searchPatientIdLineEdit->clear();
    ui->searchAcquisionNoLineEdit->clear();
    // Clear birth date by setting to invalid/null date (empty search)
    ui->searchPatientBirthDateDateEdit->clear();
    ui->searchStudyNameTextEdit->clear();
}

// Removed: onAddNewPatientClicked. Dialog is launched elsewhere in workflow.
