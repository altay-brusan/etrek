#pragma once

#ifndef TUBETABLEMODEL_H
#define TUBETABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QDate>
#include <QDateTime>
#include "XRayTube.h"                  
#include "DevicePosition.h"

using namespace Etrek::Device::Data::Entity;
using namespace Etrek::Device;

class TubeTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Columns {
        ColManufacturer = 0,  // display only
        ColModel,
        ColPartNumber,
        ColSerial,
        ColTypeNumber,
        ColFocalSpot,
        ColAnodeHeatCapacity, // spin
        ColCoolingRate,       // spin
        ColMaxVoltage,        // spin
        ColMaxCurrent,        // spin
        ColPowerRange,
        ColTubeName,
        ColPosition,          // combo (nullable)
        ColTubeFilter,
        ColMfgDate,           // date delegate
        ColInstDate,          // date delegate
        ColCalibDate,         // date delegate
        ColTubeOrder,         // VIRTUAL: "", "Tube 1", "Tube 2"
        ColCount
    };

    explicit TubeTableModel(QObject* parent = nullptr);

    void setDataSource(const QVector<XRayTube>& tubes);
    const QVector<XRayTube>& rows() const { return m_rows; }

    // TubeOrder helpers
    int tubeOrder(int row) const;            // 0 = none, 1 = Tube 1, 2 = Tube 2
    bool setTubeOrder(int row, int v);       // enforces uniqueness

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant headerData(int section, Qt::Orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& idx) const override;
    QVariant data(const QModelIndex& idx, int role) const override;
    bool setData(const QModelIndex& idx, const QVariant& value, int role) override;

private:
    // virtual “TubeOrder” column state (per row)
    // values: 0 (none), 1 (Tube 1), 2 (Tube 2)
    QVector<int> m_tubeOrder;

    // nullable Position support without changing the entity:
    // if false => show empty for Position and treat as NULL
    QVector<bool> m_hasPosition;

    QVector<XRayTube> m_rows;

    // helpers
    static QString devicePositionToDisplay(std::optional<Etrek::Device::DevicePosition> pos);
    static std::optional<Etrek::Device::DevicePosition> parseDevicePositionFromDisplay(const QString& s);

    bool isTubeOrderTaken(int v, int exceptRow = -1) const; // v in {1,2}
};


#endif // !TUBETABLEMODEL_H


