#ifndef WORKLISTFILTERPROXYMODEL_H
#define WORKLISTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDate>

namespace Etrek::Application::Delegate {

/**
 * @brief Custom proxy model for filtering worklist data by date range and source.
 *
 * Filters are applied in-memory on the already-loaded table data.
 * - Date filtering uses the "Created At" column (column 9)
 * - Source filtering uses the "Source" column (column 8)
 */
class WorklistFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit WorklistFilterProxyModel(QObject* parent = nullptr);

    // Column indices for filtering
    static constexpr int SourceColumn = 8;
    static constexpr int CreatedAtColumn = 9;

    /**
     * @brief Set the date range filter.
     * @param fromDate Start date (inclusive). If invalid, no lower bound.
     * @param toDate End date (inclusive). If invalid, no upper bound.
     */
    void setDateRangeFilter(const QDate& fromDate, const QDate& toDate);

    /**
     * @brief Set the source filter.
     * @param source Source string to match ("LOCAL", "RIS"). Empty string means no filter.
     */
    void setSourceFilter(const QString& source);

    /**
     * @brief Clear all filters and reset to defaults.
     */
    void clearFilters();

    // Getters for current filter state
    QDate fromDate() const { return m_fromDate; }
    QDate toDate() const { return m_toDate; }
    QString sourceFilter() const { return m_sourceFilter; }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QDate m_fromDate;
    QDate m_toDate;
    QString m_sourceFilter;
};

} // namespace Etrek::Application::Delegate

#endif // WORKLISTFILTERPROXYMODEL_H
