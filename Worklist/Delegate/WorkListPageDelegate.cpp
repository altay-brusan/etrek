#include <QHeaderView>
#include <QDateEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QDateTime>
#include <QStandardItem>
#include <QMessageBox>

#include "WorkListPageDelegate.h"
#include "WorklistRepository.h"
#include "ScanProtocolRepository.h"
#include "AddPatientDialog.h"
#include "LocalMwlRegistrationService.h"


using namespace Etrek::Worklist::Repository;

namespace Etrek::Worklist::Delegate
{
    WorkListPageDelegate::WorkListPageDelegate(WorkListPage* ui,
        std::shared_ptr<WorklistRepository> repository,
        std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanRepository,
        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepository,
        std::shared_ptr<Etrek::Dicom::Repository::DicomTagRepository> dicomTagRepository,
        QObject* parent)
        : QObject(parent)
        , ui(ui)
        , repository(repository)
        , scanRepository(scanRepository)
        , dicomRepository(dicomRepository)
        , dicomTagRepository(dicomTagRepository) {
         
        baseModel = new QStandardItemModel(this);
        proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(baseModel);
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxyModel->setFilterKeyColumn(-1);

        connect(ui, &WorkListPage::addNewPatient, this, &WorkListPageDelegate::onAddNewPatient);
        connect(ui, &WorkListPage::filterDateSpanChanged, this, &WorkListPageDelegate::onFilterDateRangeChanged);
        connect(ui, &WorkListPage::filterSourceChanged, this, &WorkListPageDelegate::onSourceChanged);
        connect(ui, &WorkListPage::clearAllFilters, this, &WorkListPageDelegate::onClearFilters);
        
        connect(ui, &WorkListPage::clearAllSearch, this, &WorkListPageDelegate::onClearSearch);
        connect(ui, &WorkListPage::searchName, this, &WorkListPageDelegate::onSearchName);
        connect(ui, &WorkListPage::searchPatientId, this, &WorkListPageDelegate::onSearchPatientId);
        connect(ui, &WorkListPage::searchAcquisionNo, this, &WorkListPageDelegate::onSearchAcquisionNo);
        connect(ui, &WorkListPage::searchPatientDate, this, &WorkListPageDelegate::onSearchPatientDate);
        connect(ui, &WorkListPage::searchStudyId, this, &WorkListPageDelegate::onSearchStudyId);

        connect(ui, &WorkListPage::closeWorklistPage, this, [this]() {
            if (this->ui) this->closeWorklist();
            });

        // connect(repository.get(), &WorklistRepository::worklistEntryCreated,
        //     this, &WorkListPageDelegate::onEntryCreated);

        // connect(repository.get(), &WorklistRepository::worklistEntryUpdated,
        //     this, &WorkListPageDelegate::onEntryUpdated);

        // connect(repository.get(), &WorklistRepository::worklistEntryDeleted,
        //     this, &WorkListPageDelegate::onEntryDeleted);


        onClearFilters();
    }

    void WorkListPageDelegate::onAddNewPatient()
    {
        if (!scanRepository || !dicomRepository || !dicomTagRepository)
            return;

        auto regionsRes = scanRepository->getAllAnatomicRegions();
        auto partsRes = scanRepository->getAllBodyParts();
        if (!regionsRes.isSuccess || !partsRes.isSuccess)
            return;

        // Construct dialog with injected entities
        AddPatientDialog dlg(regionsRes.value, partsRes.value, ui);

        if (dlg.exec() == QDialog::Accepted) {
            // Get patient data from dialog
            auto patientData = dlg.getPatientModel();

            // Validate patient data
            if (!patientData.isValid()) {
                QMessageBox::warning(ui, "Invalid Data",
                    "Please ensure all required fields are filled and at least one body part is selected.");
                return;
            }

            // Create registration service
            Etrek::Worklist::Service::LocalMwlRegistrationService registrationService(
                dicomRepository, dicomTagRepository);

            // Register patient and create MWL entries
            auto result = registrationService.registerPatient(patientData);

            if (result.isSuccess) {
                // Success - show message and refresh the worklist
                QString message = QString("Successfully registered patient with %1 MWL entry(ies).")
                    .arg(result.value.size());
                QMessageBox::information(ui, "Patient Registered", message);

                // Refresh the worklist display
                onClearFilters();
            } else {
                // Error - show error message
                QMessageBox::critical(ui, "Registration Failed",
                    QString("Failed to register patient:\n%1").arg(result.message));
            }
        }
    }

