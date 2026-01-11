#ifndef RISPROCEDUREMAPPINGCONFIGURATIONWIDGET_H
#define RISPROCEDUREMAPPINGCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include "RisProcedureMapping.h"

namespace Ui {
class RisProcedureMappingConfigurationWidget;
}

class QStandardItemModel;

class RisProcedureMappingConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RisProcedureMappingConfigurationWidget(
        const QVector<Etrek::Worklist::Data::Entity::RisProcedureMapping>& mappings,
        const QStringList& connectionNames,
        QWidget* parent = nullptr);
    ~RisProcedureMappingConfigurationWidget();

    QVector<Etrek::Worklist::Data::Entity::RisProcedureMapping> getMappings() const;
    QVector<Etrek::Worklist::Data::Entity::RisProcedureMapping> getModifiedMappings() const;
    QVector<int> getDeletedMappingIds() const;

signals:
    void mappingAdded(const Etrek::Worklist::Data::Entity::RisProcedureMapping& mapping);
    void mappingUpdated(const Etrek::Worklist::Data::Entity::RisProcedureMapping& mapping);
    void mappingDeleted(int mappingId);

private slots:
    void onConnectionFilterChanged(int index);
    void onMappingSelected(const QModelIndex& current, const QModelIndex& previous);
    void onAddClicked();
    void onDeleteClicked();
    void onSaveClicked();

private:
    Ui::RisProcedureMappingConfigurationWidget* ui;
    QVector<Etrek::Worklist::Data::Entity::RisProcedureMapping> m_mappings;
    QVector<Etrek::Worklist::Data::Entity::RisProcedureMapping> m_originalMappings;
    QVector<int> m_deletedIds;
    QStringList m_connectionNames;
    QStandardItemModel* m_listModel = nullptr;
    int m_currentMappingIndex = -1;

    void populateList(const QString& connectionFilter = QString());
    void showMappingDetails(int index);
    void clearDetails();
    void saveMappingFromForm();
    int findMappingIndex(int id) const;
};

#endif // RISPROCEDUREMAPPINGCONFIGURATIONWIDGET_H
