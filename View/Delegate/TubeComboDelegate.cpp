#include "TubeComboDelegate.h"
#include <QComboBox>
#include <QAbstractItemModel>
#include "GeneratorTableModel.h"

QWidget* TubeComboDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& idx) const
{
    auto* cb = new QComboBox(parent);
    cb->setEditable(false);
    // Fill both; we'll filter in setEditorData when needed
    cb->addItem("Tube 1", 1);
    cb->addItem("Tube 2", 2);
    return cb;
}

void TubeComboDelegate::setEditorData(QWidget* editor, const QModelIndex& idx) const
{
    auto* cb = qobject_cast<QComboBox*>(editor);
    if (!cb) return;

    int current = idx.model()->data(idx, Qt::EditRole).toInt();

    // If editing Output2 and it's active, restrict choices to the "other tube"
    if (m_isOutput2) {
        const auto out1Idx = idx.model()->index(idx.row(), GeneratorTableModel::ColOutput1);
        const auto act2Idx = idx.model()->index(idx.row(), GeneratorTableModel::ColOutput2Active);

        const int out1Val = out1Idx.data(Qt::EditRole).toInt();
        const bool out2Active = (act2Idx.data(Qt::CheckStateRole).toInt() == Qt::Checked);

        cb->clear();
        if (out2Active) {
            const int onlyVal = (out1Val == 1 ? 2 : 1);
            cb->addItem(onlyVal == 1 ? "Tube 1" : "Tube 2", onlyVal);
            current = onlyVal; // force-view to only valid
        }
        else {
            cb->addItem("Tube 1", 1);
            cb->addItem("Tube 2", 2);
        }
    }

    // Set current index by matching userData
    for (int i = 0; i < cb->count(); ++i) {
        if (cb->itemData(i).toInt() == current) {
            cb->setCurrentIndex(i);
            break;
        }
    }
}

void TubeComboDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& idx) const
{
    auto* cb = qobject_cast<QComboBox*>(editor);
    if (!cb) return;
    const int v = cb->currentData().toInt();
    model->setData(idx, v, Qt::EditRole);
}
