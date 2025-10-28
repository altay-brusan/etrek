#include "AddPatientDialog.h"
#include "ui_AddPatientDialog.h"
#include <QMessageBox>
#include <QDate>

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
    ui->ageLineEdit->setReadOnly(true);
    ui->bodyPartsComboBox->setVisible(false);
    ui->bodyPartsLabel->setVisible(false);

    // Cache optional widgets by object name for robustness across UI changes
    m_selectedRegionLineEdit = this->findChild<QLineEdit*>("selectedRegionLineEdit");

    // Initial view: both images already visible; no toggle needed

    // Initialize region/body part data from injected entities
    initializeFromEntities();

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
}

void AddPatientDialog::initializeFromEntities()
{
    // Start at first active region if available
    if (!m_regions.isEmpty()) {
        m_currentRegionIndex = 0;
        m_patientData.selectedRegion = m_regions[m_currentRegionIndex];
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
    connect(ui->dateOfBirthDateEdit, &QDateEdit::dateChanged, this, &AddPatientDialog::onDateOfBirthChanged);

    // Gender combo box
    connect(ui->genderComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddPatientDialog::onFormFieldChanged);

    // No view toggle buttons; labels only

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
    bool isValid = !ui->firstNameLineEdit->text().trimmed().isEmpty() &&
                   !ui->lastNameLineEdit->text().trimmed().isEmpty() &&
                   !ui->patientIdLineEdit->text().trimmed().isEmpty() &&
                   ui->dateOfBirthDateEdit->date().isValid();

    updateSaveButtonState();
    emit patientDataChanged(isValid);
}

void AddPatientDialog::updateSaveButtonState()
{
    bool isValid = !ui->firstNameLineEdit->text().trimmed().isEmpty() &&
                   !ui->lastNameLineEdit->text().trimmed().isEmpty() &&
                   !ui->patientIdLineEdit->text().trimmed().isEmpty() &&
                   ui->dateOfBirthDateEdit->date().isValid();

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

// Front/Back view toggling removed; both images are shown simultaneously

void AddPatientDialog::updateRegionDisplay()
{
    if (m_currentRegionIndex >= 0 && m_currentRegionIndex < m_regions.size()) {
        const auto& ar = m_regions[m_currentRegionIndex];
        if (m_selectedRegionLineEdit) m_selectedRegionLineEdit->setText(ar.Name);
        m_patientData.selectedRegion = ar;
    } else {
        if (m_selectedRegionLineEdit) m_selectedRegionLineEdit->clear();
    }
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
        m_patientData.selectedBodyPart = partsForRegion.first();
        ui->bodyPartsComboBox->setCurrentIndex(0);
    } else {
        m_patientData.selectedBodyPart = BodyPart{};
    }
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
    if (index >= 0) {
        m_patientData.selectedBodyPart = ui->bodyPartsComboBox->currentData().value<BodyPart>();
    }
}

PatientData AddPatientDialog::getPatientData() const
{
    PatientData data;
    data.firstName = ui->firstNameLineEdit->text().trimmed();
    data.middleName = ui->middleNameLineEdit->text().trimmed();
    data.lastName = ui->lastNameLineEdit->text().trimmed();
    data.patientId = ui->patientIdLineEdit->text().trimmed();
    data.dateOfBirth = ui->dateOfBirthDateEdit->date();
    data.age = ui->ageLineEdit->text().toInt();

    int genderIndex = ui->genderComboBox->currentIndex();
    data.gender = static_cast<Gender>(genderIndex);

    data.referringPhysician = ui->referringPhysicianLineEdit->text().trimmed();
    data.patientLocation = ui->patientLocationLineEdit->text().trimmed();
    data.admissionNumber = ui->admissionNumberLineEdit->text().trimmed();
    data.accessionNumber = ui->accessionNumberLineEdit->text().trimmed();
    if (m_currentRegionIndex >= 0 && m_currentRegionIndex < m_regions.size())
        data.selectedRegion = m_regions[m_currentRegionIndex];
    else
        data.selectedRegion = AnatomicRegion{};

    data.selectedBodyPart = ui->bodyPartsComboBox->currentData().value<BodyPart>();

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
