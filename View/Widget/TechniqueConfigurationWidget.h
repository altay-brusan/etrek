#ifndef TECHNIQUECONFIGURATIONWIDGET_H
#define TECHNIQUECONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QHash>
#include <QStandardItemModel>
#include "TechniqueParameter.h"
#include "TechniqueConfigurationWidget.h"


namespace Ui { class TechniqueConfigurationWidget; }

using namespace Etrek::ScanProtocol::Data::Entity;

class TechniqueConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TechniqueConfigurationWidget(const QVector<TechniqueParameter>& parameters,
        QWidget* parent = nullptr);
    int currentBodyPartId() const;
    BodyPart currentBodyPart() const;
    QVector<TechniqueParameter>  collectCurrentBodyPartParameters() const;
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
    QVector<TechniqueParameter> m_parameters;
    QStandardItemModel* m_bodyPartModel = nullptr;
    QHash<int, BodyPart> m_bodyPartById;
};
#endif // TECHNIQUECONFIGURATIONWIDGET_H
