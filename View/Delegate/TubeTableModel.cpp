#include "TubeTableModel.h"
#include <QBrush>
#include <QApplication>
#include <QPalette>
#include <optional>


TubeTableModel::TubeTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void TubeTableModel::setDataSource(const QVector<XRayTube>& tubes)
{
    beginResetModel();
    m_rows = tubes;
    m_tubeOrder.resize(m_rows.size());
    m_tubeOrder.fill(0);

    m_hasPosition.resize(m_rows.size());
    for (int i = 0; i < m_rows.size(); ++i) {
        // default: we assume entity value exists; mark as present
        // if you want to start as NULL, set m_hasPosition[i] = false;
        m_hasPosition[i] = true;
    }
    endResetModel();
}

int TubeTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_rows.size();
}

int TubeTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColCount;
}

QVariant TubeTableModel::headerData(int section, Qt::Orientation orient, int role) const
{
    if (orient != Qt::Horizontal || role != Qt::DisplayRole) return {};

    switch (section) {
    case ColManufacturer:      return "Manufacturer";
    case ColModel:             return "Model Number";
    case ColPartNumber:        return "Part Number";
    case ColSerial:            return "Serial Number";
    case ColTypeNumber:        return "Type Number";
    case ColFocalSpot:         return "Focal Spot";
    case ColAnodeHeatCapacity: return "Anode Heat Capacity (HU)";
    case ColCoolingRate:       return "Cooling Rate (HU/min)";
    case ColMaxVoltage:        return "Max Voltage (kV)";
    case ColMaxCurrent:        return "Max Current (mA)";
    case ColPowerRange:        return "Power Range (kW)";
    case ColTubeName:          return "Tube Name";
    case ColPosition:          return "Position";
    case ColTubeFilter:        return "Tube Filter";
    case ColMfgDate:           return "Manufacture Date";
    case ColInstDate:          return "Installation Date";
    case ColCalibDate:         return "Calibration Date";
    case ColTubeOrder:         return "Tube Order";
    default:                   return {};
    }
}

Qt::ItemFlags TubeTableModel::flags(const QModelIndex& idx) const
{
    if (!idx.isValid()) return Qt::NoItemFlags;
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    switch (idx.column()) {
        // display only:
    case ColManufacturer:
        return f;

        // editable text
    case ColModel:
    case ColPartNumber:
    case ColSerial:
    case ColTypeNumber:
    case ColFocalSpot:
    case ColPowerRange:
    case ColTubeName:
    case ColTubeFilter:
        return f | Qt::ItemIsEditable;

        // spinboxes (int)
    case ColAnodeHeatCapacity:
    case ColCoolingRate:
    case ColMaxVoltage:
    case ColMaxCurrent:
        return f | Qt::ItemIsEditable;

        // dates (DateDelegate)
    case ColMfgDate:
    case ColInstDate:
    case ColCalibDate:
        return f | Qt::ItemIsEditable;

        // position combo (nullable)
    case ColPosition:
        return f | Qt::ItemIsEditable;

        // virtual tube-order combo
    case ColTubeOrder:
        return f | Qt::ItemIsEditable;
    }

    return f;
}

QVariant TubeTableModel::data(const QModelIndex& idx, int role) const
{
    if (!idx.isValid() || idx.row() < 0 || idx.row() >= m_rows.size()) return {};
    const auto& t = m_rows[idx.row()];

    if (role == Qt::ToolTipRole && idx.column() == ColTubeOrder) {
        return "Select at most one Tube 1 and one Tube 2 across the list.";
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (idx.column()) {
        case ColManufacturer:      return t.Manufacturer;
        case ColModel:             return t.ModelNumber;
        case ColPartNumber:        return t.PartNumber;
        case ColSerial:            return t.SerialNumber;
        case ColTypeNumber:        return t.TypeNumber;
        case ColFocalSpot:         return t.FocalSpot;
        case ColAnodeHeatCapacity: return t.AnodeHeatCapacity;
        case ColCoolingRate:       return t.CoolingRate;
        case ColMaxVoltage:        return t.MaxVoltage;
        case ColMaxCurrent:        return t.MaxCurrent;
        case ColPowerRange:        return t.PowerRange;
        case ColTubeName:          return t.TubeName;

        case ColPosition:
            if (!m_hasPosition[idx.row()]) {
                // show empty as NULL
                return role == Qt::DisplayRole ? "" : QVariant();
            }
            else {
                // display friendly string
                return devicePositionToDisplay(t.Position);
            }

        case ColTubeFilter:        return t.TubeFilter;

        case ColMfgDate:
            return (role == Qt::DisplayRole) ? t.ManufactureDate.toString("yyyy-MM-dd")
                : QVariant::fromValue(t.ManufactureDate);
        case ColInstDate:
            return (role == Qt::DisplayRole) ? t.InstallationDate.toString("yyyy-MM-dd")
                : QVariant::fromValue(t.InstallationDate);
        case ColCalibDate:
            return (role == Qt::DisplayRole) ? t.CalibrationDate.toString("yyyy-MM-dd")
                : QVariant::fromValue(t.CalibrationDate);

        case ColTubeOrder: {
            int v = m_tubeOrder[idx.row()];
            if (role == Qt::DisplayRole) {
                if (v == 1) return "Tube 1";
                if (v == 2) return "Tube 2";
                return "";
            }
            else {
                return v; // for EditRole, provide raw int
            }
        }
        }
    }

    return {};
}

