// TubeComboDelegate.h

#ifndef TUBECOMBODELEGATE_H
#define TUBECOMBODELEGATE_H
#include <QStyledItemDelegate>

class TubeComboDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TubeComboDelegate(bool isOutput2Column, QObject* parent = nullptr)
        : QStyledItemDelegate(parent), m_isOutput2(isOutput2Column) {
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& idx) const override;
    void setEditorData(QWidget* editor, const QModelIndex& idx) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& idx) const override;

private:
    bool m_isOutput2;
};
#endif // !TUBECOMBODELEGATE_H


