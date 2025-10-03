// PositionComboDelegate.cpp
#include "PositionComboDelegate.h"
#include <QComboBox>

QWidget* PositionComboDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const {
    auto* cb = new QComboBox(parent);
    cb->setEditable(false);
    cb->addItem("");          // (none) => NULL
    cb->addItem("Stand");
    cb->addItem("Table");
    cb->addItem("Motorized");
    return cb;
}

void PositionComboDelegate::setEditorData(QWidget* editor, const QModelIndex& idx) const {
    auto* cb = qobject_cast<QComboBox*>(editor);
    if (!cb) return;

    const QString display = idx.model()->data(idx, Qt::DisplayRole).toString();
    int to = cb->findText(display, Qt::MatchFixedString);
    if (to < 0) to = 0; // default to blank
    cb->setCurrentIndex(to);
}

void PositionComboDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& idx) const {
    auto* cb = qobject_cast<QComboBox*>(editor);
    if (!cb) return;
    model->setData(idx, cb->currentText(), Qt::EditRole);
}