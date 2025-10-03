#ifndef INTSPINDELEGATE_H
#define INTSPINDELEGATE_H

#include <QStyledItemDelegate>

class IntSpinDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit IntSpinDelegate(int minVal, int maxVal, int step = 1, QObject* parent = nullptr)
        : QStyledItemDelegate(parent), m_min(minVal), m_max(maxVal), m_step(step) {
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;
    void setEditorData(QWidget* editor, const QModelIndex& idx) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& idx) const override;

private:
    int m_min, m_max, m_step;
};

#endif // !INTSPINDELEGATE_H


