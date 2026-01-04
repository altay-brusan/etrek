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
#include "IContextManager.h"
#include "ISessionContext.h"
#include "ExaminationContext.h"
#include "User.h"
#include "WorklistFilterProxyModel.h"
#include "ViewSelectionDialogBuilder.h"
#include "ViewSelectionDialogDelegate.h"
#include "DelegateParameter.h"
#include "EntityStatusService.h"
#include "EntityStatus.h"


using namespace Etrek::Worklist::Repository;

namespace Etrek::Application::Delegate
{
    namespace mdl = Etrek::ScanProtocol::Data::Model;

    WorkListPageDelegate::WorkListPageDelegate(WorkListPage* ui,
        std::shared_ptr<WorklistRepository> repository,
        std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanRepository,
        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepository,
        std::shared_ptr<Etrek::Dicom::Repository::DicomTagRepository> dicomTagRepository,
        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> dbConnection,
        std::weak_ptr<Etrek::Context::IContextManager> contextManager,
        QObject* parent)
        : QObject(parent)
        , ui(ui)
        , repository(repository)
        , scanRepository(scanRepository)
        , dicomRepository(dicomRepository)
        , dicomTagRepository(dicomTagRepository)
        , dbConnection(dbConnection)
        , contextManager(contextManager)
        , entityStatusService(std::make_shared<Etrek::Dicom::Service::EntityStatusService>(dicomRepository)) {

        baseModel = new QStandardItemModel(this);
        proxyModel = new WorklistFilterProxyModel(this);
        proxyModel->setSourceModel(baseModel);

        connect(ui, &WorkListPage::addNewPatient, this, &WorkListPageDelegate::onAddNewPatient);
        connect(ui, &WorkListPage::updatePatient, this, &WorkListPageDelegate::onUpdatePatient);
        connect(ui, &WorkListPage::worklistItemDoubleClicked, this, &WorkListPageDelegate::onWorklistItemDoubleClicked);
        connect(ui, &WorkListPage::filterDateSpanChanged, this, &WorkListPageDelegate::onFilterDateRangeChanged);
        connect(ui, &WorkListPage::filterSourceChanged, this, &WorkListPageDelegate::onSourceChanged);
        connect(ui, &WorkListPage::clearAllFilters, this, &WorkListPageDelegate::onClearFilters);

        connect(ui, &WorkListPage::clearAllSearch, this, &WorkListPageDelegate::onClearSearch);
        connect(ui, &WorkListPage::searchName, this, &WorkListPageDelegate::onSearchName);
        connect(ui, &WorkListPage::searchPatientId, this, &WorkListPageDelegate::onSearchPatientId);
        connect(ui, &WorkListPage::searchAcquisionNo, this, &WorkListPageDelegate::onSearchAcquisionNo);
        connect(ui, &WorkListPage::searchPatientDate, this, &WorkListPageDelegate::onSearchPatientDate);
        connect(ui, &WorkListPage::searchStudyName, this, &WorkListPageDelegate::onSearchStudyName);

        connect(ui, &WorkListPage::closeWorklistPage, this, [this]() {
            if (this->ui) this->closeWorklist();
            });

        // Connect selection changed signal to enable/disable Update button
        if (auto* tableView = ui->getWorklistTableView()) {
            // Need to defer this connection until after the model is set
            // Will connect in loadWorklistData after first model initialization
        }

        // connect(repository.get(), &WorklistRepository::worklistEntryCreated,
        //     this, &WorkListPageDelegate::onEntryCreated);

        // connect(repository.get(), &WorklistRepository::worklistEntryUpdated,
        //     this, &WorkListPageDelegate::onEntryUpdated);

        // connect(repository.get(), &WorklistRepository::worklistEntryDeleted,
        //     this, &WorkListPageDelegate::onEntryDeleted);


        onClearFilters();

        // Setup periodic refresh timer
        setupRefreshTimer();
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
                // Track entity status for each created MWL entry
                if (entityStatusService) {
                    // Get user ID from session context
                    int userId = -1;
                    if (auto ctxMgr = contextManager.lock()) {
                        if (auto sessionCtx = ctxMgr->sessionContext()) {
                            if (auto user = sessionCtx->currentUser()) {
                                userId = user->Id;
                            }
                        }
                    }

                    using namespace Etrek::Dicom::Data::Entity;
                    for (const auto& entry : result.value) {
                        auto statusResult = entityStatusService->scheduleEntity(
                            EntityType::STUDY,
                            entry.Id,
                            userId,
                            Priority::NORMAL);

                        if (statusResult.isSuccess) {
                            qDebug() << "[WorkListPageDelegate] Entity status set to SCHEDULED for entry:" << entry.Id;
                        } else {
                            qWarning() << "[WorkListPageDelegate] Failed to track entity status for entry:"
                                       << entry.Id << "-" << statusResult.message;
                        }
                    }
                }

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

    void WorkListPageDelegate::onFilterDateRangeChanged(const DateTimeSpan& dateSpan) {
        if (proxyModel) {
            proxyModel->setDateRangeFilter(dateSpan.from.date(), dateSpan.to.date());
        }
    }


    void WorkListPageDelegate::onSourceChanged(const QString& source)
    {
        if (proxyModel) {
            // Empty string or "All" means no source filter
            QString filterValue = (source.isEmpty() || source.compare("All", Qt::CaseInsensitive) == 0)
                ? QString()
                : source;
            proxyModel->setSourceFilter(filterValue);
        }
    }

    void WorkListPageDelegate::onClearFilters() {
        // Load all worklist data from repository
        auto result = repository->getWorklistEntries(nullptr, nullptr);
        if (result.isSuccess) {
            loadWorklistData(result.value);
        }

        // Apply default filter: last 1 year to today, no source filter
        if (proxyModel) {
            QDate today = QDate::currentDate();
            QDate oneYearAgo = today.addYears(-1);
            proxyModel->setDateRangeFilter(oneYearAgo, today);
            proxyModel->setSourceFilter(QString()); // Clear source filter
        }
    }

    void WorkListPageDelegate::onSearchChanged() {
        applySearch();
    }

    void WorkListPageDelegate::onClearSearch()
    {
        if (proxyModel) {
            proxyModel->clearSearch();
        }
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
        qDebug() << "[WorkListPageDelegate] ========== loadWorklistData START ==========";
        qDebug() << "[WorkListPageDelegate] Loading" << entries.size() << "entries from database";
        qDebug() << "[WorkListPageDelegate] Current model row count:" << baseModel->rowCount();

        // Remove all existing rows but keep the model structure
        if (baseModel->rowCount() > 0) {
            baseModel->removeRows(0, baseModel->rowCount());
            qDebug() << "[WorkListPageDelegate] Removed all existing rows";
        }

        // Set headers (needed after first clear or if model was empty)
        QStringList headers;
        headers << "Patient Name"
                << "Patient ID"
                << "Study Name"
                << "Gender"
                << "Birth Date"
                << "Accession Number"
                << "Admission ID"
                << "Status"
                << "Workflow Status"
                << "Assigned To"
                << "Source"
                << "Created At";

        if (baseModel->columnCount() != headers.size()) {
            baseModel->setHorizontalHeaderLabels(headers);
            qDebug() << "[WorkListPageDelegate] Headers set, column count:" << baseModel->columnCount();
        }

        // Set model to the view only on first call (initialization)
        if (!modelInitialized) {
            ui->setProxyModel(proxyModel);
            modelInitialized = true;
            qDebug() << "[WorkListPageDelegate] Model initialized and connected to view";

            // Connect selection model to enable/disable Update button
            auto* tableView = ui->getWorklistTableView();
            if (tableView && tableView->selectionModel()) {
                connect(tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
                    this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
                        // Enable Update button if a row is selected, disable if not
                        bool hasSelection = !selected.isEmpty();
                        if (auto* updateBtn = ui->findChild<QPushButton*>("updatePatientBtn")) {
                            updateBtn->setEnabled(hasSelection);
                        }
                    });
                qDebug() << "[WorkListPageDelegate] Connected selection model to Update button state";
            }
        }

        // Populate rows
        int rowsAdded = 0;
        for (const auto& entry : entries) {
            baseModel->appendRow(createRowForEntry(entry));
            rowsAdded++;

            // Log first entry and every entry with non-PENDING status for debugging
            if (rowsAdded == 1 || entry.Status != ProcedureStepStatus::PENDING) {
                qDebug() << "[WorkListPageDelegate] Row" << rowsAdded << "- Entry ID:" << entry.Id
                         << "Status:" << ProcedureStepStatusToString(entry.Status);
            }
        }

        qDebug() << "[WorkListPageDelegate] Added" << rowsAdded << "rows to base model";
        qDebug() << "[WorkListPageDelegate] Base model now has" << baseModel->rowCount() << "rows";

        // Force proxy model to re-filter and update
        if (proxyModel) {
            proxyModel->invalidate();
            qDebug() << "[WorkListPageDelegate] Proxy model invalidated, visible rows:" << proxyModel->rowCount();
        }

        qDebug() << "[WorkListPageDelegate] ========== loadWorklistData END ==========";
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
        if (proxyModel) {
            proxyModel->setSearchName(name);
        }
    }

