#ifndef TUBEORDERDELEGATE_H
#define TUBEORDERDELEGATE_H

#include <QStyledItemDelegate>

class TubeOrderDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& idx) const override;
    void setEditorData(QWidget* editor, const QModelIndex& idx) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& idx) const override;
};

#endif // !TUBEORDERDELEGATE_H