    void WorkListPageDelegate::onFilterDateRangeChanged(const DateTimeSpan& date) {
        //applyFilters();
    }

     
    void WorkListPageDelegate::onSourceChanged(const QString& source)
    {
    }

    void WorkListPageDelegate::onClearFilters() {

         auto result = repository->getWorklistEntries(nullptr, nullptr);
         if (result.isSuccess) {
             loadWorklistData(result.value);
         }
    }

    void WorkListPageDelegate::onSearchChanged() {
        applySearch();
    }

    void WorkListPageDelegate::onClearSearch() 
    {		
        proxyModel->setFilterFixedString("");
    }

    void WorkListPageDelegate::applyFilters() {
        // QDateTime from, to;
        // QDateTime* fromPtr = nullptr;
        // QDateTime* toPtr = nullptr;

        // if (ui->filterStartDateEdit->date().isValid()) {
        //     from = QDateTime(ui->filterStartDateEdit->date(), QTime(0, 0));
        //     fromPtr = &from;
        // }
        // if (ui->filterDueDateEdit->date().isValid()) {
        //     to = QDateTime(ui->filterDueDateEdit->date(), QTime(23, 59, 59));
        //     toPtr = &to;
        // }

        // QString sourceText = ui->filterSourceLineEdit->text().trimmed();
        // auto result = repository->getWorklistEntries(fromPtr, toPtr);
        // if (result.isSuccess) {
        //     QList<WorklistEntry> filteredList;
        //     const QList<WorklistEntry>& entriesRef = result.value;
        //     for (const auto& entry : entriesRef) {
        //         QString sourceStr = SourceToString(entry.Source);
        //         if (sourceText.isEmpty() || sourceStr.contains(sourceText, Qt::CaseInsensitive)) {
        //             filteredList.append(entry);
        //         }
        //     }
        //     const QList<WorklistEntry>& filteredListRef = filteredList;
        //     loadWorklistData(filteredList);
        // }
    }

    void WorkListPageDelegate::applySearch() {
        QStringList parts;

        // if (!ui->searchNameLineEdit->text().isEmpty())
        //     parts << ui->searchNameLineEdit->text();
        // if (!ui->searchPatientIdLineEdit->text().isEmpty())
        //     parts << ui->searchPatientIdLineEdit->text();
        // if (!ui->searchAcquisionNoLineEdit->text().isEmpty())
        //     parts << ui->searchAcquisionNoLineEdit->text();
        // if (ui->searchPatientBirthDateDateEdit->date().isValid())
        //     parts << ui->searchPatientBirthDateDateEdit->date().toString("yyyy-MM-dd");
        // if (!ui->searchStudyIdTextEdit->text().isEmpty())
        //     parts << ui->searchStudyIdTextEdit->text();

        // proxyModel->setFilterFixedString(parts.join(" "));
    }

    void WorkListPageDelegate::loadWorklistData(const QList<ent::WorklistEntry>& entries) {
        baseModel->clear();

        const QList<ent::DicomTag> tagList = getDisplayTagList();
        if (tagList.isEmpty()) return;

        // Headers
        QStringList headers;
        for (const auto& tag : tagList)
            headers << (tag.DisplayName.isEmpty() ? tag.Name : tag.DisplayName);

        headers << "Status" << "Created At" << "Source";
        baseModel->setHorizontalHeaderLabels(headers);

        // Populate rows
        for (const auto& entry : entries)
            baseModel->appendRow(createRowForEntry(entry));
    }

    void WorkListPageDelegate::onEntryCreated(const ent::WorklistEntry& entry) {
        const QList<ent::DicomTag> tagList = getDisplayTagList();
        if (tagList.isEmpty()) return;

        QList<QStandardItem*> row = createRowForEntry(entry);
        baseModel->appendRow(row);
    }

