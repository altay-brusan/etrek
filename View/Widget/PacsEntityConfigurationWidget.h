#ifndef PACSENTITYCONFIGURATIONWIDGET_H
#define PACSENTITYCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QHash>
#include <QItemSelection>
#include <QStandardItemModel>
#include "PacsNode.h"
#include "PacsEntityType.h" // PacsEntityType / Utils

namespace Ui {
    class PacsEntityConfigurationWidget;
}

using namespace Etrek::Pacs::Data::Entity; // PacsNode
using namespace Etrek::Pacs;              // PacsEntityType

class PacsEntityConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PacsEntityConfigurationWidget(const QVector<PacsNode>& nodes, QWidget* parent = nullptr);
    ~PacsEntityConfigurationWidget();

signals:
    // Let a delegate handle the real DICOM C-ECHO / assoc test
    void testConnectionRequested(const PacsNode& node);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
private slots:
    void onEntityTypeChanged(int index);
    void onArchiveSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onMppsSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onTestConnectionClicked();

private:
    Ui::PacsEntityConfigurationWidget* ui = nullptr;
    QVector<PacsNode> m_nodes;

    // Quick lookup by Id
    QHash<int, int> m_indexById; // Id -> index in m_nodes

    // Models for the lists
    QStandardItemModel* m_archiveModel = nullptr;
    QStandardItemModel* m_mppsModel = nullptr;

    // Helpers
    void populateEntityTypes();
    void buildNodeIndex();
    void buildModels();
    void bindListViews();

    static QString nodeDisplay(const PacsNode& n);
    void loadNodeIntoForm(const PacsNode* n);
    void clearForm();

    const PacsNode* selectedArchiveNode() const;
    const PacsNode* selectedMppsNode() const;
};

#endif // PACSENTITYCONFIGURATIONWIDGET_H
