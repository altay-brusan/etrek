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

bool WorklistFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    // Get the source model
    QAbstractItemModel* model = sourceModel();
    if (!model)
        return true;

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

    return true;
}

} // namespace Etrek::Application::Delegate
