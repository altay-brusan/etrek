#include "TubeOrderDelegate.h"
#include <QComboBox>
#include <QAbstractItemModel>

QWidget* TubeOrderDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& idx) const {
    auto* cb = new QComboBox(parent);
    cb->setEditable(false);
    // items will be filled in setEditorData based on global state
    return cb;
}

void TubeOrderDelegate::setEditorData(QWidget* editor, const QModelIndex& idx) const {
    auto* cb = qobject_cast<QComboBox*>(editor);
    if (!cb) return;

    // compute which options are globally available
    // model must expose ColTubeOrder as EditRole int per row; we can scan rows
    const int rows = idx.model()->rowCount();
    bool tube1Taken = false;
    bool tube2Taken = false;
    const int row = idx.row();

    for (int r = 0; r < rows; ++r) {
        const int v = idx.model()->index(r, idx.column()).data(Qt::EditRole).toInt(); // 0/1/2
        if (r == row) continue; // allow keeping current choice
        if (v == 1) tube1Taken = true;
        if (v == 2) tube2Taken = true;
    }

    cb->clear();
    cb->addItem("", 0);

    // current value
    int current = idx.model()->data(idx, Qt::EditRole).toInt();

    // If current row already holds a tube, allow that option to stay selectable
    // even if "taken" (so user can keep it or clear it).
    if (!tube1Taken || current == 1) cb->addItem("Tube 1", 1);
    if (!tube2Taken || current == 2) cb->addItem("Tube 2", 2);

    // set current
    for (int i = 0; i < cb->count(); ++i) {
        if (cb->itemData(i).toInt() == current) {
            cb->setCurrentIndex(i);
            break;
        }
    }
}

void TubeOrderDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& idx) const {
    auto* cb = qobject_cast<QComboBox*>(editor);
    if (!cb) return;
    const int v = cb->currentData().toInt(); // 0/1/2
    model->setData(idx, v, Qt::EditRole);
}