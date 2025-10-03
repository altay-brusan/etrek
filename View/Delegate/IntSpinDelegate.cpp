#include "IntSpinDelegate.h"
#include <QSpinBox>

QWidget* IntSpinDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const {
    auto* sb = new QSpinBox(parent);
    sb->setRange(m_min, m_max);
    sb->setSingleStep(m_step);
    return sb;
}

void IntSpinDelegate::setEditorData(QWidget* editor, const QModelIndex& idx) const {
    auto* sb = qobject_cast<QSpinBox*>(editor);
    if (!sb) return;
    sb->setValue(idx.model()->data(idx, Qt::EditRole).toInt());
}

void IntSpinDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& idx) const {
    auto* sb = qobject_cast<QSpinBox*>(editor);
    if (!sb) return;
    model->setData(idx, sb->value(), Qt::EditRole);
}