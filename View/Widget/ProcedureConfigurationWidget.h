#ifndef PROCEDURECONFIGURATIONWIDGET_H
#define PROCEDURECONFIGURATIONWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QVector>
#include <QComboBox>
#include "View.h"
#include "Procedure.h"
#include "BodyPart.h"
#include "AnatomicRegion.h"

namespace Ui { class ProcedureConfigurationWidget; }

class ProcedureConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProcedureConfigurationWidget(const QVector<Etrek::ScanProtocol::Data::Entity::Procedure>& procedures,
        const QVector<Etrek::ScanProtocol::Data::Entity::BodyPart>& bodyParts,
        const QVector<Etrek::ScanProtocol::Data::Entity::AnatomicRegion>& anatomicRegions,
        const QVector<Etrek::ScanProtocol::Data::Entity::View>& views,
        QWidget* parent = nullptr);
    ~ProcedureConfigurationWidget();

private:
    Ui::ProcedureConfigurationWidget* ui;

    // Data
    QVector<Etrek::ScanProtocol::Data::Entity::Procedure>     m_procedures;
    QVector<Etrek::ScanProtocol::Data::Entity::View>          m_views;
    QVector<Etrek::ScanProtocol::Data::Entity::BodyPart>      m_bodyParts;
    QVector<Etrek::ScanProtocol::Data::Entity::AnatomicRegion> m_anatomicRegions;

    // Models
    QStandardItemModel* m_procModel = nullptr; // for table view (names)
    QStandardItemModel* m_allViewsModel = nullptr; // all views
    QStandardItemModel* m_selectedViewsModel = nullptr; // views for selected procedure

    // Selection state
    int m_currentProcRow = -1;

private:
    void initStyles();
    void buildModels();
    void populateCombosOnce();                   // body parts + regions
    void populateAllViewsModel();                // uses m_views
    void populateProcedureTable();               // uses m_procedures
    void refreshDetailsForRow(int row);          // set UI fields from m_procedures[row]
    void refreshSelectedViewsForRow(int row);    // set selectedViewListView from procedure.Views
    void bindSignals();

    // Helpers
    int findComboIndexById(QComboBox* cb, int id) const;    // match stored id in UserRole
    int findViewIndexById(const QVector<Etrek::ScanProtocol::Data::Entity::View>& src, int id) const;
    int findProcedureIndexById(int id) const;
private slots:
    void onProcedureTableCurrentChanged(const QModelIndex& current, const QModelIndex& previous);
    void onAllViewsSelectionChanged();
    void onSelectedViewsSelectionChanged();
    void onAddView();
    void onRemoveView();

    // detail field change handlers (write back to m_procedures[m_currentProcRow])
    void onNameChanged(const QString& v);
    void onCodeChanged(const QString& v);
    void onCodingSchemaChanged(const QString& v);
    void onCodeMeaningChanged(const QString& v);
    void onBodyPartChanged(int idx);
    void onRegionChanged(int idx);
    void onPrintOrientationChanged(int idx);
    void onPrintFormatChanged(int idx);
    void onTrueSizeToggled(bool on);
};

#endif // PROCEDURECONFIGURATIONWIDGET_H
