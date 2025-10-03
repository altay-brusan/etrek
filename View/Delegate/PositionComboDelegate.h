// PositionComboDelegate.h
#ifndef POSITIONCOMBODELEGATE_H
#define POSITIONCOMBODELEGATE_H

#include <QStyledItemDelegate>

class PositionComboDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;
    void setEditorData(QWidget* editor, const QModelIndex& idx) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& idx) const override;
};
#endif // POSITIONCOMBODELEGATE_H

