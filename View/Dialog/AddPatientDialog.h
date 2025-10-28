#ifndef ADDPATIENTDIALOG_H
#define ADDPATIENTDIALOG_H

#include <QDialog>
#include <QString>
#include <QDate>
#include <QMouseEvent>
#include <QMap>
#include <QStringList>
#include <QVector>
#include <QLineEdit>
#include "AnatomicRegion.h"
#include "BodyPart.h"

namespace Ui {
class AddPatientDialog;
}

enum class Gender
{
    Male,
    Female,
    Other
};

struct PatientData
{
    QString firstName;
    QString middleName;
    QString lastName;
    QString patientId;
    QDate dateOfBirth;
    int age;
    Gender gender;
    QString referringPhysician;
    QString patientLocation;
    QString admissionNumber;
    QString accessionNumber;
    Etrek::ScanProtocol::Data::Entity::AnatomicRegion selectedRegion;
    Etrek::ScanProtocol::Data::Entity::BodyPart selectedBodyPart;

    bool isValid() const
    {
        return !firstName.isEmpty() &&
               !lastName.isEmpty() &&
               !patientId.isEmpty() &&
               dateOfBirth.isValid();
    }
};

class AddPatientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPatientDialog(
        const QVector<Etrek::ScanProtocol::Data::Entity::AnatomicRegion>& regions,
        const QVector<Etrek::ScanProtocol::Data::Entity::BodyPart>& bodyParts,
        QWidget *parent = nullptr);
    ~AddPatientDialog();

    PatientData getPatientData() const;
    void clearForm();

signals:
    void patientDataChanged(bool isValid);

private slots:
    void onDateOfBirthChanged(const QDate& date);
    void onFormFieldChanged();
    void onPrevRegionClicked();
    void onNextRegionClicked();
    void onBodyPartSelectedIndex(int index);
    void validateForm();

private:
    void initializeStyles();
    void setupConnections();
    void initializeFromEntities();
    void updateAge();
    void updateSaveButtonState();
    void updateRegionDisplay();
    void updateBodyPartListForRegion(int regionIndex);

    Ui::AddPatientDialog *ui;
    QLineEdit* m_selectedRegionLineEdit = nullptr;
    const QVector<Etrek::ScanProtocol::Data::Entity::AnatomicRegion>& m_regions;
    const QVector<Etrek::ScanProtocol::Data::Entity::BodyPart>& m_bodyParts;
    int m_currentRegionIndex = -1;
    bool m_isFrontView = true; // deprecated visual state, kept for styling if needed
    PatientData m_patientData;
};

#endif // ADDPATIENTDIALOG_H
