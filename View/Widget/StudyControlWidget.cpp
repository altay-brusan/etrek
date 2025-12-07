#include "StudyControlWidget.h"
#include "ui_StudyControlWidget.h"
#include "View.h"
#include <QStandardItemModel>
#include <QHeaderView>

StudyControlWidget::StudyControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudyControlWidget)
{
    ui->setupUi(this);
}

StudyControlWidget::~StudyControlWidget()
{
    delete ui;
}

void StudyControlWidget::setViews(const QVector<Etrek::ScanProtocol::Data::Entity::View>& views)
{
    // Create a new model for the table
    QStandardItemModel* model = new QStandardItemModel(views.size(), 3, this);

    // Set column headers
    model->setHorizontalHeaderItem(0, new QStandardItem("View Name"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Body Part"));
    model->setHorizontalHeaderItem(2, new QStandardItem("Status"));

    // Populate the model with view data
    for (int row = 0; row < views.size(); ++row) {
        const auto& view = views[row];

        // View name
        QStandardItem* nameItem = new QStandardItem(view.Name);
        nameItem->setEditable(false);
        model->setItem(row, 0, nameItem);

        // Body part name
        QStandardItem* bodyPartItem = new QStandardItem(view.BodyPart.Name);
        bodyPartItem->setEditable(false);
        model->setItem(row, 1, bodyPartItem);

        // Status (placeholder - could show "Pending", "Acquired", etc.)
        QStandardItem* statusItem = new QStandardItem("Pending");
        statusItem->setEditable(false);
        model->setItem(row, 2, statusItem);
    }

    // Set the model on the table view
    ui->tableView->setModel(model);

    // Adjust column widths
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    // Select first row by default if views exist
    if (!views.isEmpty()) {
        ui->tableView->selectRow(0);
    }
}
