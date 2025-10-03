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
using namespace Etrek::ScanProtocol;
using namespace Etrek::ScanProtocol::Data::Entity;

namespace Ui { class ProcedureConfigurationWidget; }

class ProcedureConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProcedureConfigurationWidget(const QVector<Procedure>& procedures,
        const QVector<BodyPart>& bodyParts,
        const QVector<AnatomicRegion>& anatomicRegions,
        const QVector<View>& views,
        QWidget* parent = nullptr);
    ~ProcedureConfigurationWidget();

private:
    Ui::ProcedureConfigurationWidget* ui;

    // Data
    QVector<Procedure>     m_procedures;
    QVector<View>          m_views;
    QVector<BodyPart>      m_bodyParts;
    QVector<AnatomicRegion> m_anatomicRegions;

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
    int findViewIndexById(const QVector<View>& src, int id) const;
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