bool TubeTableModel::setData(const QModelIndex& idx, const QVariant& value, int role)
{
    if (!idx.isValid() || idx.row() < 0 || idx.row() >= m_rows.size()) return false;
    if (role != Qt::EditRole) return false;

    auto& t = m_rows[idx.row()];
    bool changed = false;

    switch (idx.column()) {
    case ColModel:             t.ModelNumber = value.toString(); changed = true; break;
    case ColPartNumber:        t.PartNumber = value.toString(); changed = true; break;
    case ColSerial:            t.SerialNumber = value.toString(); changed = true; break;
    case ColTypeNumber:        t.TypeNumber = value.toString(); changed = true; break;
    case ColFocalSpot:         t.FocalSpot = value.toString(); changed = true; break;
    case ColAnodeHeatCapacity: t.AnodeHeatCapacity = value.toInt(); changed = true; break;
    case ColCoolingRate:       t.CoolingRate = value.toInt(); changed = true; break;
    case ColMaxVoltage:        t.MaxVoltage = value.toInt(); changed = true; break;
    case ColMaxCurrent:        t.MaxCurrent = value.toInt(); changed = true; break;
    case ColPowerRange:        t.PowerRange = value.toString(); changed = true; break;
    case ColTubeName:          t.TubeName = value.toString(); changed = true; break;

    case ColPosition: {
        const QString s = value.toString();
        if (s.trimmed().isEmpty()) {
            m_hasPosition[idx.row()] = false; // NULL
        }
        else {
            m_hasPosition[idx.row()] = true;
            t.Position = ParseDevicePosition(s);
        }
        changed = true;
        break;
    }

    case ColTubeFilter:        t.TubeFilter = value.toString(); changed = true; break;

    case ColMfgDate: {
        const QDate d = value.toDate();
        if (!d.isValid()) return false;
        t.ManufactureDate = d; changed = true; break;
    }
    case ColInstDate: {
        const QDate d = value.toDate();
        if (!d.isValid()) return false;
        t.InstallationDate = d; changed = true; break;
    }
    case ColCalibDate: {
        const QDate d = value.toDate();
        if (!d.isValid()) return false;
        t.CalibrationDate = d; changed = true; break;
    }

    case ColTubeOrder: {
        const int v = value.toInt(); // 0,1,2
        if (v < 0 || v > 2) return false;
        if (!setTubeOrder(idx.row(), v)) return false;
        changed = true;
        break;
    }

    default: break;
    }

    if (changed) {
        emit dataChanged(index(idx.row(), 0), index(idx.row(), ColCount - 1));
        return true;
    }
    return false;
}

int TubeTableModel::tubeOrder(int row) const
{
    if (row < 0 || row >= m_tubeOrder.size()) return 0;
    return m_tubeOrder[row];
}

bool TubeTableModel::isTubeOrderTaken(int v, int exceptRow) const
{
    if (v == 0) return false;
    for (int r = 0; r < m_tubeOrder.size(); ++r) {
        if (r == exceptRow) continue;
        if (m_tubeOrder[r] == v) return true;
    }
    return false;
}

bool TubeTableModel::setTubeOrder(int row, int v)
{
    if (row < 0 || row >= m_tubeOrder.size()) return false;
    // Enforce: at most one Tube 1 and one Tube 2 across table.
    // If selection already taken by another row, reject.
    if (v == 1 || v == 2) {
        if (isTubeOrderTaken(v, row)) return false;
    }
    m_tubeOrder[row] = v;
    return true;
}

// ----- helpers for Position display -----

QString TubeTableModel::devicePositionToDisplay(std::optional<DevicePosition> pos)
{
    if (!pos.has_value()) return "";
    return DevicePositionToString(pos.value());
}

std::optional<DevicePosition> TubeTableModel::parseDevicePositionFromDisplay(const QString& s)
{
    if (s.trimmed().isEmpty()) return std::nullopt;
    return ParseDevicePosition(s);
}
