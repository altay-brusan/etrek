#include "DateDelegate.h"
#include <QDateEdit>

QWidget* DateDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const {
    auto* ed = new QDateEdit(parent);
    ed->setCalendarPopup(true);
    ed->setDisplayFormat("yyyy-MM-dd");
    ed->setMinimumDate(QDate(1900, 1, 1));
    ed->setMaximumDate(QDate(2100, 12, 31));
    return ed;
}

void DateDelegate::setEditorData(QWidget* editor, const QModelIndex& idx) const {
    auto* ed = qobject_cast<QDateEdit*>(editor);
    if (!ed) return;
    ed->setDate(idx.model()->data(idx, Qt::EditRole).toDate());
}

void DateDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& idx) const {
    auto* ed = qobject_cast<QDateEdit*>(editor);
    if (!ed) return;
    model->setData(idx, ed->date(), Qt::EditRole);
}
