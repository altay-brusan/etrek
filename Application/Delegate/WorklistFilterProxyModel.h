#ifndef WORKLISTFILTERPROXYMODEL_H
#define WORKLISTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDate>

namespace Etrek::Application::Delegate {

/**
 * @brief Custom proxy model for filtering and searching worklist data.
 *
 * Filters and search are applied in-memory on the already-loaded table data.
 * - Date filtering uses the "Created At" column (column 9)
 * - Source filtering uses the "Source" column (column 8)
 * - Search operates on: Name (0), Patient ID (1), Study Name (2), Birth Date (4), Accession No (5)
 *
 * Search is applied ON TOP of filters - rows must match both filters AND search criteria.
 */
class WorklistFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit WorklistFilterProxyModel(QObject* parent = nullptr);

    // Column indices for filtering and searching
    static constexpr int PatientNameColumn = 0;
    static constexpr int PatientIdColumn = 1;
    static constexpr int StudyNameColumn = 2;
    static constexpr int BirthDateColumn = 4;
    static constexpr int AccessionNoColumn = 5;
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
     * @brief Clear all filters and reset to defaults (does NOT clear search).
     */
    void clearFilters();

    // Search criteria setters
    void setSearchName(const QString& name);
    void setSearchPatientId(const QString& patientId);
    void setSearchStudyName(const QString& studyName);
    void setSearchBirthDate(const QDate& birthDate);
    void setSearchAccessionNo(const QString& accessionNo);

    /**
     * @brief Clear all search criteria (does NOT clear filters).
     */
    void clearSearch();

    // Getters for current filter state
    QDate fromDate() const { return m_fromDate; }
    QDate toDate() const { return m_toDate; }
    QString sourceFilter() const { return m_sourceFilter; }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    // Filter state
    QDate m_fromDate;
    QDate m_toDate;
    QString m_sourceFilter;

    // Search state
    QString m_searchName;
    QString m_searchPatientId;
    QString m_searchStudyName;
    QDate m_searchBirthDate;
    QString m_searchAccessionNo;
};

} // namespace Etrek::Application::Delegate

#endif // WORKLISTFILTERPROXYMODEL_H