    void WorkListPageDelegate::onSearchPatientId(const QString& patientId)
    {
        if (proxyModel) {
            proxyModel->setSearchPatientId(patientId);
        }
    }

    void WorkListPageDelegate::onSearchAcquisionNo(const QString& accessionNo)
    {
        if (proxyModel) {
            proxyModel->setSearchAccessionNo(accessionNo);
        }
    }

    void WorkListPageDelegate::onSearchPatientDate(const QDate& birthDate)
    {
        if (proxyModel) {
            proxyModel->setSearchBirthDate(birthDate);
        }
    }

    void WorkListPageDelegate::onSearchStudyName(const QString& studyName)
    {
        if (proxyModel) {
            proxyModel->setSearchStudyName(studyName);
        }
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

    QList<QStandardItem*> WorkListPageDelegate::createRowForEntry(const ent::WorklistEntry& entry) {
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

        // Helper lambda to create workflow status item with color coding
        auto createWorkflowStatusItem = [&entry](const QString& text,
                                                  Etrek::Dicom::Data::Entity::WorkflowStatus status) -> QStandardItem* {
            QStandardItem* item = new QStandardItem(text);
            item->setData(entry.Id, Qt::UserRole);

            // Color code based on workflow status
            QColor color;
            switch (status) {
                case Etrek::Dicom::Data::Entity::WorkflowStatus::SCHEDULED:
                    color = QColor(128, 128, 128);  // Gray
                    break;
                case Etrek::Dicom::Data::Entity::WorkflowStatus::IN_PROGRESS:
                    color = QColor(70, 130, 180);   // Steel Blue
                    break;
                case Etrek::Dicom::Data::Entity::WorkflowStatus::COMPLETED:
                    color = QColor(60, 179, 113);   // Medium Sea Green
                    break;
                case Etrek::Dicom::Data::Entity::WorkflowStatus::CANCELLED:
                    color = QColor(220, 20, 60);    // Crimson Red
                    break;
                case Etrek::Dicom::Data::Entity::WorkflowStatus::ABORTED:
                    color = QColor(255, 140, 0);    // Dark Orange
                    break;
                case Etrek::Dicom::Data::Entity::WorkflowStatus::PENDING:
                default:
                    color = QColor(208, 208, 208);  // Default gray text
                    break;
            }
            item->setData(color, Qt::ForegroundRole);
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

        // Column 7: Status (from WorklistEntry.Status enum - MWL procedure step status)
        row << createItem(ProcedureStepStatusToString(entry.Status));

        // Column 8: Workflow Status (from entity_status table)
        // Column 9: Assigned To (from entity_status table)
        QString workflowStatusText = "-";
        QString assignedToText = "-";
        Etrek::Dicom::Data::Entity::WorkflowStatus workflowStatus =
            Etrek::Dicom::Data::Entity::WorkflowStatus::PENDING;

        if (entityStatusService) {
            using namespace Etrek::Dicom::Data::Entity;
            auto statusResult = entityStatusService->getCurrentStatus(EntityType::STUDY, entry.Id);
            if (statusResult.isSuccess && statusResult.value.has_value()) {
                const auto& entityStatus = statusResult.value.value();
                workflowStatus = entityStatus.Status;
                workflowStatusText = EntityStatus::WorkflowStatusToString(entityStatus.Status);

                // Show assigned user ID (could be enhanced to show username with user lookup)
                if (entityStatus.AssignedTo >= 0) {
                    assignedToText = QString("User %1").arg(entityStatus.AssignedTo);
                }
            }
        }
        row << createWorkflowStatusItem(workflowStatusText, workflowStatus);
        row << createItem(assignedToText);

        // Column 10: Source (from WorklistEntry.Source enum)
        row << createItem(SourceToString(entry.Source));

        // Column 11: Created At (from WorklistEntry.CreatedAt timestamp)
        row << createItem(entry.CreatedAt.toString("yyyy-MM-dd HH:mm"));

        return row;
    }

    QString WorkListPageDelegate::name() const
    {
        return QString();
    }

    void WorkListPageDelegate::attachDelegates(const QVector<QObject*>& delegates)
    {
    }

    WorkListPageDelegate::~WorkListPageDelegate()
    {
        // Stop and clean up the refresh timer
        if (refreshTimer) {
            refreshTimer->stop();
            qDebug() << "[WorkListPageDelegate] Refresh timer stopped";
        }
    }

    void WorkListPageDelegate::refreshWorklistData()
    {
        // Safety check: ensure UI is valid before accessing
        if (!ui) {
            // UI has been destroyed - this is normal when navigating away from the page
            // The timer will continue running but won't update the non-existent view
            return;
        }

        qDebug() << "";
        qDebug() << "#################### REFRESH TRIGGERED ####################";
        qDebug() << "[WorkListPageDelegate] Timer-triggered refresh at" << QDateTime::currentDateTime().toString("hh:mm:ss");

        // Get table view reference
        auto* tableView = ui->getWorklistTableView();
        if (!tableView) {
            qWarning() << "[WorkListPageDelegate] TableView is null, cannot refresh";
            return;
        }

        qDebug() << "[WorkListPageDelegate] Current view row count:" << (tableView->model() ? tableView->model()->rowCount() : -1);

        // Save the currently selected entry ID before refresh
        int selectedEntryId = -1;
        if (tableView->selectionModel()) {
            auto selectedIndexes = tableView->selectionModel()->selectedRows();
            if (!selectedIndexes.isEmpty()) {
                selectedEntryId = selectedIndexes.first().data(Qt::UserRole).toInt();
                qDebug() << "[WorkListPageDelegate] Saved selected entry ID:" << selectedEntryId;
            }
        }

        // Reload all worklist entries from the database
        auto result = repository->getWorklistEntries(nullptr, nullptr);
        if (result.isSuccess) {
            qDebug() << "[WorkListPageDelegate] Successfully loaded" << result.value.size() << "entries from database";

            // Load the data into the model
            loadWorklistData(result.value);

            // Additional forced updates to ensure view refreshes
            if (proxyModel) {
                // Reset the proxy model to force complete refresh
                proxyModel->invalidate();

                qDebug() << "[WorkListPageDelegate] After refresh - Proxy model row count:" << proxyModel->rowCount();
            }

            // Force the view to repaint
            tableView->reset();
            tableView->viewport()->update();
            tableView->update();

            // Restore the selection if there was one
            if (selectedEntryId != -1 && tableView->model()) {
                bool selectionRestored = false;
                for (int row = 0; row < tableView->model()->rowCount(); ++row) {
                    QModelIndex index = tableView->model()->index(row, 0);
                    int entryId = index.data(Qt::UserRole).toInt();
                    if (entryId == selectedEntryId) {
                        tableView->selectRow(row);
                        selectionRestored = true;
                        qDebug() << "[WorkListPageDelegate] Restored selection to row" << row << "with entry ID" << selectedEntryId;
                        break;
                    }
                }
                if (!selectionRestored) {
                    qDebug() << "[WorkListPageDelegate] Could not restore selection - entry ID" << selectedEntryId << "not found";
                }
            }

            qDebug() << "[WorkListPageDelegate] View reset and repainted";
            qDebug() << "[WorkListPageDelegate] Final view row count:" << tableView->model()->rowCount();
            qDebug() << "#################### REFRESH COMPLETE ####################";
            qDebug() << "";
        } else {
            qWarning() << "[WorkListPageDelegate] Failed to refresh worklist:" << result.message;
            qDebug() << "#################### REFRESH FAILED ####################";
            qDebug() << "";
        }
    }

    void WorkListPageDelegate::apply()
    {
    }

    void WorkListPageDelegate::accept()
    {
    }

    void WorkListPageDelegate::reject()
    {
    }

    void WorkListPageDelegate::onWorklistItemDoubleClicked(int entryId)
    {
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

        // Create and store ExaminationContext
        if (auto ctxMgr = contextManager.lock()) {
            auto examContext = std::make_shared<Etrek::Core::Context::ExaminationContext>(selectedEntry);
            ctxMgr->setWorkflowContext("Examination", examContext);

            // Show ViewSelectionDialog to allow user to select procedure and views
            // Pass only dbConnection - ViewSelectionDialogBuilder will create its own repository
            DelegateParameter params;
            params.dbConnection = dbConnection;
            params.contextManager = contextManager;

            Etrek::Application::Delegate::ViewSelectionDialogBuilder builder;
            auto [dialog, delegate] = builder.build(params, ui, this);

            // Connect dialog signals
            connect(delegate, &Etrek::Application::Delegate::ViewSelectionDialogDelegate::examinationReady,
                    this, [this, entryId](int procedureId, const QVector<int>& viewIds) {
                qDebug() << "Selected Procedure ID:" << procedureId;
                qDebug() << "Selected View IDs:" << viewIds;

                // Get user ID from session context for status tracking
                int userId = -1;
                if (auto ctxMgr = contextManager.lock()) {
                    auto workflowCtx = ctxMgr->workflowContext("Examination");
                    auto examCtx = std::dynamic_pointer_cast<Etrek::Core::Context::ExaminationContext>(workflowCtx);

                    if (examCtx) {
                        examCtx->setProcedureId(procedureId);
                        examCtx->setViewIds(viewIds);
                        qDebug() << "[WorkListPageDelegate] Updated ExaminationContext with procedure and views";
                    }

                    // Get user ID from session context
                    if (auto sessionCtx = ctxMgr->sessionContext()) {
                        if (auto user = sessionCtx->currentUser()) {
                            userId = user->Id;
                        }
                    }
                }

                // Update worklist status to IN_PROGRESS
                auto statusResult = repository->updateWorklistStatus(entryId, ProcedureStepStatus::IN_PROGRESS);
                if (statusResult.isSuccess) {
                    qDebug() << "[WorkListPageDelegate] Updated worklist entry to IN_PROGRESS";
                } else {
                    qWarning() << "[WorkListPageDelegate] Failed to update status:" << statusResult.message;
                }

                // Track entity status in entity_status table
                if (entityStatusService) {
                    using namespace Etrek::Dicom::Data::Entity;
                    auto entityStatusResult = entityStatusService->startExamination(
                        EntityType::STUDY,
                        entryId,
                        userId,
                        Priority::NORMAL);

                    if (entityStatusResult.isSuccess) {
                        qDebug() << "[WorkListPageDelegate] Entity status set to IN_PROGRESS";
                    } else {
                        qWarning() << "[WorkListPageDelegate] Failed to track entity status:" << entityStatusResult.message;
                    }
                }

                emit startExamination(entryId);
            });

            // Show dialog modally
            dialog->exec();

            // Clean up
            dialog->deleteLater();
            delegate->deleteLater();
        } else {
            QMessageBox::warning(ui, "Context Error",
                               "Context manager is not available. Cannot proceed with examination.");
        }
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

        // For body parts, try to extract from DICOM tags if available
        QString bodyPartExamined = tagMap.value("BodyPartExamined", "");
        QString requestedProcedure = tagMap.value("RequestedProcedureDescription", "");

        // Try to match body parts from the scan repository
        if (!bodyPartExamined.isEmpty() || !requestedProcedure.isEmpty()) {
            auto regionsRes = scanRepository->getAllAnatomicRegions();
            auto partsRes = scanRepository->getAllBodyParts();

            if (regionsRes.isSuccess && partsRes.isSuccess) {
                QString searchTerm = !bodyPartExamined.isEmpty() ? bodyPartExamined : requestedProcedure;

                for (const auto& bodyPart : partsRes.value) {
                    if (bodyPart.Name.contains(searchTerm, Qt::CaseInsensitive) ||
                        searchTerm.contains(bodyPart.Name, Qt::CaseInsensitive)) {
                        for (const auto& region : regionsRes.value) {
                            if (region.Id == bodyPart.Region.Id) {
                                BodyPartSelection selection;
                                selection.region = region;
                                selection.bodyPart = bodyPart;
                                patient.selectedBodyParts.append(selection);
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }

        return patient;
    }

    void WorkListPageDelegate::setupRefreshTimer()
    {
        // Create timer for periodic worklist refresh
        refreshTimer = new QTimer(this);

        // Set refresh interval to 5 seconds (5000 milliseconds)
        refreshTimer->setInterval(5000);

        // Connect timer timeout to refresh method
        connect(refreshTimer, &QTimer::timeout, this, &WorkListPageDelegate::refreshWorklistData);

        // Start the timer
        refreshTimer->start();

        qDebug() << "[WorkListPageDelegate] Periodic refresh timer started (5 second interval)";
    }


}


