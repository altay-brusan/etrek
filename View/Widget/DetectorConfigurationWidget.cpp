#include "DetectorConfigurationWidget.h"
#include "ui_DetectorConfigurationWidget.h"

#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>
#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "DetectorTableModel.h"

DetectorConfigurationWidget::DetectorConfigurationWidget(const QVector<Detector>& nodes,QWidget* parent)
	: QWidget(parent),
      m_nodes(nodes)
    , ui(new Ui::DetectorConfigurationWidget)
{
    ui->setupUi(this);

    // --- reusable snippets ---
    const char* lineCss =
        "QLineEdit {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:28px; padding:2px 6px; }"
        "QLineEdit:focus { border-color: rgb(160,160,160); }";

    const char* comboCss =
        "QComboBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:28px; padding:2px 28px 2px 6px;" /* room for arrow */
        "}"
        "QComboBox:focus { border-color: rgb(160,160,160); }"

        /* drop-down button area */
        "QComboBox::drop-down {"
        "  width:26px; border-left:1px solid rgb(120,120,120); margin:0;"
        "}"

        /* the arrow itself */
        "QComboBox::down-arrow {"
        "  image: url(:/Images/Asset/Icon/chevron-down.svg);"
        "  width: 12px; height: 12px;"
        "}"
        "QComboBox::down-arrow:disabled {"
        "  image: url(:/Images/Asset/Icon/chevron-down.svg);"
        "  /* optional: reduce opacity via a lighter icon if you have one */"
        "}"

        /* popup list */
        "QComboBox QAbstractItemView {"
        "  border:1px solid rgb(120,120,120); background:rgb(74,74,74);"
        "  selection-background-color: rgb(92,92,92);"
        "}";

    const char* spinCss =
        "QSpinBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:28px; padding:2px 6px; }"
        "QSpinBox:focus { border-color: rgb(160,160,160); }";

    const char* dateCss =
        "QDateEdit {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:28px; padding:2px 6px; }"
        "QDateEdit:focus { border-color: rgb(160,160,160); }"
        "QDateEdit::drop-down { width:26px; border-left:1px solid rgb(120,120,120); margin:0; }";

    const char* groupCss =
        "QGroupBox {"
        "  border:1px solid rgb(120,120,120); border-radius:6px; margin-top:18px; }"
        "QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 6px; }";

    // Subtle, clean checkbox style (new)
    const char* checkCss =
        "QCheckBox { spacing: 8px; }"
        "QCheckBox::indicator { width:16px; height:16px; }"
        "QCheckBox::indicator:unchecked {"
        "  border:1px solid rgb(120,120,120); background:rgb(74,74,74); border-radius:3px; }"
        "QCheckBox::indicator:unchecked:hover { border-color: rgb(160,160,160); }"
        "QCheckBox::indicator:checked {"
        "  border:1px solid rgb(160,160,160); background:rgb(100,100,100); border-radius:3px; }";

    const char* tabCss =
        "QTabWidget::pane { border:1px solid rgb(120,120,120); border-radius:6px; padding-top:4px; }"
        "QTabBar::tab { padding:6px 12px; }"
        "QTabBar::tab:selected { background: rgb(92,92,92); }";

    // Table styling (same as generator table)
    const char* tableCss =
        "QTableView {"
        "  background-color: rgb(83,83,83);"
        "  color: white;"
        "  gridline-color: #a0a0a0;"
        "  selection-background-color: #4a90e2;"
        "  selection-color: white;"
        "}"
        "QHeaderView::section {"
        "  background-color: #eaffea;"   /* very light green */
        "  color: black;"
        "  font-weight: bold;"
        "  border: 1px solid #c0c0c0;"
        "  padding: 4px;"
        "}";

    // (Optional) Buttons to match — keep subtle
    const char* btnCss =
        "QPushButton {"
        "  border:1px solid rgb(120,120,120); border-radius:6px;"
        "  background:rgb(74,74,74); padding:6px 12px; }"
        "QPushButton:hover { border-color: rgb(160,160,160); }"
        "QPushButton:pressed { background: rgb(92,92,92); }";

    // --- apply per widget ---
    
   // Create the model
    auto* baseModel = new DetectorTableModel(this);
    baseModel->setDataSource(&m_nodes);

    // Create proxy models for sorting and filtering
    auto* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(baseModel);
    proxy->setDynamicSortFilter(true);

    // Set models for both tables
    ui->detectorTableView->setModel(proxy);


    // Set the selection behavior for both tables to select the entire row
    ui->detectorTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Set the selection mode to single selection (one row at a time)
    ui->detectorTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Enable sorting
    ui->detectorTableView->setSortingEnabled(true);


	// detector styles
	ui->detectorTableView->setStyleSheet(tableCss);

    // Tab widget
    ui->tabWidget->setStyleSheet(tabCss);


     // Detector 1 — ComboBoxes
    ui->detector1RotationDegreeComboBox->setStyleSheet(comboCss);
    if (auto w = findChild<QComboBox*>("dector1CropModeComboBox")) w->setStyleSheet(comboCss); // keep typo-safe
    ui->detector1CropModeComboBox->setStyleSheet(comboCss);
    if (auto w = findChild<QComboBox*>("dector1SaveRawDataComboBox")) w->setStyleSheet(comboCss); // keep typo-safe
    ui->detector1PositionComboBox->setStyleSheet(comboCss);

    // Detector 2 — ComboBoxes
    ui->detector2RotationDegreeComboBox->setStyleSheet(comboCss);
    if (auto w = findChild<QComboBox*>("dector2CropModeComboBox")) w->setStyleSheet(comboCss);
    ui->detector2CropModeComboBox->setStyleSheet(comboCss);
    if (auto w = findChild<QComboBox*>("dector2SaveRawDataComboBox")) w->setStyleSheet(comboCss);
    ui->detector2PositionComboBox->setStyleSheet(comboCss);

	ui->connectedDetector1ComboBox->setStyleSheet(comboCss);
	ui->connectedDetector2ComboBox->setStyleSheet(comboCss);

    // Detector 1 — SpinBoxes
    ui->detector1SaturationValueSpinBox->setStyleSheet(spinCss);
    ui->detector1FullWidthSpinBox->setStyleSheet(spinCss);
    ui->detector1FullHeightSpinBox->setStyleSheet(spinCss);
    ui->detector1WidthShiftSpinBox->setStyleSheet(spinCss);
    ui->detector1HeightShiftSpinBox->setStyleSheet(spinCss);

    // Detector 2 — SpinBoxes
    ui->detector2SaturationValueSpinBox->setStyleSheet(spinCss);
    ui->detector2FullWidthSpinBox->setStyleSheet(spinCss);
    ui->detector2FullHeightSpinBox->setStyleSheet(spinCss);
    ui->detector2WidthShiftSpinBox->setStyleSheet(spinCss);
    ui->detector2HeightShiftSpinBox->setStyleSheet(spinCss);

    // DateEdits
    ui->detector1LastCalibrationDateEdit->setStyleSheet(dateCss);
    ui->detector2LastCalibrationDateEdit->setStyleSheet(dateCss);

    // GroupBoxes
    ui->detector1PreProcessStepsGroupBox->setStyleSheet(groupCss);
    ui->detector1ConfigurationGroupBox->setStyleSheet(groupCss);
    ui->detector1SizeGroupBox->setStyleSheet(groupCss);
    ui->connectedDetector1GroupBox->setStyleSheet(groupCss);
    ui->connectedDetector2GroupBox->setStyleSheet(groupCss);

    ui->detectorListGroupBox->setStyleSheet(groupCss);

    // name in .ui: detector2PreProcessStepsGroupBox_2
    if (auto gb = findChild<QGroupBox*>("detector2PreProcessStepsGroupBox_2")) gb->setStyleSheet(groupCss);
    ui->detector2ConfigurationGroupBox->setStyleSheet(groupCss);
    ui->detector2SizeGroupBox->setStyleSheet(groupCss);


    // CheckBoxes — Detector 1
    ui->detector1HorizontalFlipCheckBox->setStyleSheet(checkCss);
    ui->detector1EnableCheckBox->setStyleSheet(checkCss);
    ui->detector1EnableDeadPixelFilterCheckBox->setStyleSheet(checkCss);
    ui->detector1ConnectDetectorCheckBox->setStyleSheet(checkCss);

    // CheckBoxes — Detector 2
    ui->detector2HorizontalFlipCheckBox->setStyleSheet(checkCss);
    ui->detector2EnableCheckBox->setStyleSheet(checkCss);
    ui->detector2EnableDeadPixelFilterCheckBox->setStyleSheet(checkCss);
    ui->detector2ConnectDetectorCheckBox->setStyleSheet(checkCss);


    ui->detector1DiagnosisPushButton->setStyleSheet(btnCss);
    ui->detector1DataChannelSettingBtn->setStyleSheet(btnCss);
    ui->detector2DiagnosisPushButton->setStyleSheet(btnCss);
    ui->detector2DataChannelSettingBtn->setStyleSheet(btnCss);



}

DetectorConfigurationWidget::~DetectorConfigurationWidget()
{
    delete ui;
}
