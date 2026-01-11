#include "RisProcedureMappingConfigurationWidget.h"
#include "ui_RisProcedureMappingConfigurationWidget.h"
#include <QStandardItemModel>
#include <QMessageBox>

namespace ent = Etrek::Worklist::Data::Entity;

RisProcedureMappingConfigurationWidget::RisProcedureMappingConfigurationWidget(
    const QVector<ent::RisProcedureMapping>& mappings,
    const QStringList& connectionNames,
    QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::RisProcedureMappingConfigurationWidget)
    , m_mappings(mappings)
    , m_originalMappings(mappings)
    , m_connectionNames(connectionNames)
{
    ui->setupUi(this);

    // Setup list model
    m_listModel = new QStandardItemModel(this);
    ui->listView->setModel(m_listModel);

    // Populate connection filter combo
    ui->connectionComboBox->addItem(tr("All Connections"), QString());
    for (const auto& name : m_connectionNames) {
        ui->connectionComboBox->addItem(name, name);
    }

    // Connect signals
    connect(ui->connectionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RisProcedureMappingConfigurationWidget::onConnectionFilterChanged);
    connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &RisProcedureMappingConfigurationWidget::onMappingSelected);
    connect(ui->addButton, &QPushButton::clicked,
            this, &RisProcedureMappingConfigurationWidget::onAddClicked);
    connect(ui->deleteButton, &QPushButton::clicked,
            this, &RisProcedureMappingConfigurationWidget::onDeleteClicked);
    connect(ui->saveButton, &QPushButton::clicked,
            this, &RisProcedureMappingConfigurationWidget::onSaveClicked);

    // Initial population
    populateList();
    clearDetails();
}

RisProcedureMappingConfigurationWidget::~RisProcedureMappingConfigurationWidget()
{
    delete ui;
}

QVector<ent::RisProcedureMapping> RisProcedureMappingConfigurationWidget::getMappings() const
{
    return m_mappings;
}

QVector<ent::RisProcedureMapping> RisProcedureMappingConfigurationWidget::getModifiedMappings() const
{
    QVector<ent::RisProcedureMapping> modified;
    for (const auto& mapping : m_mappings) {
        // Find original
        bool isModified = true;
        for (const auto& orig : m_originalMappings) {
            if (orig.Id == mapping.Id) {
                // Compare fields
                if (orig.ConnectionName == mapping.ConnectionName &&
                    orig.ExternalCode == mapping.ExternalCode &&
                    orig.ExternalCodingScheme == mapping.ExternalCodingScheme &&
                    orig.ExternalCodeMeaning == mapping.ExternalCodeMeaning &&
                    orig.InternalViewId == mapping.InternalViewId &&
                    orig.IsActive == mapping.IsActive &&
                    orig.Notes == mapping.Notes) {
                    isModified = false;
                }
                break;
            }
        }
        if (isModified || mapping.Id < 0) {
            modified.append(mapping);
        }
    }
    return modified;
}

QVector<int> RisProcedureMappingConfigurationWidget::getDeletedMappingIds() const
{
    return m_deletedIds;
}

void RisProcedureMappingConfigurationWidget::onConnectionFilterChanged(int index)
{
    Q_UNUSED(index);
    QString filter = ui->connectionComboBox->currentData().toString();
    populateList(filter);
}

void RisProcedureMappingConfigurationWidget::onMappingSelected(
    const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);

    // Save previous mapping if modified
    if (m_currentMappingIndex >= 0) {
        saveMappingFromForm();
    }

    if (!current.isValid()) {
        clearDetails();
        return;
    }

    int mappingId = current.data(Qt::UserRole + 1).toInt();
    int idx = findMappingIndex(mappingId);
    showMappingDetails(idx);
}

void RisProcedureMappingConfigurationWidget::onAddClicked()
{
    ent::RisProcedureMapping newMapping;
    newMapping.Id = -1; // Mark as new
    newMapping.ConnectionName = ui->connectionComboBox->currentData().toString();
    if (newMapping.ConnectionName.isEmpty() && !m_connectionNames.isEmpty()) {
        newMapping.ConnectionName = m_connectionNames.first();
    }
    newMapping.IsActive = true;
    newMapping.CreatedAt = QDateTime::currentDateTime();
    newMapping.UpdatedAt = QDateTime::currentDateTime();

    m_mappings.append(newMapping);

    // Refresh list and select new item
    QString filter = ui->connectionComboBox->currentData().toString();
    populateList(filter);

    // Select the new item
    if (m_listModel->rowCount() > 0) {
        ui->listView->setCurrentIndex(m_listModel->index(m_listModel->rowCount() - 1, 0));
    }

    emit mappingAdded(newMapping);
}

