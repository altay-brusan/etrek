// GeneratorConfigurationWidget.cpp

#include "GeneratorConfigurationWidget.h"
#include "ui_GeneratorConfigurationWidget.h"
#include "GeneratorTableModel.h"
#include "TubeComboDelegate.h"
#include "DateDelegate.h"
#include "TubeTableModel.h"
#include "PositionComboDelegate.h"
#include "IntSpinDelegate.h"
#include "TubeOrderDelegate.h"

#include <QComboBox>
#include <QVariant>

using namespace Etrek::Device::Data::Entity;

GeneratorConfigurationWidget::GeneratorConfigurationWidget(
    const QVector<Generator>& generators,
	const QVector<XRayTube>& xRayTubes,
    QWidget* parent
)
    : QWidget(parent)
    , ui(new Ui::GeneratorConfigurationWidget)
    , allXRayTubes(xRayTubes)
{
    ui->setupUi(this);
    auto* model = new GeneratorTableModel(this);
    model->setDataSource(generators);

    
      auto* view = ui->generatorTableView;
      view->setModel(model);
      view->setAlternatingRowColors(false);
      view->setSelectionBehavior(QAbstractItemView::SelectRows);
      // Force a uniform background for all rows
      view->setStyleSheet(R"(
        QTableView {
        background-color: rgb(83, 83, 83);
        color: white;                       /* white text for contrast */
        gridline-color: #a0a0a0;
        selection-background-color: #4a90e2; /* optional: blue highlight */
        selection-color: white;
        }
        QHeaderView::section {
            background-color: #eaffea;  /* very light green */
            color: black;
            font-weight: bold;
            border: 1px solid #c0c0c0;
                padding: 4px;
            }
        )");
      view->setSelectionMode(QAbstractItemView::SingleSelection);
      view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    
      // Delegates
      auto* dateDel = new DateDelegate(view);
      view->setItemDelegateForColumn(GeneratorTableModel::ColMfgDate, dateDel);
      view->setItemDelegateForColumn(GeneratorTableModel::ColInstDate, dateDel);
      view->setItemDelegateForColumn(GeneratorTableModel::ColCalibDate, dateDel);
    
      auto* out1Del = new TubeComboDelegate(false, view);
      auto* out2Del = new TubeComboDelegate( true, view);
      view->setItemDelegateForColumn(GeneratorTableModel::ColOutput1, out1Del);
      view->setItemDelegateForColumn(GeneratorTableModel::ColOutput2, out2Del);
    
      // Optional cosmetics
      auto* gh = view->horizontalHeader();
      gh->setStretchLastSection(false);

      // One-time autosize to content, then let user resize freely
      view->resizeColumnsToContents();

      // Let user drag column borders
      gh->setSectionResizeMode(QHeaderView::Interactive);

      // (optional) sensible min/default sizes
      gh->setMinimumSectionSize(60);
      gh->setDefaultSectionSize(120);

      // If you still want TechSpecs to eat remaining space (others stay resizable):
      // gh->setSectionResizeMode(GeneratorTableModel::ColTechSpecs, QHeaderView::Stretch);

      // Smooth horizontal scrolling when table gets wider than viewport
      view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);


      //==================
      // Tubes
      //==================

      auto* tubeModel = new TubeTableModel(this);
      tubeModel->setDataSource(xRayTubes);                 // QVector<XRayTube>
      auto* tubeView = ui->tubeTableView;
      tubeView->setModel(tubeModel);

      // same look & feel as your generator table
      tubeView->setAlternatingRowColors(false);
      tubeView->setStyleSheet(R"(
            QTableView {
                background-color: rgb(83,83,83);
                color: white;
                gridline-color: #a0a0a0;
                selection-background-color: #4a90e2;
                selection-color: white;
            }
            QHeaderView::section {
                background-color: #eaffea;   /* very light green */
                color: black;
                font-weight: bold;
                border: 1px solid #c0c0c0;
                padding: 4px;
            }
        )");

      tubeView->setSelectionBehavior(QAbstractItemView::SelectRows);
      tubeView->setSelectionMode(QAbstractItemView::SingleSelection);
      tubeView->setEditTriggers(QAbstractItemView::DoubleClicked |
          QAbstractItemView::SelectedClicked |
          QAbstractItemView::EditKeyPressed);

      // Delegates
      auto* dateDel2 = new DateDelegate(tubeView);
      tubeView->setItemDelegateForColumn(TubeTableModel::ColMfgDate, dateDel2);
      tubeView->setItemDelegateForColumn(TubeTableModel::ColInstDate, dateDel2);
      tubeView->setItemDelegateForColumn(TubeTableModel::ColCalibDate, dateDel2);

      auto* posDel = new PositionComboDelegate(tubeView);
      tubeView->setItemDelegateForColumn(TubeTableModel::ColPosition, posDel);

      auto* orderDel = new TubeOrderDelegate(tubeView);
      tubeView->setItemDelegateForColumn(TubeTableModel::ColTubeOrder, orderDel);

      // Spin delegates (choose sensible ranges)
      tubeView->setItemDelegateForColumn(TubeTableModel::ColAnodeHeatCapacity, new IntSpinDelegate(0, 5'000'000, 100, tubeView));
      tubeView->setItemDelegateForColumn(TubeTableModel::ColCoolingRate, new IntSpinDelegate(0, 500'000, 10, tubeView));
      tubeView->setItemDelegateForColumn(TubeTableModel::ColMaxVoltage, new IntSpinDelegate(0, 200, 1, tubeView));
      tubeView->setItemDelegateForColumn(TubeTableModel::ColMaxCurrent, new IntSpinDelegate(0, 2000, 10, tubeView));

      // Column sizing
      auto* th = tubeView->horizontalHeader();
      th->setStretchLastSection(false);

      // First fit to contents, then allow manual resizing
      tubeView->resizeColumnsToContents();
      th->setSectionResizeMode(QHeaderView::Interactive);

      // (optional)
      th->setMinimumSectionSize(60);
      th->setDefaultSectionSize(120);
      // If you want PowerRange to stretch instead:
      // th->setSectionResizeMode(TubeTableModel::ColPowerRange, QHeaderView::Stretch);

      tubeView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);



    

}

GeneratorConfigurationWidget::~GeneratorConfigurationWidget()
{
    delete ui;
}
