#include "DetectorTableModel.h"

using namespace Etrek::Device::Data::Entity;

DetectorTableModel::DetectorTableModel(QObject* parent)
    : QAbstractTableModel(parent) {
}

int DetectorTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid() || !m_data) return 0;
    return m_data->size();
}

int DetectorTableModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return COL_COUNT - 1; // Exclude COL_Id from the column count
}

QVariant DetectorTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) return {};

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case COL_Manufacturer:       return "Manufacturer";
        case COL_ModelName:          return "Model";
        case COL_Resolution:         return "Resolution";
        case COL_Size:               return "Size";
        case COL_PixelWidth:         return "Pixel Width (mm)";
        case COL_PixelHeight:        return "Pixel Height (mm)";
        case COL_Sensitivity:        return "Sensitivity";
        case COL_ImagerPixelSpacing: return "Imager Pixel Spacing";
        case COL_Identifier:         return "Identifier";
        case COL_SpaceLUTReference:  return "Space LUT Ref";
        case COL_SpatialResolution:  return "Spatial Res (mm)";
        case COL_ScanOptions:        return "Scan Options";
        case COL_Width:              return "Width (px)";
        case COL_Height:             return "Height (px)";
        case COL_WidthShift:         return "Width Shift";
        case COL_HeightShift:        return "Height Shift";
        case COL_SaturationValue:    return "Saturation";
        default:                     return {};
        }
    }
    return section + 1;
}

Qt::ItemFlags DetectorTableModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    const int col = index.column();

    // Make Manufacturer and other columns read-only except the editable ones
    bool editable =
        (col != COL_Manufacturer);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled |
        (editable ? Qt::ItemIsEditable : Qt::NoItemFlags);
}

QVariant DetectorTableModel::data(const QModelIndex& index, int role) const {
    if (!m_data || !index.isValid()) return {};
    const auto& d = m_data->at(index.row());
    if (role != Qt::DisplayRole && role != Qt::EditRole) return {};

    switch (index.column()) {
    case COL_Manufacturer:       return d.Manufacturer;
    case COL_ModelName:          return d.ModelName;
    case COL_Resolution:         return d.Resolution;
    case COL_Size:               return d.Size;
    case COL_PixelWidth:         return d.PixelWidth;
    case COL_PixelHeight:        return d.PixelHeight;
    case COL_Sensitivity:        return d.Sensitivity;
    case COL_ImagerPixelSpacing: return d.ImagerPixelSpacing;
    case COL_Identifier:         return d.Identifier;
    case COL_SpaceLUTReference:  return d.SpaceLUTReference;
    case COL_SpatialResolution:  return d.SpatialResolution;
    case COL_ScanOptions:        return d.ScanOptions;
    case COL_Width:              return d.Width;
    case COL_Height:             return d.Height;
    case COL_WidthShift:         return d.WidthShift;
    case COL_HeightShift:        return d.HeightShift;
    case COL_SaturationValue:    return d.SaturationValue;
    default:                     return {};
    }
}

bool DetectorTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!m_data || !index.isValid() || role != Qt::EditRole) return false;
    auto& d = (*m_data)[index.row()];
    bool ok = true;

    switch (index.column()) {
    case COL_Manufacturer:       d.Manufacturer = value.toString(); break;
    case COL_ModelName:          d.ModelName = value.toString(); break;
    case COL_Resolution:         d.Resolution = value.toString(); break;
    case COL_Size:               d.Size = value.toString(); break;
    case COL_PixelWidth:         d.PixelWidth = value.toDouble(&ok); break;
    case COL_PixelHeight:        d.PixelHeight = value.toDouble(&ok); break;
    case COL_Sensitivity:        d.Sensitivity = value.toDouble(&ok); break;
    case COL_ImagerPixelSpacing: d.ImagerPixelSpacing = value.toString(); break;
    case COL_Identifier:         d.Identifier = value.toString(); break;
    case COL_SpaceLUTReference:  d.SpaceLUTReference = value.toString(); break;
    case COL_SpatialResolution:  d.SpatialResolution = value.toDouble(&ok); break;
    case COL_ScanOptions:        d.ScanOptions = value.toString(); break;
    case COL_Width:              d.Width = value.toInt(&ok); break;
    case COL_Height:             d.Height = value.toInt(&ok); break;
    case COL_WidthShift:         d.WidthShift = value.toInt(&ok); break;
    case COL_HeightShift:        d.HeightShift = value.toInt(&ok); break;
    case COL_SaturationValue:    d.SaturationValue = value.toInt(&ok); break;
    default: ok = false; break;
    }

    if (!ok) return false;

    // Update the UpdateDate column automatically if needed
    if (index.column() != COL_Manufacturer) {
        d.UpdateDate = QDateTime::currentDateTime();
        const int updCol = COL_HeightShift; // Or any column to trigger the update
        emit dataChanged(this->index(index.row(), updCol),
            this->index(index.row(), updCol),
            { Qt::DisplayRole, Qt::EditRole });
    }

    emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });
    return true;
}

void DetectorTableModel::setDataSource(QVector<Detector>* data) {
    beginResetModel();
    m_data = data;
    endResetModel();
}

const QVector<Detector>* DetectorTableModel::dataSource() const {
    return m_data;
}