    void WorkListPageDelegate::onEntryUpdated(const ent::WorklistEntry& entry) {
        // Find the row with matching entry ID
        int targetRow = -1;
        for (int row = 0; row < baseModel->rowCount(); ++row) {
            auto idIndex = baseModel->index(row, 0); // assuming ID is stored in column 0 data role
            int rowId = idIndex.data(Qt::UserRole).toInt();
            if (rowId == entry.Id) {
                targetRow = row;
                break;
            }
        }

        if (targetRow == -1)
            return; // Entry not found

        // Build new row items (reuse utility function if you created one like createRowForEntry)
        QList<QStandardItem*> updatedItems = createRowForEntry(entry);
        if (updatedItems.size() != baseModel->columnCount())
            return; // Structure mismatch

        // Replace item values in-place
        for (int col = 0; col < updatedItems.size(); ++col) {
            QStandardItem* currentItem = baseModel->item(targetRow, col);
            QStandardItem* updatedItem = updatedItems[col];

            currentItem->setText(updatedItem->text());
            currentItem->setData(updatedItem->data(Qt::ForegroundRole), Qt::ForegroundRole);
            currentItem->setData(entry.Id, Qt::UserRole); // Keep Id stored
        }

        // Optionally delete temp items
        qDeleteAll(updatedItems);
    }

    void WorkListPageDelegate::onEntryDeleted(int entryId) {
       // onFilterChanged();  // optionally just remove row by ID
    }

    void WorkListPageDelegate::onSearchName(const QString& name)
    {
    }

    void WorkListPageDelegate::onSearchPatientId(const QString& patientId)
    {
    }

    void WorkListPageDelegate::onSearchAcquisionNo(const QString& acquisionNo)
    {
    }

    void WorkListPageDelegate::onSearchPatientDate(const QDate& acquisionNo)
    {
    }

    void WorkListPageDelegate::onSearchStudyId(const QString& studyId)
    {
    }

    QList<ent::DicomTag> WorkListPageDelegate::getDisplayTagList() const {
        auto mandatoryResult = repository->getMandatoryIdentifierTags(1);
        auto activeResult = repository->getActiveIdentifierTags(1);

        if (!mandatoryResult.isSuccess || !activeResult.isSuccess)
            return {};

        QMap<QString, ent::DicomTag> combinedTags;
        for (const auto& tag : mandatoryResult.value) combinedTags.insert(tag.Name, tag);
        for (const auto& tag : activeResult.value) combinedTags.insert(tag.Name, tag);

        QList<ent::DicomTag> tagList = combinedTags.values();
        std::sort(tagList.begin(), tagList.end(), [](const ent::DicomTag& a, const ent::DicomTag& b) {
            return a.DisplayName.toLower() < b.DisplayName.toLower();
            });

        return tagList;
    }

    QList<QStandardItem*> WorkListPageDelegate::createRowForEntry(const ent::WorklistEntry& entry) const {
        const QList<ent::DicomTag> tagList = getDisplayTagList();
        if (tagList.isEmpty()) return {};

        QMap<QString, QString> tagMap;
        for (const auto& attr : entry.Attributes)
            tagMap[attr.Tag.Name] = attr.TagValue;

        QList<QStandardItem*> row;
        for (const ent::DicomTag& tag : tagList) {
            QString value = tagMap.value(tag.Name, "");
            QStandardItem* item = new QStandardItem(value);
            item->setData(QColor(208, 208, 208), Qt::ForegroundRole);
            item->setData(entry.Id, Qt::UserRole);  // Store WorklistEntry ID
            row << item;
        }

        QStandardItem* statusItem = new QStandardItem(ProcedureStepStatusToString(entry.Status));
        statusItem->setData(QColor(208, 208, 208), Qt::ForegroundRole);
        row << statusItem;

        QStandardItem* createdAtItem = new QStandardItem(entry.CreatedAt.toString("yyyy-MM-dd"));
        createdAtItem->setData(QColor(208, 208, 208), Qt::ForegroundRole);
        row << createdAtItem;

        QStandardItem* sourceItem = new QStandardItem(SourceToString(entry.Source));
        sourceItem->setData(QColor(208, 208, 208), Qt::ForegroundRole);
        row << sourceItem;

        return row;
    }

    QString Delegate::WorkListPageDelegate::name() const
    {
        return QString();
    }

    void Delegate::WorkListPageDelegate::attachDelegates(const QVector<QObject*>& delegates)
    {
    }

    Delegate::WorkListPageDelegate::~WorkListPageDelegate()
    {
    }

    void Delegate::WorkListPageDelegate::apply()
    {
    }

    void Delegate::WorkListPageDelegate::accept()
    {
    }

    void Delegate::WorkListPageDelegate::reject()
    {
    }

}


