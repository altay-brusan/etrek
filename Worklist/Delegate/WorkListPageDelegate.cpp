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
    namespace mdl = Etrek::ScanProtocol::Data::Model;

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
        connect(ui, &WorkListPage::updatePatient, this, &WorkListPageDelegate::onUpdatePatient);
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

    void WorkListPageDelegate::onUpdatePatient()
    {
        if (!scanRepository || !dicomRepository || !dicomTagRepository)
            return;

        // Get the worklist table view
        auto* tableView = ui->getWorklistTableView();
        if (!tableView || !tableView->selectionModel())
            return;

        // Get selected row
        auto selectedIndexes = tableView->selectionModel()->selectedRows();
        if (selectedIndexes.isEmpty())
            return;

        // Get the entry ID from the first column of the selected row
        QModelIndex selectedIndex = selectedIndexes.first();
        int entryId = selectedIndex.data(Qt::UserRole).toInt();

        // Find the WorklistEntry with this ID
        auto result = repository->getWorklistEntries(nullptr, nullptr);
        if (!result.isSuccess)
            return;

        ent::WorklistEntry selectedEntry;
        bool found = false;
        for (const auto& entry : result.value) {
            if (entry.Id == entryId) {
                selectedEntry = entry;
                found = true;
                break;
            }
        }

        if (!found)
            return;

        // Convert WorklistEntry to PatientModel
        auto patientData = worklistEntryToPatientModel(selectedEntry);

        // Get regions and body parts
        auto regionsRes = scanRepository->getAllAnatomicRegions();
        auto partsRes = scanRepository->getAllBodyParts();
        if (!regionsRes.isSuccess || !partsRes.isSuccess)
            return;

        // Construct dialog with injected entities
        AddPatientDialog dlg(regionsRes.value, partsRes.value, ui);

        // Set dialog mode for update
        dlg.setDialogMode("Update Patient", "Update");

        // Prefill with patient data
        dlg.setPatientModel(patientData);

        if (dlg.exec() == QDialog::Accepted) {
            // Get updated patient data from dialog
            auto updatedPatientData = dlg.getPatientModel();

            // Validate patient data
            if (!updatedPatientData.isValid()) {
                QMessageBox::warning(ui, "Invalid Data",
                    "Please ensure all required fields are filled and at least one body part is selected.");
                return;
            }

            // Create registration service
            Etrek::Worklist::Service::LocalMwlRegistrationService registrationService(
                dicomRepository, dicomTagRepository);

            // Update patient (similar to register, but could have different logic)
            auto updateResult = registrationService.registerPatient(updatedPatientData);

            if (updateResult.isSuccess) {
                // Success - show message and refresh the worklist
                QString message = QString("Successfully updated patient with %1 MWL entry(ies).")
                    .arg(updateResult.value.size());
                QMessageBox::information(ui, "Patient Updated", message);

                // Refresh the worklist display
                onClearFilters();
            } else {
                // Error - show error message
                QMessageBox::critical(ui, "Update Failed",
                    QString("Failed to update patient:\n%1").arg(updateResult.message));
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

        // Fixed column headers for consistent worklist display
        QStringList headers;
        headers << "Patient Name"
                << "Patient ID"
                << "Study Name"
                << "Gender"
                << "Birth Date"
                << "Accession Number"
                << "Admission ID"
                << "Status"
                << "Source"
                << "Created At";

        baseModel->setHorizontalHeaderLabels(headers);

        // Set model to the view (ensure table is connected)
        ui->setProxyModel(proxyModel);

        // Populate rows
        for (const auto& entry : entries)
            baseModel->appendRow(createRowForEntry(entry));
    }

    void WorkListPageDelegate::onEntryCreated(const ent::WorklistEntry& entry) {
        // Simply create and append the new row using fixed column structure
        QList<QStandardItem*> row = createRowForEntry(entry);
        if (!row.isEmpty()) {
            baseModel->appendRow(row);
        }
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
        // Build tag map from entry attributes for quick lookup
        QMap<QString, QString> tagMap;
        for (const auto& attr : entry.Attributes)
            tagMap[attr.Tag.Name] = attr.TagValue;

        // Helper lambda to create styled item
        auto createItem = [&entry](const QString& text) -> QStandardItem* {
            QStandardItem* item = new QStandardItem(text);
            item->setData(QColor(208, 208, 208), Qt::ForegroundRole);
            item->setData(entry.Id, Qt::UserRole);  // Store WorklistEntry ID for selection/updates
            return item;
        };

        QList<QStandardItem*> row;

        // Column 0: Patient Name (DICOM tag: PatientName)
        row << createItem(tagMap.value("PatientName", ""));

        // Column 1: Patient ID (DICOM tag: PatientID)
        row << createItem(tagMap.value("PatientID", ""));

        // Column 2: Study Name (DICOM tag: StudyDescription or StudyID as fallback)
        QString studyName = tagMap.value("StudyDescription", "");
        if (studyName.isEmpty()) studyName = tagMap.value("StudyID", "");
        row << createItem(studyName);

        // Column 3: Gender (DICOM tag: PatientSex)
        row << createItem(tagMap.value("PatientSex", ""));

        // Column 4: Birth Date (DICOM tag: PatientBirthDate) - format as readable date
        QString birthDate = tagMap.value("PatientBirthDate", "");
        if (!birthDate.isEmpty() && birthDate.length() == 8) {
            // Convert DICOM DA format (YYYYMMDD) to display format (YYYY-MM-DD)
            birthDate = QString("%1-%2-%3")
                .arg(birthDate.mid(0, 4))
                .arg(birthDate.mid(4, 2))
                .arg(birthDate.mid(6, 2));
        }
        row << createItem(birthDate);

        // Column 5: Accession Number (DICOM tag: AccessionNumber)
        row << createItem(tagMap.value("AccessionNumber", ""));

        // Column 6: Admission ID (DICOM tag: AdmissionID)
        row << createItem(tagMap.value("AdmissionID", ""));

        // Column 7: Status (from WorklistEntry.Status enum)
        row << createItem(ProcedureStepStatusToString(entry.Status));

        // Column 8: Source (from WorklistEntry.Source enum)
        row << createItem(SourceToString(entry.Source));

        // Column 9: Created At (from WorklistEntry.CreatedAt timestamp)
        row << createItem(entry.CreatedAt.toString("yyyy-MM-dd HH:mm"));

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

    mdl::PatientModel WorkListPageDelegate::worklistEntryToPatientModel(const ent::WorklistEntry& entry) const
    {
        using namespace Etrek::ScanProtocol::Data::Model;
        using namespace Etrek::ScanProtocol::Data::Entity;

        PatientModel patient;

        // Build tag map from entry attributes for quick lookup
        QMap<QString, QString> tagMap;
        for (const auto& attr : entry.Attributes)
            tagMap[attr.Tag.Name] = attr.TagValue;

        // Extract patient name and split into components
        QString patientName = tagMap.value("PatientName", "");
        QStringList nameParts = patientName.split("^");
        if (nameParts.size() > 0) patient.lastName = nameParts[0].trimmed();
        if (nameParts.size() > 1) patient.firstName = nameParts[1].trimmed();
        if (nameParts.size() > 2) patient.middleName = nameParts[2].trimmed();

        // Extract other demographic fields
        patient.patientId = tagMap.value("PatientID", "");
        patient.accessionNumber = tagMap.value("AccessionNumber", "");
        patient.admissionNumber = tagMap.value("AdmissionID", "");
        patient.referringPhysician = tagMap.value("ReferringPhysicianName", "");

        // Extract and parse birth date (DICOM DA format: YYYYMMDD)
        QString birthDate = tagMap.value("PatientBirthDate", "");
        if (!birthDate.isEmpty() && birthDate.length() == 8) {
            int year = birthDate.mid(0, 4).toInt();
            int month = birthDate.mid(4, 2).toInt();
            int day = birthDate.mid(6, 2).toInt();
            patient.dateOfBirth = QDate(year, month, day);

            // Calculate age
            QDate currentDate = QDate::currentDate();
            patient.age = currentDate.year() - year;
            if (currentDate.month() < month ||
                (currentDate.month() == month && currentDate.day() < day)) {
                patient.age--;
            }
        } else {
            patient.dateOfBirth = QDate::currentDate();
            patient.age = 0;
        }

        // Extract and parse gender
        QString genderStr = tagMap.value("PatientSex", "M");
        if (auto g = Etrek::ScanProtocol::ScanProtocolUtil::parseGender(genderStr))
            patient.gender = *g;

        // For body parts, we need to try to extract from DICOM tags if available
        // The body part information might be in RequestedProcedureDescription or BodyPartExamined
        QString bodyPartExamined = tagMap.value("BodyPartExamined", "");
        QString requestedProcedure = tagMap.value("RequestedProcedureDescription", "");

        // Try to match body parts from the scan repository
        if (!bodyPartExamined.isEmpty() || !requestedProcedure.isEmpty()) {
            // Get all regions and body parts
            auto regionsRes = scanRepository->getAllAnatomicRegions();
            auto partsRes = scanRepository->getAllBodyParts();

            if (regionsRes.isSuccess && partsRes.isSuccess) {
                // Try to find matching body part by name
                QString searchTerm = !bodyPartExamined.isEmpty() ? bodyPartExamined : requestedProcedure;

                for (const auto& bodyPart : partsRes.value) {
                    if (bodyPart.Name.contains(searchTerm, Qt::CaseInsensitive) ||
                        searchTerm.contains(bodyPart.Name, Qt::CaseInsensitive)) {
                        // Find the corresponding region
                        for (const auto& region : regionsRes.value) {
                            if (region.Id == bodyPart.Region.Id) {
                                BodyPartSelection selection;
                                selection.region = region;
                                selection.bodyPart = bodyPart;
                                patient.selectedBodyParts.append(selection);
                                break;
                            }
                        }
                        break; // Only add first match
                    }
                }
            }
        }

        // If no body parts were found, add a default or leave empty
        // (The dialog validation will require at least one body part)

        return patient;
    }

}