void RisProcedureMappingConfigurationWidget::onDeleteClicked()
{
    if (m_currentMappingIndex < 0 || m_currentMappingIndex >= m_mappings.size()) {
        return;
    }

    auto result = QMessageBox::question(this, tr("Confirm Delete"),
        tr("Are you sure you want to delete this mapping?"),
        QMessageBox::Yes | QMessageBox::No);

    if (result != QMessageBox::Yes) {
        return;
    }

    int deletedId = m_mappings[m_currentMappingIndex].Id;
    if (deletedId >= 0) {
        m_deletedIds.append(deletedId);
    }

    m_mappings.removeAt(m_currentMappingIndex);
    m_currentMappingIndex = -1;

    QString filter = ui->connectionComboBox->currentData().toString();
    populateList(filter);
    clearDetails();

    if (deletedId >= 0) {
        emit mappingDeleted(deletedId);
    }
}

void RisProcedureMappingConfigurationWidget::onSaveClicked()
{
    if (m_currentMappingIndex < 0) {
        return;
    }

    saveMappingFromForm();
    emit mappingUpdated(m_mappings[m_currentMappingIndex]);

    // Refresh list to show updated data
    QString filter = ui->connectionComboBox->currentData().toString();
    int currentId = m_mappings[m_currentMappingIndex].Id;
    populateList(filter);

    // Re-select the item
    for (int i = 0; i < m_listModel->rowCount(); ++i) {
        if (m_listModel->item(i)->data(Qt::UserRole + 1).toInt() == currentId) {
            ui->listView->setCurrentIndex(m_listModel->index(i, 0));
            break;
        }
    }
}

void RisProcedureMappingConfigurationWidget::populateList(const QString& connectionFilter)
{
    m_listModel->clear();

    for (int i = 0; i < m_mappings.size(); ++i) {
        const auto& mapping = m_mappings[i];

        // Apply filter
        if (!connectionFilter.isEmpty() && mapping.ConnectionName != connectionFilter) {
            continue;
        }

        QString displayText = mapping.ExternalCode;
        if (!mapping.ExternalCodeMeaning.isEmpty()) {
            displayText += QStringLiteral(" (%1)").arg(mapping.ExternalCodeMeaning);
        }

        auto* item = new QStandardItem(displayText);
        item->setData(mapping.Id, Qt::UserRole + 1);
        item->setData(i, Qt::UserRole + 2); // Store index for direct access
        item->setEditable(false);

        if (!mapping.IsActive) {
            item->setForeground(Qt::gray);
        }

        m_listModel->appendRow(item);
    }
}

void RisProcedureMappingConfigurationWidget::showMappingDetails(int index)
{
    if (index < 0 || index >= m_mappings.size()) {
        clearDetails();
        return;
    }

    m_currentMappingIndex = index;
    const auto& mapping = m_mappings[index];

    ui->connectionNameEdit->setText(mapping.ConnectionName);
    ui->externalCodeEdit->setText(mapping.ExternalCode);
    ui->codingSchemeEdit->setText(mapping.ExternalCodingScheme);
    ui->codeMeaningEdit->setText(mapping.ExternalCodeMeaning);
    ui->internalViewIdSpinBox->setValue(mapping.InternalViewId);
    ui->isActiveCheckBox->setChecked(mapping.IsActive);
    ui->notesEdit->setPlainText(mapping.Notes);

    // Enable editing
    ui->detailsGroupBox->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->saveButton->setEnabled(true);
}

void RisProcedureMappingConfigurationWidget::clearDetails()
{
    m_currentMappingIndex = -1;

    ui->connectionNameEdit->clear();
    ui->externalCodeEdit->clear();
    ui->codingSchemeEdit->clear();
    ui->codeMeaningEdit->clear();
    ui->internalViewIdSpinBox->setValue(-1);
    ui->isActiveCheckBox->setChecked(true);
    ui->notesEdit->clear();

    ui->detailsGroupBox->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
}

void RisProcedureMappingConfigurationWidget::saveMappingFromForm()
{
    if (m_currentMappingIndex < 0 || m_currentMappingIndex >= m_mappings.size()) {
        return;
    }

    auto& mapping = m_mappings[m_currentMappingIndex];
    mapping.ConnectionName = ui->connectionNameEdit->text();
    mapping.ExternalCode = ui->externalCodeEdit->text();
    mapping.ExternalCodingScheme = ui->codingSchemeEdit->text();
    mapping.ExternalCodeMeaning = ui->codeMeaningEdit->text();
    mapping.InternalViewId = ui->internalViewIdSpinBox->value();
    mapping.IsActive = ui->isActiveCheckBox->isChecked();
    mapping.Notes = ui->notesEdit->toPlainText();
    mapping.UpdatedAt = QDateTime::currentDateTime();
}

int RisProcedureMappingConfigurationWidget::findMappingIndex(int id) const
{
    for (int i = 0; i < m_mappings.size(); ++i) {
        if (m_mappings[i].Id == id) {
            return i;
        }
    }
    return -1;
}
