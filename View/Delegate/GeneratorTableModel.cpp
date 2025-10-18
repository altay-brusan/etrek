#include "GeneratorTableModel.h"
#include "Generator.h"
#include <QColor> 
#include <QVector>
#include <QApplication>


using namespace Etrek::Device::Data::Entity;

GeneratorTableModel::GeneratorTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void GeneratorTableModel::setDataSource(const QVector<Generator>& gens)
{
    beginResetModel();
    m_rows = gens;

    for (auto& g : m_rows) {
        if (g.Output1 != 1 && g.Output1 != 2) g.Output1 = 1;
        if (g.Output2 != 1 && g.Output2 != 2) g.Output2 = 2;
    }

    endResetModel();
}

const QVector<Generator>& GeneratorTableModel::rows() const
{
    return m_rows;
}

int GeneratorTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_rows.size();
}

int GeneratorTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColCount;
}

QVariant GeneratorTableModel::headerData(int section, Qt::Orientation orient, int role) const
{
    if (orient != Qt::Horizontal || role != Qt::DisplayRole) return {};

    switch (section) {
    case ColManufacturer:   return "Manufacturer";
    case ColIsActive:       return "Active";
    case ColModel:          return "Model Number";
    case ColPartNumber:     return "Part Number";
    case ColSerial:         return "Serial Number";
    case ColTypeNumber:     return "Type Number";
    case ColTechSpecs:      return "Technical Specifications";
    case ColOutput1:        return "Output 1";
    case ColOutput1Active:  return "Output 1 Active";
    case ColOutput2:        return "Output 2";
    case ColOutput2Active:  return "Output 2 Active";
    case ColMfgDate:        return "Manufacture Date";
    case ColInstDate:       return "Installation Date";
    case ColCalibDate:      return "Calibration Date";
    default: return {};
    }
}

Qt::ItemFlags GeneratorTableModel::flags(const QModelIndex& idx) const {
    if (!idx.isValid()) return Qt::NoItemFlags;
    const auto& g = m_rows[idx.row()];
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    switch (idx.column()) {
    case ColManufacturer:
    case ColIsActive:
        return f; // non-editable

    case ColModel:
    case ColPartNumber:
    case ColSerial:
    case ColTypeNumber:
    case ColTechSpecs:
        return f | Qt::ItemIsEditable;

    case ColOutput1:
        if (g.IsOutput1Active) f |= Qt::ItemIsEditable;
        return f;

    case ColOutput2:
        if (g.IsOutput2Active) f |= Qt::ItemIsEditable;
        return f;

    case ColOutput1Active:
    case ColOutput2Active:
        return f | Qt::ItemIsUserCheckable;

    case ColMfgDate:
    case ColInstDate:
    case ColCalibDate:
        return f | Qt::ItemIsEditable;
    }

    return f;
}


QVariant GeneratorTableModel::data(const QModelIndex& idx, int role) const {
    if (!idx.isValid() || idx.row() < 0 || idx.row() >= m_rows.size()) return {};
    const auto& g = m_rows[idx.row()];

    // gray out output cells if inactive
    if (role == Qt::ForegroundRole) {
        if ((idx.column() == ColOutput1 && !g.IsOutput1Active) ||
            (idx.column() == ColOutput2 && !g.IsOutput2Active)) {
            return QApplication::palette().brush(QPalette::Disabled, QPalette::Text);
        }
    }
    if (role == Qt::ToolTipRole) {
        if (idx.column() == ColOutput1 && !g.IsOutput1Active)
            return "Output 1 is inactive";
        if (idx.column() == ColOutput2 && !g.IsOutput2Active)
            return "Output 2 is inactive";
    }

    switch (idx.column()) {
    case ColManufacturer:
        if (role == Qt::DisplayRole) return g.Manufacturer;
        break;

    case ColIsActive:
        if (role == Qt::DisplayRole) return g.IsActive ? "Yes" : "No";
        if (role == Qt::CheckStateRole) return g.IsActive ? Qt::Checked : Qt::Unchecked; // display as checkbox, but not editable
        break;

    case ColModel:
        if (role == Qt::DisplayRole || role == Qt::EditRole) return g.ModelNumber;
        break;

    case ColPartNumber:
        if (role == Qt::DisplayRole || role == Qt::EditRole) return g.PartNumber;
        break;

    case ColSerial:
        if (role == Qt::DisplayRole || role == Qt::EditRole) return g.SerialNumber;
        break;

    case ColTypeNumber:
        if (role == Qt::DisplayRole || role == Qt::EditRole) return g.TypeNumber;
        break;

    case ColTechSpecs:
        if (role == Qt::DisplayRole || role == Qt::EditRole) return g.TechnicalSpecifications;
        break;

    case ColOutput1:
        if (role == Qt::DisplayRole) return tubeName(g.Output1);
        if (role == Qt::EditRole)    return g.Output1;
        break;

    case ColOutput1Active:
        if (role == Qt::CheckStateRole) return g.IsOutput1Active ? Qt::Checked : Qt::Unchecked;
        break;

    case ColOutput2:
        if (role == Qt::DisplayRole) return tubeName(g.Output2);
        if (role == Qt::EditRole)    return g.Output2;
        break;

    case ColOutput2Active:
        if (role == Qt::CheckStateRole) return g.IsOutput2Active ? Qt::Checked : Qt::Unchecked;
        break;

    case ColMfgDate:
        if (role == Qt::DisplayRole) return fmtDate(g.ManufactureDate);
        if (role == Qt::EditRole)    return g.ManufactureDate;
        break;

    case ColInstDate:
        if (role == Qt::DisplayRole) return fmtDate(g.InstallationDate);
        if (role == Qt::EditRole)    return g.InstallationDate;
        break;

    case ColCalibDate:
        if (role == Qt::DisplayRole) return fmtDate(g.CalibrationDate);
        if (role == Qt::EditRole)    return g.CalibrationDate;
        break;
    }
    return {};
}

