#ifndef TECHNIQUECONFIGURATIONWIDGET_H
#define TECHNIQUECONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QHash>
#include <QStandardItemModel>
#include "TechniqueParameter.h"
#include "TechniqueConfigurationWidget.h"


namespace Ui { class TechniqueConfigurationWidget; }

class TechniqueConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TechniqueConfigurationWidget(const QVector<Etrek::ScanProtocol::Data::Entity::TechniqueParameter>& parameters,
        QWidget* parent = nullptr);
    int currentBodyPartId() const;
    Etrek::ScanProtocol::Data::Entity::BodyPart currentBodyPart() const;
    QVector<Etrek::ScanProtocol::Data::Entity::TechniqueParameter>  collectCurrentBodyPartParameters() const;
    ~TechniqueConfigurationWidget();

signals:
    void bodyPartSelected(int bodyPartId);

private slots:
    void onBodyPartSelectionChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    void buildBodyPartList();
    void initStyles();
    enum Roles { BodyPartIdRole = Qt::UserRole + 1 };

    Ui::TechniqueConfigurationWidget* ui;
    QVector<Etrek::ScanProtocol::Data::Entity::TechniqueParameter> m_parameters;
    QStandardItemModel* m_bodyPartModel = nullptr;
    QHash<int, Etrek::ScanProtocol::Data::Entity::BodyPart> m_bodyPartById;
};
#endif // TECHNIQUECONFIGURATIONWIDGET_H
