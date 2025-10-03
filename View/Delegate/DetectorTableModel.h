#pragma once
#include <QAbstractTableModel>
#include <QVector>
#include <QDateTime>
#include "Detector.h"

namespace Etrek::Device::Data::Entity {

    class DetectorTableModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        explicit DetectorTableModel(QObject* parent = nullptr);

        enum Column {
            COL_Manufacturer = 0,
            COL_ModelName,
            COL_Resolution,
            COL_Size,
            COL_PixelWidth,
            COL_PixelHeight,
            COL_Sensitivity,
            COL_ImagerPixelSpacing,
            COL_Identifier,
            COL_SpaceLUTReference,
            COL_SpatialResolution,
            COL_ScanOptions,
            COL_Width,
            COL_Height,
            COL_WidthShift,
            COL_HeightShift,
            COL_SaturationValue,
            COL_COUNT // Updated to exclude COL_Id
        };

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role);
        void setDataSource(QVector<Detector>* data);
        const QVector<Detector>* dataSource() const;

    private:
        QVector<Detector>* m_data = nullptr; // shared with widget
    };

}