bool GeneratorTableModel::setData(const QModelIndex& idx, const QVariant& value, int role) {
    if (!idx.isValid() || idx.row() < 0 || idx.row() >= m_rows.size()) return false;
    auto& g = m_rows[idx.row()];
    bool changed = false;

    switch (idx.column()) {
    case ColModel:
        if (role == Qt::EditRole) { g.ModelNumber = value.toString(); changed = true; }
        break;
    case ColPartNumber:
        if (role == Qt::EditRole) { g.PartNumber = value.toString(); changed = true; }
        break;
    case ColSerial:
        if (role == Qt::EditRole) { g.SerialNumber = value.toString(); changed = true; }
        break;
    case ColTypeNumber:
        if (role == Qt::EditRole) { g.TypeNumber = value.toString(); changed = true; }
        break;
    case ColTechSpecs:
        if (role == Qt::EditRole) { g.TechnicalSpecifications = value.toString(); changed = true; }
        break;

    case ColOutput1:
        if (role == Qt::EditRole) {
            const int v = value.toInt();
            if (v == 1 || v == 2) {
                g.Output1 = v;
                // enforce uniqueness if Output2 active
                if (g.IsOutput2Active && g.Output2 == g.Output1)
                    g.Output2 = (g.Output1 == 1 ? 2 : 1);
                changed = true;
            }
        }
        break;

    case ColOutput2:
        if (role == Qt::EditRole) {
            const int v = value.toInt();
            if (v == 1 || v == 2) {
                if (g.IsOutput2Active && v == g.Output1)
                    return false; // cannot equal Output1 when Output2 is active
                g.Output2 = v;
                changed = true;
            }
        }
        break;

    case ColOutput1Active:
        if (role == Qt::CheckStateRole) {
            const bool want = (value.toInt() == Qt::Checked);
            if (want) {
                deactivateAllExcept(idx.row());
                g.IsOutput1Active = true;               
                changed = true;
            }
            else {
                if (isSoleActive(idx.row(), true)) return false;
                g.IsOutput1Active = false;
                changed = true;
            }
        }
        break;

    case ColOutput2Active:
        if (role == Qt::CheckStateRole) {
            const bool want = (value.toInt() == Qt::Checked);
            if (want) {
                if (g.Output2 == g.Output1)
                    g.Output2 = (g.Output1 == 1 ? 2 : 1);
                deactivateAllExcept(idx.row());
                g.IsOutput2Active = true;
                changed = true;
            }
            else {
                if (isSoleActive(idx.row(), false)) return false;
                g.IsOutput2Active = false;
                changed = true;
            }
        }
        break;

    case ColMfgDate:
    case ColInstDate:
    case ColCalibDate:
        if (role == Qt::EditRole) {
            const QDate d = value.toDate();
            if (!d.isValid()) return false;
            if (idx.column() == ColMfgDate)  g.ManufactureDate = d;
            if (idx.column() == ColInstDate) g.InstallationDate = d;
            if (idx.column() == ColCalibDate) g.CalibrationDate = d;
            changed = true;
        }
        break;

    default:
        break;
    }

    if (changed) {
        emit dataChanged(index(idx.row(), 0), index(idx.row(), ColCount - 1));
        return true;
    }
    return false;
}

QString GeneratorTableModel::tubeName(int v) {
    return (v == 2) ? QStringLiteral("Tube 2") : QStringLiteral("Tube 1");
}

void GeneratorTableModel::deactivateAllExcept(int keepRow) {
    for (int r = 0; r < m_rows.size(); ++r) {
        if (r == keepRow) continue;
        auto& other = m_rows[r];
        if (other.IsOutput1Active || other.IsOutput2Active) {
            other.IsOutput1Active = false;
            other.IsOutput2Active = false;
            emit dataChanged(index(r, ColOutput1Active), index(r, ColOutput2Active));
        }
    }
}

bool GeneratorTableModel::isSoleActive(int row, bool isOutput1) const {
    int totalActive = 0;
    for (int r = 0; r < m_rows.size(); ++r) {
        totalActive += (m_rows[r].IsOutput1Active ? 1 : 0);
        totalActive += (m_rows[r].IsOutput2Active ? 1 : 0);
    }
    if (totalActive == 1) {
        const auto& g = m_rows[row];
        return (isOutput1 ? g.IsOutput1Active : g.IsOutput2Active);
    }
    return false;
}
