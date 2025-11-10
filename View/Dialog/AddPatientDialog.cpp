#include "AddPatientDialog.h"
#include "ui_AddPatientDialog.h"
#include <QMessageBox>
#include <QDate>
#include <QHeaderView>
#include <QStyle>

using namespace Etrek::ScanProtocol::Data::Entity;

AddPatientDialog::AddPatientDialog(
        const QVector<AnatomicRegion>& regions,
        const QVector<BodyPart>& bodyParts,
        QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddPatientDialog)
    , m_regions(regions)
    , m_bodyParts(bodyParts)
    , m_isFrontView(true)
{
    ui->setupUi(this);
    initializeStyles();
    setupConnections();

    // Defaults
    ui->dateOfBirthDateEdit->setDate(QDate::currentDate());
    ui->dateOfBirthDateEdit->setMaximumDate(QDate::currentDate());
    ui->ageLineEdit->setReadOnly(true);
    ui->bodyPartsComboBox->setVisible(false);
    ui->bodyPartsLabel->setVisible(false);

    // Cache optional widgets by object name for robustness across UI changes
    m_selectedRegionLineEdit = this->findChild<QLineEdit*>("selectedRegionLineEdit");

    // Initial view: both images already visible; no toggle needed

    // Initialize region/body part data from injected entities
    initializeFromEntities();
    initializeSelectedPartsTable();


    validateForm();
}

AddPatientDialog::~AddPatientDialog()
{
    delete ui;
}

void AddPatientDialog::initializeStyles()
{
    const char* lineCss =
        "QLineEdit {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "  min-height: 20px;"
        "  padding: 2px 6px;"
        "  color: rgb(230,230,230);"
        "}"
        "QLineEdit[invalid=\"true\"] { border: 1px solid rgb(200,60,60); }"
        "QLineEdit:focus { border-color: rgb(160,160,160); }"
        "QLineEdit:read-only { background: rgb(60,60,60); color: rgb(180,180,180); }";

    const char* dateCss =
        "QDateEdit {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "  min-height: 20px;"
        "  padding: 2px 6px;"
        "  color: rgb(230,230,230);"
        "}"
        "QDateEdit[invalid=\"true\"] { border: 1px solid rgb(200,60,60); }"
        "QDateEdit:focus { border-color: rgb(160,160,160); }"
        "QDateEdit::drop-down { width: 20px; border-left: 1px solid rgb(120,120,120); margin: 0; }";

    const char* comboCss =
        "QComboBox {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "  min-height: 20px;"
        "  padding: 2px 6px;"
        "  color: rgb(230,230,230);"
        "}"
        "QComboBox[invalid=\"true\"] { border: 1px solid rgb(200,60,60); }"
        "QComboBox:focus { border-color: rgb(160,160,160); }"
        "QComboBox::drop-down { width: 20px; border-left: 1px solid rgb(120,120,120); }";

    const char* buttonCss =
        "QPushButton {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "  min-height: 20px;"
        "  padding: 4px 12px;"
        "  color: rgb(230,230,230);"
        "}"
        "QPushButton:hover { border-color: rgb(160,160,160); background: rgb(92,92,92); }"
        "QPushButton:pressed { background: rgb(64,64,64); }"
        "QPushButton:focus { border-color: rgb(180,180,180); }"
        "QPushButton:disabled { border-color: rgb(80,80,80); background: rgb(60,60,60); color: rgb(130,130,130); }";

    const char* groupCss =
        "QGroupBox {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 6px;"
        "  margin-top: 18px;"
        "  color: rgb(230,230,230);"
        "}"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 6px; }";

    const char* labelCss =
        "QLabel { color: rgb(208,208,208); }";

    // Apply styles
    ui->firstNameLineEdit->setStyleSheet(lineCss);
    ui->middleNameLineEdit->setStyleSheet(lineCss);
    ui->lastNameLineEdit->setStyleSheet(lineCss);
    ui->patientIdLineEdit->setStyleSheet(lineCss);
    ui->ageLineEdit->setStyleSheet(lineCss);
    ui->referringPhysicianLineEdit->setStyleSheet(lineCss);
    ui->patientLocationLineEdit->setStyleSheet(lineCss);
    ui->admissionNumberLineEdit->setStyleSheet(lineCss);
    ui->accessionNumberLineEdit->setStyleSheet(lineCss);

    ui->dateOfBirthDateEdit->setStyleSheet(dateCss);
    ui->genderComboBox->setStyleSheet(comboCss);
    ui->bodyPartsComboBox->setStyleSheet(comboCss);

    // Front/Back text labels use default styles (no border)

    ui->demographicsGroupBox->setStyleSheet(groupCss);
    ui->bodyMapGroupBox->setStyleSheet(groupCss);
    if (ui->selectedPartsGroupBox) ui->selectedPartsGroupBox->setStyleSheet(groupCss);
    if (ui->addPartButton) ui->addPartButton->setStyleSheet(buttonCss);
    if (ui->removePartButton) ui->removePartButton->setStyleSheet(buttonCss);
}

