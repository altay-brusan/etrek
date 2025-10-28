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
#include "ScanProtocolUtil.h"
#include "PatientModel.h"

namespace Ui {
class AddPatientDialog;
}

class AddPatientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPatientDialog(
        const QVector<Etrek::ScanProtocol::Data::Entity::AnatomicRegion>& regions,
        const QVector<Etrek::ScanProtocol::Data::Entity::BodyPart>& bodyParts,
        QWidget *parent = nullptr);
    ~AddPatientDialog();

    Etrek::ScanProtocol::Data::Model::PatientModel getPatientModel() const;
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
    void updateImagesForRegion(const Etrek::ScanProtocol::Data::Entity::AnatomicRegion& ar);
    void updateRegionDisplay();
    void updateBodyPartListForRegion(int regionIndex);

    Ui::AddPatientDialog *ui;
    QLineEdit* m_selectedRegionLineEdit = nullptr;
    const QVector<Etrek::ScanProtocol::Data::Entity::AnatomicRegion>& m_regions;
    const QVector<Etrek::ScanProtocol::Data::Entity::BodyPart>& m_bodyParts;
    int m_currentRegionIndex = -1;
    bool m_isFrontView = true; // deprecated visual state, kept for styling if needed
    Etrek::ScanProtocol::Data::Model::PatientModel m_patient;
};

#endif // ADDPATIENTDIALOG_H
