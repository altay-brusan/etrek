#ifndef GENERATORTABLEMODEL_H
#define GENERATORTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QBrush>
#include <QColor>
#include <QPalette>
#include <QVariant>
#include "Generator.h"

using namespace Etrek::Device::Data::Entity;

class GeneratorTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns {
        ColManufacturer = 0,     // not editable
        ColIsActive,             // not editable (checkbox display)
        ColModel,                // editable
        ColPartNumber,           // editable
        ColSerial,               // editable
        ColTypeNumber,           // editable
        ColTechSpecs,            // editable (long text ok)
        ColOutput1,              // combobox: 1/2, editable only if IsOutput1Active
        ColOutput1Active,        // checkbox
        ColOutput2,              // combobox: 1/2, editable only if IsOutput2Active
        ColOutput2Active,        // checkbox
        ColMfgDate,              // date delegate
        ColInstDate,             // date delegate
        ColCalibDate,            // date delegate
        ColCount
    };

    explicit GeneratorTableModel(QObject* parent = nullptr);

    void setDataSource(const QVector<Generator>& gens);
    const QVector<Generator>& rows() const;

    // QAbstractTableModel overrides
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant headerData(int section, Qt::Orientation orient, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& idx) const override;
    QVariant data(const QModelIndex& idx, int role) const override;
    bool setData(const QModelIndex& idx, const QVariant& value, int role) override;

private:
    QVector<Generator> m_rows;

    static QString tubeName(int v);
    static QString fmtDate(const QDate& d) { return d.isValid() ? d.toString("yyyy-MM-dd") : QString(); }

    void deactivateAllExcept(int keepRow);
    bool isSoleActive(int row, bool isOutput1) const;
};

#endif //GENERATORTABLEMODEL_H