void AddPatientDialog::initializeSelectedPartsTable()
{
    if (!ui->selectedPartsTable) return;
    auto* table = ui->selectedPartsTable;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    if (table->horizontalHeader()) {
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    table->setStyleSheet(R"(
        QTableView {
            background-color: rgb(83, 83, 83);
            color: white;
            gridline-color: #a0a0a0;
            selection-background-color: #4a90e2;
            selection-color: white;
        }
        QHeaderView::section {
            background-color: #eaffea;  /* very light green */
            color: black;
            font-weight: bold;
            border: 1px solid #c0c0c0;
            padding: 4px;
        }
        QTableView[invalid=\"true\"] { border: 1px solid rgb(200,60,60); }
    )");
}

void AddPatientDialog::initializeFromEntities()
{
    // Start at first active region if available
    if (!m_regions.isEmpty()) {
        m_currentRegionIndex = 0;
        updateRegionDisplay();
        updateBodyPartListForRegion(m_currentRegionIndex);
    } else {
        m_currentRegionIndex = -1;
        if (m_selectedRegionLineEdit) m_selectedRegionLineEdit->clear();
        ui->bodyPartsComboBox->clear();
        ui->bodyPartsComboBox->setVisible(false);
        ui->bodyPartsLabel->setVisible(false);
    }
}

void AddPatientDialog::setupConnections()
{
    // Form field connections
    connect(ui->firstNameLineEdit, &QLineEdit::textChanged, this, &AddPatientDialog::onFormFieldChanged);
    connect(ui->lastNameLineEdit, &QLineEdit::textChanged, this, &AddPatientDialog::onFormFieldChanged);
    connect(ui->patientIdLineEdit, &QLineEdit::textChanged, this, &AddPatientDialog::onFormFieldChanged);
    connect(ui->accessionNumberLineEdit, &QLineEdit::textChanged, this, &AddPatientDialog::onFormFieldChanged);
    connect(ui->dateOfBirthDateEdit, &QDateEdit::dateChanged, this, &AddPatientDialog::onDateOfBirthChanged);

    // Gender combo box
    connect(ui->genderComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddPatientDialog::onFormFieldChanged);

    // No view toggle buttons; labels only
    
    // Selected parts add/remove
    if (ui->addPartButton)
        connect(ui->addPartButton, &QPushButton::clicked, this, &AddPatientDialog::onAddPartClicked);
    if (ui->removePartButton)
        connect(ui->removePartButton, &QPushButton::clicked, this, &AddPatientDialog::onRemovePartClicked);
    if (ui->selectedPartsTable)
        connect(ui->selectedPartsTable, &QTableWidget::itemSelectionChanged, this, [this]() {
            if (!ui->removePartButton || !ui->selectedPartsTable) return;
            ui->removePartButton->setEnabled(ui->selectedPartsTable->currentRow() >= 0);
        });

    // Body parts combo box
    connect(ui->bodyPartsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddPatientDialog::onBodyPartSelectedIndex);

    // Prev/Next region navigation (buttons will be added in .ui and named accordingly)
    if (auto prevBtn = this->findChild<QPushButton*>("prevRegionButton"))
        connect(prevBtn, &QPushButton::clicked, this, &AddPatientDialog::onPrevRegionClicked);
    if (auto nextBtn = this->findChild<QPushButton*>("nextRegionButton"))
        connect(nextBtn, &QPushButton::clicked, this, &AddPatientDialog::onNextRegionClicked);

    // Dialog buttons
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void AddPatientDialog::onDateOfBirthChanged(const QDate& date)
{
    updateAge();
    onFormFieldChanged();
}

void AddPatientDialog::onFormFieldChanged()
{
    validateForm();
}

void AddPatientDialog::updateAge()
{
    QDate dob = ui->dateOfBirthDateEdit->date();
    if (dob.isValid()) {
        QDate currentDate = QDate::currentDate();
        int age = currentDate.year() - dob.year();

        // Adjust age if birthday hasn't occurred this year
        if (currentDate.month() < dob.month() ||
            (currentDate.month() == dob.month() && currentDate.day() < dob.day())) {
            age--;
        }

        ui->ageLineEdit->setText(QString::number(age));
    } else {
        ui->ageLineEdit->clear();
    }
}

void AddPatientDialog::validateForm()
{
    const bool hasBodyPart = ui->bodyPartsComboBox->count() > 0 && ui->bodyPartsComboBox->currentIndex() >= 0;
    const bool dobOk = ui->dateOfBirthDateEdit->date().isValid() && ui->dateOfBirthDateEdit->date() <= QDate::currentDate();
    const bool hasAccession = !ui->accessionNumberLineEdit->text().trimmed().isEmpty();
    const bool hasAnySelectedRow = ui->selectedPartsTable && ui->selectedPartsTable->rowCount() > 0;
    bool isValid = !ui->firstNameLineEdit->text().trimmed().isEmpty() &&
                   !ui->lastNameLineEdit->text().trimmed().isEmpty() &&
                   !ui->patientIdLineEdit->text().trimmed().isEmpty() &&
                   hasAccession &&
                   dobOk &&
                   hasBodyPart &&
                   hasAnySelectedRow;

    auto mark = [](QWidget* w, bool invalid) {
        if (!w) return;
        w->setProperty("invalid", invalid);
        if (w->style()) { w->style()->unpolish(w); w->style()->polish(w); }
        w->update();
    };
    mark(ui->firstNameLineEdit, ui->firstNameLineEdit->text().trimmed().isEmpty());
    mark(ui->lastNameLineEdit, ui->lastNameLineEdit->text().trimmed().isEmpty());
    mark(ui->patientIdLineEdit, ui->patientIdLineEdit->text().trimmed().isEmpty());
    mark(ui->accessionNumberLineEdit, !hasAccession);
    mark(ui->dateOfBirthDateEdit, !dobOk);
    mark(ui->bodyPartsComboBox, !hasBodyPart);
    mark(ui->selectedPartsTable, !hasAnySelectedRow);

    updateSaveButtonState();
    emit patientDataChanged(isValid);
}

void AddPatientDialog::updateSaveButtonState()
{
    const bool hasBodyPart = ui->bodyPartsComboBox->count() > 0 && ui->bodyPartsComboBox->currentIndex() >= 0;
    const bool dobOk = ui->dateOfBirthDateEdit->date().isValid() && ui->dateOfBirthDateEdit->date() <= QDate::currentDate();
    const bool hasAccession = !ui->accessionNumberLineEdit->text().trimmed().isEmpty();
    const bool hasAnySelectedRow = ui->selectedPartsTable && ui->selectedPartsTable->rowCount() > 0;
    bool isValid = !ui->firstNameLineEdit->text().trimmed().isEmpty() &&
                   !ui->lastNameLineEdit->text().trimmed().isEmpty() &&
                   !ui->patientIdLineEdit->text().trimmed().isEmpty() &&
                   hasAccession &&
                   dobOk &&
                   hasBodyPart &&
                   hasAnySelectedRow;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

// Front/Back view toggling removed; both images are shown simultaneously

void AddPatientDialog::updateRegionDisplay()
{
    if (m_currentRegionIndex >= 0 && m_currentRegionIndex < m_regions.size()) {
        const auto& ar = m_regions[m_currentRegionIndex];
        if (m_selectedRegionLineEdit) m_selectedRegionLineEdit->setText(ar.Name);
        updateImagesForRegion(ar);
    } else {
        if (m_selectedRegionLineEdit) m_selectedRegionLineEdit->clear();
    }
}

void AddPatientDialog::updateImagesForRegion(const AnatomicRegion& ar)
{
    const QString name = ar.Name.trimmed().toUpper();
    QString front = ":/Images/Asset/Image/blank-front.jpg";
    QString back  = ":/Images/Asset/Image/blank-back.jpg";

    auto setThorax = [&]() { front = ":/Images/Asset/Image/thorax-front.JPG"; back = ":/Images/Asset/Image/thorax-back.JPG"; };

    if (name.contains("HEAD")) {
        front = ":/Images/Asset/Image/head-front.JPG";
        back  = ":/Images/Asset/Image/head-back.JPG";
    } else if (name.contains("NECK")) {
        front = ":/Images/Asset/Image/neck-front.JPG";
        back  = ":/Images/Asset/Image/neck-back.JPG";
    } else if (name.contains("THORAX") || name.contains("CHEST")) {
        setThorax();
    } else if (name.contains("ABDOMEN")) {
        // Note: asset spelling is 'abdoman'
        front = ":/Images/Asset/Image/abdoman-front.JPG";
        back  = ":/Images/Asset/Image/abdoman-back.JPG";
    } else if (name.contains("PELVIS")) {
        front = ":/Images/Asset/Image/pelvis-front.JPG";
        back  = ":/Images/Asset/Image/pelvis-back.JPG";
    } else if (name.contains("SPINE")) {
        front = ":/Images/Asset/Image/spine.JPG";
        back  = ":/Images/Asset/Image/spine.JPG";
    } else if (name.contains("UPPER") || name.contains("UPPER EXTREMITY")) {
        front = ":/Images/Asset/Image/upper-extremity-front.JPG";
        back  = ":/Images/Asset/Image/upper-extremity-back.JPG";
    } else if (name.contains("LOWER") || name.contains("LOWER EXTREMITY")) {
        // Note: asset spelling uses 'font' for front
        front = ":/Images/Asset/Image/lower-extremity-font.JPG";
        back  = ":/Images/Asset/Image/lower-extremity-back.JPG";
    }

    const QSize boxSizeF = ui->frontViewLabel->size();
    const QSize boxSizeB = ui->backViewLabel->size();

    QPixmap pmFront(front);
    QPixmap pmBack(back);

    if (!pmFront.isNull())
        pmFront = pmFront.scaled(boxSizeF, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (!pmBack.isNull())
        pmBack = pmBack.scaled(boxSizeB, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->frontViewLabel->setPixmap(pmFront);
    ui->backViewLabel->setPixmap(pmBack);
}

void AddPatientDialog::updateBodyPartListForRegion(int regionIndex)
{
    if (regionIndex < 0 || regionIndex >= m_regions.size()) {
        ui->bodyPartsComboBox->clear();
        ui->bodyPartsComboBox->setVisible(false);
        ui->bodyPartsLabel->setVisible(false);
        return;
    }

    const int regionId = m_regions[regionIndex].Id;
    ui->bodyPartsComboBox->clear();

    QVector<BodyPart> partsForRegion;
    partsForRegion.reserve(m_bodyParts.size());
    for (const auto& bp : m_bodyParts) {
        if (bp.Region.Id == regionId) {
            partsForRegion.push_back(bp);
        }
    }

    for (const auto& bp : partsForRegion) {
        ui->bodyPartsComboBox->addItem(bp.Name, QVariant::fromValue(bp));
    }

    const bool hasParts = !partsForRegion.isEmpty();
    ui->bodyPartsComboBox->setVisible(hasParts);
    ui->bodyPartsLabel->setVisible(hasParts);

    if (hasParts) {
        // Default select first
        ui->bodyPartsComboBox->setCurrentIndex(0);
    }
    // Re-validate when region/body part changes
    validateForm();
}

void AddPatientDialog::onPrevRegionClicked()
{
    if (m_regions.isEmpty()) return;
    m_currentRegionIndex = (m_currentRegionIndex <= 0) ? (m_regions.size() - 1) : (m_currentRegionIndex - 1);
    updateRegionDisplay();
    updateBodyPartListForRegion(m_currentRegionIndex);
}

void AddPatientDialog::onNextRegionClicked()
{
    if (m_regions.isEmpty()) return;
    m_currentRegionIndex = (m_currentRegionIndex + 1) % m_regions.size();
    updateRegionDisplay();
    updateBodyPartListForRegion(m_currentRegionIndex);
}

void AddPatientDialog::onBodyPartSelectedIndex(int index)
{
    // Body part selection is now managed via the table, not m_patient directly
    validateForm();
}

void AddPatientDialog::onAddPartClicked()
{
    if (m_currentRegionIndex < 0 || m_currentRegionIndex >= m_regions.size()) return;
    if (!ui->bodyPartsComboBox || ui->bodyPartsComboBox->currentIndex() < 0) return;
    if (!ui->selectedPartsTable) return;

    const auto& ar = m_regions[m_currentRegionIndex];
    const auto bp = ui->bodyPartsComboBox->currentData().value<BodyPart>();
    if (bp.Id < 0) return;

    // Avoid duplicates: check existing rows for same region/bodyPart ids
    for (int r = 0; r < ui->selectedPartsTable->rowCount(); ++r) {
        const auto rid = ui->selectedPartsTable->item(r, 0)->data(Qt::UserRole).toInt();
        const auto bpid = ui->selectedPartsTable->item(r, 1)->data(Qt::UserRole).toInt();
        if (rid == ar.Id && bpid == bp.Id)
            return;
    }

    int row = ui->selectedPartsTable->rowCount();
    ui->selectedPartsTable->insertRow(row);

    auto* regionItem = new QTableWidgetItem(ar.Name);
    regionItem->setData(Qt::UserRole, ar.Id);
    auto* bodyItem = new QTableWidgetItem(bp.Name);
    bodyItem->setData(Qt::UserRole, bp.Id);

    ui->selectedPartsTable->setItem(row, 0, regionItem);
    ui->selectedPartsTable->setItem(row, 1, bodyItem);
    ui->selectedPartsTable->setCurrentCell(row, 0);
    if (ui->removePartButton)
        ui->removePartButton->setEnabled(true);

    // Trigger validation to update OK button state
    validateForm();
}

void AddPatientDialog::onRemovePartClicked()
{
    if (!ui->selectedPartsTable) return;
    int row = ui->selectedPartsTable->currentRow();
    if (row < 0) {
        if (ui->selectedPartsTable->rowCount() == 0) {
            if (ui->removePartButton) ui->removePartButton->setEnabled(false);
            return;
        }
        row = ui->selectedPartsTable->rowCount() - 1;
    }
    ui->selectedPartsTable->removeRow(row);
    const int rows = ui->selectedPartsTable->rowCount();
    if (rows > 0) {
        const int newRow = (row >= rows) ? rows - 1 : row;
        ui->selectedPartsTable->setCurrentCell(newRow, 0);
        if (ui->removePartButton) ui->removePartButton->setEnabled(true);
    } else {
        if (ui->removePartButton) ui->removePartButton->setEnabled(false);
    }

    // Trigger validation to update OK button state
    validateForm();
}

Etrek::ScanProtocol::Data::Model::PatientModel AddPatientDialog::getPatientModel() const
{
    Etrek::ScanProtocol::Data::Model::PatientModel data;
    data.firstName = ui->firstNameLineEdit->text().trimmed();
    data.middleName = ui->middleNameLineEdit->text().trimmed();
    data.lastName = ui->lastNameLineEdit->text().trimmed();
    data.patientId = ui->patientIdLineEdit->text().trimmed();
    data.dateOfBirth = ui->dateOfBirthDateEdit->date();
    data.age = ui->ageLineEdit->text().toInt();

    // Robust gender mapping via util
    if (auto g = Etrek::ScanProtocol::ScanProtocolUtil::parseGender(ui->genderComboBox->currentText()))
        data.gender = *g;

    data.referringPhysician = ui->referringPhysicianLineEdit->text().trimmed();
    data.patientLocation = ui->patientLocationLineEdit->text().trimmed();
    data.admissionNumber = ui->admissionNumberLineEdit->text().trimmed();
    data.accessionNumber = ui->accessionNumberLineEdit->text().trimmed();

    // Build selectedBodyParts from table
    if (ui->selectedPartsTable) {
        for (int row = 0; row < ui->selectedPartsTable->rowCount(); ++row) {
            const int regionId = ui->selectedPartsTable->item(row, 0)->data(Qt::UserRole).toInt();
            const int bodyPartId = ui->selectedPartsTable->item(row, 1)->data(Qt::UserRole).toInt();

            // Find the region and body part entities by ID
            AnatomicRegion region;
            for (const auto& r : m_regions) {
                if (r.Id == regionId) {
                    region = r;
                    break;
                }
            }

            BodyPart bodyPart;
            for (const auto& bp : m_bodyParts) {
                if (bp.Id == bodyPartId) {
                    bodyPart = bp;
                    break;
                }
            }

            // Create BodyPartSelection and add to list
            Etrek::ScanProtocol::Data::Model::BodyPartSelection selection;
            selection.region = region;
            selection.bodyPart = bodyPart;
            data.selectedBodyParts.append(selection);
        }
    }

    return data;
}

void AddPatientDialog::clearForm()
{
    ui->firstNameLineEdit->clear();
    ui->middleNameLineEdit->clear();
    ui->lastNameLineEdit->clear();
    ui->patientIdLineEdit->clear();
    ui->dateOfBirthDateEdit->setDate(QDate::currentDate());
    ui->ageLineEdit->clear();
    ui->genderComboBox->setCurrentIndex(0);
    ui->referringPhysicianLineEdit->clear();
    ui->patientLocationLineEdit->clear();
    ui->admissionNumberLineEdit->clear();
    ui->accessionNumberLineEdit->clear();
    ui->bodyPartsComboBox->clear();
    ui->bodyPartsComboBox->setVisible(false);
    ui->bodyPartsLabel->setVisible(false);
    if (m_selectedRegionLineEdit) m_selectedRegionLineEdit->clear();

    m_currentRegionIndex = -1;

    validateForm();
}
