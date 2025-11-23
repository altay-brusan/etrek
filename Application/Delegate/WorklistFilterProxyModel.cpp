#include "WorklistFilterProxyModel.h"
#include <QDateTime>

namespace Etrek::Application::Delegate {

WorklistFilterProxyModel::WorklistFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    // Set default filter behavior
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void WorklistFilterProxyModel::setDateRangeFilter(const QDate& fromDate, const QDate& toDate)
{
    m_fromDate = fromDate;
    m_toDate = toDate;
    invalidateFilter();
}

void WorklistFilterProxyModel::setSourceFilter(const QString& source)
{
    m_sourceFilter = source.trimmed();
    invalidateFilter();
}

void WorklistFilterProxyModel::clearFilters()
{
    m_fromDate = QDate();
    m_toDate = QDate();
    m_sourceFilter.clear();
    invalidateFilter();
}

void WorklistFilterProxyModel::setSearchName(const QString& name)
{
    m_searchName = name.trimmed();
    invalidateFilter();
}

void WorklistFilterProxyModel::setSearchPatientId(const QString& patientId)
{
    m_searchPatientId = patientId.trimmed();
    invalidateFilter();
}

void WorklistFilterProxyModel::setSearchStudyName(const QString& studyName)
{
    m_searchStudyName = studyName.trimmed();
    invalidateFilter();
}

void WorklistFilterProxyModel::setSearchBirthDate(const QDate& birthDate)
{
    m_searchBirthDate = birthDate;
    invalidateFilter();
}

void WorklistFilterProxyModel::setSearchAccessionNo(const QString& accessionNo)
{
    m_searchAccessionNo = accessionNo.trimmed();
    invalidateFilter();
}

void WorklistFilterProxyModel::clearSearch()
{
    m_searchName.clear();
    m_searchPatientId.clear();
    m_searchStudyName.clear();
    m_searchBirthDate = QDate();
    m_searchAccessionNo.clear();
    invalidateFilter();
}

bool WorklistFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    // Get the source model
    QAbstractItemModel* model = sourceModel();
    if (!model)
        return true;

    // ===== FILTER CHECKS =====

    // Check source filter (column 8)
    if (!m_sourceFilter.isEmpty()) {
        QModelIndex sourceIndex = model->index(sourceRow, SourceColumn, sourceParent);
        QString sourceValue = model->data(sourceIndex).toString();

        // Case-insensitive comparison
        if (sourceValue.compare(m_sourceFilter, Qt::CaseInsensitive) != 0)
            return false;
    }

    // Check date range filter (column 9 - "Created At" in format "yyyy-MM-dd HH:mm")
    if (m_fromDate.isValid() || m_toDate.isValid()) {
        QModelIndex dateIndex = model->index(sourceRow, CreatedAtColumn, sourceParent);
        QString dateStr = model->data(dateIndex).toString();

        // Parse the date from the "yyyy-MM-dd HH:mm" format
        QDateTime dateTime = QDateTime::fromString(dateStr, "yyyy-MM-dd HH:mm");
        if (!dateTime.isValid()) {
            // Try parsing as date only
            QDate date = QDate::fromString(dateStr.left(10), "yyyy-MM-dd");
            if (date.isValid())
                dateTime = QDateTime(date, QTime(0, 0));
        }

        if (dateTime.isValid()) {
            QDate rowDate = dateTime.date();

            // Check lower bound (from date)
            if (m_fromDate.isValid() && rowDate < m_fromDate)
                return false;

            // Check upper bound (to date)
            if (m_toDate.isValid() && rowDate > m_toDate)
                return false;
        }
    }

    // ===== SEARCH CHECKS (applied on top of filters) =====

    // Check name search (column 0 - Patient Name) - contains match
    if (!m_searchName.isEmpty()) {
        QModelIndex nameIndex = model->index(sourceRow, PatientNameColumn, sourceParent);
        QString nameValue = model->data(nameIndex).toString();
        if (!nameValue.contains(m_searchName, Qt::CaseInsensitive))
            return false;
    }

    // Check patient ID search (column 1) - contains match
    if (!m_searchPatientId.isEmpty()) {
        QModelIndex patientIdIndex = model->index(sourceRow, PatientIdColumn, sourceParent);
        QString patientIdValue = model->data(patientIdIndex).toString();
        if (!patientIdValue.contains(m_searchPatientId, Qt::CaseInsensitive))
            return false;
    }

    // Check study name search (column 2) - contains match
    if (!m_searchStudyName.isEmpty()) {
        QModelIndex studyNameIndex = model->index(sourceRow, StudyNameColumn, sourceParent);
        QString studyNameValue = model->data(studyNameIndex).toString();
        if (!studyNameValue.contains(m_searchStudyName, Qt::CaseInsensitive))
            return false;
    }

    // Check birth date search (column 4) - exact date match
    if (m_searchBirthDate.isValid()) {
        QModelIndex birthDateIndex = model->index(sourceRow, BirthDateColumn, sourceParent);
        QString birthDateStr = model->data(birthDateIndex).toString();

        // Parse the date from "yyyy-MM-dd" format
        QDate rowBirthDate = QDate::fromString(birthDateStr, "yyyy-MM-dd");
        if (!rowBirthDate.isValid() || rowBirthDate != m_searchBirthDate)
            return false;
    }

    // Check accession number search (column 5) - contains match
    if (!m_searchAccessionNo.isEmpty()) {
        QModelIndex accessionNoIndex = model->index(sourceRow, AccessionNoColumn, sourceParent);
        QString accessionNoValue = model->data(accessionNoIndex).toString();
        if (!accessionNoValue.contains(m_searchAccessionNo, Qt::CaseInsensitive))
            return false;
    }

    return true;
}

} // namespace Etrek::Application::Delegate
