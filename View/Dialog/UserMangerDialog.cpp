#include "UserMangerDialog.h"
#include <QStringListModel>
#include <QMessageBox>
#include <QPushButton>
#include "ui_UserMangerDialog.h"
#include "MessageKey.h"
#include "User.h"
#include "Role.h"
#include "TranslationProvider.h"

using namespace Etrek::Core::Data::Entity;
using namespace Etrek::Core::Globalization;


UserMangerDialog::UserMangerDialog(const QVector<User>& activeUsers,
                                   const QVector<User>& deactivatedUsers,
                                   const QVector<Role>& roles,
                                   TranslationProvider* translationProvider,
                                   QWidget *parent)
    : QDialog(parent),
    m_roles(roles),
    m_activeUsers(activeUsers),
    m_deactivatedUsers(deactivatedUsers),
    translator(translationProvider),
    ui(new Ui::UserMangerDialog)
{
    ui->setupUi(this);

    QString pleaseSelectOneRoleMessage = translator->getInfoMessage(AUTH_PLEASE_SELECT_ONE_MSG);
    QString SelectOneForEditMessage = translator->getInfoMessage(AUTH_SELECT_ONE_FOR_EDIT_MSG);
    ui->activeUsersComboBox->addItem(SelectOneForEditMessage, QVariant()); // index 0
    for (int i = 0; i < m_activeUsers.size(); ++i)
        ui->activeUsersComboBox->addItem(m_activeUsers[i].Username, QVariant::fromValue(m_activeUsers[i]));

    ui->deactivatedUsersListComboBox->addItem(SelectOneForEditMessage, QVariant()); // index 0
    for (int i = 0; i < m_deactivatedUsers.size(); ++i)
        ui->deactivatedUsersListComboBox->addItem(m_deactivatedUsers[i].Username, QVariant::fromValue(m_deactivatedUsers[i]));

    ui->rolesComboBox->addItem(pleaseSelectOneRoleMessage, QVariant()); // index 0
    for (int i = 0; i < m_roles.size(); ++i)
        ui->rolesComboBox->addItem(m_roles[i].Name, QVariant::fromValue(m_roles[i]));


    connect(ui->addUserBtn, &QPushButton::clicked, this, &UserMangerDialog::onAddUserBtnClicked);
    connect(ui->updateUserBtn, &QPushButton::clicked, this, &UserMangerDialog::onUpdateUserBtnClicked);
    connect(ui->deleteUserBtn, &QPushButton::clicked, this, &UserMangerDialog::onDeleteUserBtnClicked);
    connect(ui->okBtn, &QPushButton::clicked, this, &UserMangerDialog::onOkBtnClicked);
    connect(ui->activeUsersComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UserMangerDialog::onActiveUserChanged);
    connect(ui->deactivatedUsersListComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UserMangerDialog::onDeactiveUserChanged);


    UpdateActionButtonsState();
    // TODO: do we need this?
    //onActiveUserChanged(ui->activeUsersComboBox->currentIndex());
    //ui->rolesComboBox->setItemData(0, 0, Qt::UserRole - 1); // Disable

    initStyles();
}

void UserMangerDialog::onActiveUserChanged(int index)
{
    if (index <= 0 || index > m_activeUsers.size()) {
        // Clear fields if "Select" is chosen
        ui->userNameLineEdit->clear();
        ui->nameLineEdit->clear();
        ui->surnameLineEdit->clear();
        ui->activeCheckBox->setChecked(false);
        return;
    }

    const auto& user = m_activeUsers.at(index - 1); // -1 since we inserted "Select"

    ui->userNameLineEdit->setText(user.Username);
    ui->nameLineEdit->setText(user.Name);
    ui->surnameLineEdit->setText(user.Surname);
    ui->activeCheckBox->setChecked(user.IsActive);

    if (!user.Roles.isEmpty()) {
        const auto& role = user.Roles.first();
        int roleIndex = ui->rolesComboBox->findData(QVariant::fromValue(role));
        if (roleIndex != -1) {
            ui->rolesComboBox->setCurrentIndex(roleIndex);
        }
    }
}
void UserMangerDialog::onDeactiveUserChanged(int index)
{
    if (index <= 0 || index > m_deactivatedUsers.size()) {
        // Clear fields if "Select" is chosen
        ui->userNameLineEdit->clear();
        ui->nameLineEdit->clear();
        ui->surnameLineEdit->clear();
        ui->activeCheckBox->setChecked(false);
        return;
    }

    const auto& user = m_deactivatedUsers.at(index - 1); // -1 since we inserted "Select"

    ui->userNameLineEdit->setText(user.Username);
    ui->nameLineEdit->setText(user.Name);
    ui->surnameLineEdit->setText(user.Surname);
    ui->activeCheckBox->setChecked(user.IsActive);

    if (!user.Roles.isEmpty()) {
        const auto& role = user.Roles.first();
        int roleIndex = ui->rolesComboBox->findData(QVariant::fromValue(role));
        if (roleIndex != -1) {
            ui->rolesComboBox->setCurrentIndex(roleIndex);
        }
    }
}


Role UserMangerDialog::GetSelectedRole() const {
    if (ui->rolesComboBox->currentIndex() <= 0)
        return {}; // empty role
    return ui->rolesComboBox->currentData().value<Role>();
}

User UserMangerDialog::GetSelectedUser() const {
    if (ui->activeUsersComboBox->currentIndex() <= 0)
        return {}; // empty user
    return ui->activeUsersComboBox->currentData().value<User>();
}


void UserMangerDialog::onAddUserBtnClicked()
{
    QString username = ui->userNameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();

    QString messageBoxTitle = translator->getWarningMessage(AUTH_VALIDIATION_FAILED_TITLE_MSG);

    if (username.isEmpty()) {
        QString warningMessage = translator->getWarningMessage(AUTH_EMPTY_USER_NAME_WARNING_MSG);
        QMessageBox::warning(this, messageBoxTitle, warningMessage);
        return;
    }

    // Check for duplicate username in active users
    bool usernameExists = std::any_of(m_activeUsers.begin(), m_activeUsers.end(), [&](const User& u) {
        return u.Username.compare(username, Qt::CaseInsensitive) == 0;
    });

    // Check in deactivated users as well
    if (!usernameExists) {
        usernameExists = std::any_of(m_deactivatedUsers.begin(), m_deactivatedUsers.end(), [&](const User& u) {
            return u.Username.compare(username, Qt::CaseInsensitive) == 0;
        });
    }

    if (usernameExists) {
        QString warningMessage = translator->getWarningMessage(AUTH_USER_ALREADY_EXISTS_WARNING_MSG);
        QMessageBox::warning(this, messageBoxTitle, warningMessage);
        return;
    }

    int roleIndex = ui->rolesComboBox->currentIndex();
    if (roleIndex <= 0) {
        QString warningMessage = translator->getWarningMessage(AUTH_ROLE_NOT_SELECTED_WARNING_MSG);
        QMessageBox::warning(this, messageBoxTitle, warningMessage);
        return;
    }



    if (password.isEmpty() || confirmPassword.isEmpty()) {
        QString warningMessage = translator->getWarningMessage(AUTH_EMPTY_PASSWORD_WARNING_MSG);
        QMessageBox::warning(this, messageBoxTitle, warningMessage);
        return;
    }

    if (password != confirmPassword) {
        QString warningMessage = translator->getWarningMessage(AUTH_PASSWORD_NOT_MATCHING_WARNING_MSG);
        QMessageBox::warning(this, messageBoxTitle, warningMessage);
        return;
    }

    QRegularExpression specialCharRegex(R"([^A-Za-z0-9])");
    if (password.length() < 8 || !specialCharRegex.match(password).hasMatch()) {
        QString warningMessage = translator->getWarningMessage(AUTH_PASSWORD_COMPLEXITY_WARNING_MSG);
        QMessageBox::warning(this, messageBoxTitle, warningMessage);
        return;
    }

    User newUser;
    newUser.Username = username;
    newUser.Name = ui->nameLineEdit->text();
    newUser.Surname = ui->surnameLineEdit->text();
    newUser.IsActive = true;
    newUser.IsDeleted = false;

    // role assignment
    auto role = ui->rolesComboBox->currentData().value<Role>();
    newUser.Roles.append(role);

    emit AddUserRequested(newUser, password);
}
void UserMangerDialog::onUserCreated(const User& user)
{
    // Show success message
    QString infoMessage = translator->getInfoMessage(AUTH_USER_CREATION_SUCCEED_MSG).arg(user.Username);
    QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_CREATION_SUCCEED_TITLE_MSG);
    QMessageBox::information(this, messageBoxTitle, infoMessage);

    // Clear input fields
    ui->userNameLineEdit->clear();
    ui->passwordLineEdit->clear();
    ui->confirmPasswordLineEdit->clear();
    ui->nameLineEdit->clear();
    ui->surnameLineEdit->clear();

    // Optionally reset combo box and checkbox
    ui->rolesComboBox->setCurrentIndex(0);
    ui->activeCheckBox->setChecked(true);

    // Add new user to active users list
    m_activeUsers.append(user);
    ui->activeUsersComboBox->addItem(user.Username, QVariant::fromValue(user));
    UpdateActionButtonsState();

}
void UserMangerDialog::onUserCreationFailed(const User& user, QString& failReason)
{

    // Show success message
    QString infoMessage = translator->getErrorMessage(AUTH_USER_CREATION_FAILED_ERROR).arg(user.Username).arg(failReason);
    QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_CREATION_FAILED_TITLE_MSG);
    QMessageBox::information(this, messageBoxTitle, infoMessage);

    // Optionally reset combo box and checkbox
    ui->rolesComboBox->setCurrentIndex(0);
    ui->activeCheckBox->setChecked(true);

}


void UserMangerDialog::onUpdateUserBtnClicked()
{
    int index = ui->activeUsersComboBox->currentIndex();

    index--; // the first element is select, remove the index of that item.

    // 1. Check if a user is selected
    if (index < 0 || index >= m_activeUsers.size()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a user to update."));
        return;
    }

    // 2. Read fields
    QString newUsername = ui->userNameLineEdit->text().trimmed();
    QString name = ui->nameLineEdit->text().trimmed();
    QString surname = ui->surnameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text().trimmed();
    QString confirmPassword = ui->confirmPasswordLineEdit->text().trimmed();
    bool isActive = ui->activeCheckBox->isChecked();
    auto selectedRole = ui->rolesComboBox->currentData().value<Role>();

    // 3. Validate fields
    if (newUsername.isEmpty()) {

        QString infoMessage = translator->getWarningMessage(AUTH_EMPTY_USER_NAME_WARNING_MSG);
        QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_UPDATE_FAILED_TITLE_MSG);
        QMessageBox::warning(this, messageBoxTitle, infoMessage);
        return;
    }

    if (newUsername.isEmpty()) {

        QString infoMessage = translator->getWarningMessage(AUTH_EMPTY_USER_NAME_WARNING_MSG);
        QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_UPDATE_FAILED_TITLE_MSG);
        QMessageBox::warning(this, messageBoxTitle, infoMessage);
        return;
    }

    if (password.isEmpty() || confirmPassword.isEmpty()) {
        QString warningMessage = translator->getWarningMessage(AUTH_EMPTY_PASSWORD_WARNING_MSG);
        QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_UPDATE_FAILED_TITLE_MSG);
        QMessageBox::warning(this, messageBoxTitle, warningMessage);
        return;
    }

    if (password != confirmPassword) {
        QString warningMessage = translator->getWarningMessage(AUTH_PASSWORD_NOT_MATCHING_WARNING_MSG);
        QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_UPDATE_FAILED_TITLE_MSG);
        QMessageBox::warning(this, messageBoxTitle, warningMessage);
        return;
    }

    QRegularExpression specialCharRegex(R"([^A-Za-z0-9])");
    if (password.length() < 8 || !specialCharRegex.match(password).hasMatch()) {
        QString warningMessage = translator->getWarningMessage(AUTH_PASSWORD_COMPLEXITY_WARNING_MSG);
        QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_UPDATE_FAILED_TITLE_MSG);
        QMessageBox::warning(this, messageBoxTitle, warningMessage);
        return;
    }

    // 4. Build updated user object (preserve existing ID)
    User updatedUser = m_activeUsers[index];
    updatedUser.Username = newUsername;
    updatedUser.Name = name;
    updatedUser.Surname = surname;
    updatedUser.IsActive = isActive;
    updatedUser.Roles.clear();
    updatedUser.Roles.append(selectedRole);

    // 5. Emit signal to notify service layer
    emit UpdateUserRequested(updatedUser,password);

    // Optional: show feedback or close the dialog
}
void UserMangerDialog::onUserUpdated(const User& user)
{
    // Show success message
    QString infoMessage = translator->getInfoMessage(AUTH_USER_UPDATE_SUCCEED_MSG).arg(user.Username);
    QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_UPDATE_SUCCEED_TITLE_MSG);
    QMessageBox::information(this, messageBoxTitle, infoMessage);

    // Clear input fields
    ui->userNameLineEdit->clear();
    ui->passwordLineEdit->clear();
    ui->confirmPasswordLineEdit->clear();
    ui->nameLineEdit->clear();
    ui->surnameLineEdit->clear();
    ui->rolesComboBox->setCurrentIndex(0);
    ui->activeCheckBox->setChecked(true);

    // Find the existing user in the list and update it
    int indexToUpdate = -1;
    for (int i = 0; i < m_activeUsers.size(); ++i) {
        if (m_activeUsers[i].Id == user.Id) {
            indexToUpdate = i;
            break;
        }
    }

    if (indexToUpdate != -1) {
        m_activeUsers[indexToUpdate] = user;

        indexToUpdate++; // to compensate the first element with index 0.

        // Update combo box item (both label and data)
        ui->activeUsersComboBox->setItemText(indexToUpdate, user.Username);
        ui->activeUsersComboBox->setItemData(indexToUpdate, QVariant::fromValue(user));
    } else {
        // If not found (e.g. was inactive before), just add it
        m_activeUsers.append(user);
        ui->activeUsersComboBox->addItem(user.Username, QVariant::fromValue(user));
    }
}
void UserMangerDialog::onUserUpdateFailed(const User& user, QString& failReason)
{
    // Show success message
    QString infoMessage = translator->getErrorMessage(AUTH_USER_UPDATE_FAILED_ERROR).arg(user.Username).arg(failReason);
    QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_UPDATE_FAILED_TITLE_MSG);
    QMessageBox::information(this, messageBoxTitle, infoMessage);
}


void UserMangerDialog::onDeleteUserBtnClicked()
{
    int index = ui->activeUsersComboBox->currentIndex();

    index--; // the first element is select, remove the index of that item.

    // 1. Check if a user is selected
    if (index < 0 || index >= m_activeUsers.size()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a user to update."));
        return;
    }

    User selectedUser = m_activeUsers[index];
    emit DeleteUserRequested(selectedUser);

}
void UserMangerDialog::onUserDeleted(const User& user)
{
    // Show success message
    QString infoMessage = translator->getInfoMessage(AUTH_USER_DELETE_SUCCEED_MSG).arg(user.Username);
    QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_DELETE_SUCCEED_TITLE_MSG);
    QMessageBox::information(this, messageBoxTitle, infoMessage);

    // Find user in m_activeUsers and remove
    int localIndex = -1;
    for (int i = 0; i < m_activeUsers.size(); ++i) {
        if (m_activeUsers[i].Id == user.Id) {
            localIndex = i;
            break;
        }
    }

    if (localIndex != -1) {
        m_activeUsers.removeAt(localIndex);
    }

    // Find and remove from combo box using user ID
    for (int i = 1; i < ui->activeUsersComboBox->count(); ++i) { // skip index 0 ("Select one")
        auto comboData = ui->activeUsersComboBox->itemData(i).value<User>();
        if (comboData.Id == user.Id) {
            ui->activeUsersComboBox->removeItem(i);
            break;
        }
    }

    UpdateActionButtonsState();

    // Insert into deactivated users list and combo box
    m_deactivatedUsers.append(user);
    ui->deactivatedUsersListComboBox->addItem(user.Username, QVariant::fromValue(user));


}
void UserMangerDialog::onUserDeletionFailed(const User& user,QString& failReason)
{
    QString infoMessage = translator->getErrorMessage(AUTH_USER_DELETE_FAILED_ERROR).arg(user.Username).arg(failReason);
    QString messageBoxTitle = translator->getInfoMessage(AUTH_USER_DELETE_SUCCEED_TITLE_MSG);
    QMessageBox::information(this, messageBoxTitle, infoMessage);

}

void UserMangerDialog::UpdateActionButtonsState()
{
    bool hasActiveUsers = m_activeUsers.size() > 0;

    ui->updateUserBtn->setEnabled(hasActiveUsers);
    ui->deleteUserBtn->setEnabled(hasActiveUsers);
    ui->updateUserBtn->update();
    ui->deleteUserBtn->update();
}


void UserMangerDialog::onOkBtnClicked()
{
    accept(); // or any custom logic
}

UserMangerDialog::~UserMangerDialog()
{
    delete ui;
}


void UserMangerDialog::initStyles()
{
    // ----- minimal styles -----
    const char* lineCss =
        "QLineEdit {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background:rgb(74,74,74);"
        "  min-height:20px;"
        "  padding:2px 6px;"
        "   width:26px; "
        "}"
        "QLineEdit:focus {"
        "  border-color: rgb(160,160,160);"
        "}"
        /* invalid IP cue (red border when property invalid=true) */
        "QLineEdit#hostIPLineEdit[invalid=\"true\"] {"
        "  border:1px solid rgb(200,80,80);"
        "}";

    const char* spinCss =
        "QSpinBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QSpinBox:focus { border-color: rgb(160,160,160); }";

    const char* listCss =
        "QListView {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"   // contrast with the container bg
        "}";

    const char* buttonCss =
        "QPushButton {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background:rgb(74,74,74);"
        "  min-height:20px;"
        "  padding:4px 12px;"
        "  color: rgb(230,230,230);"
        "}"
        "QPushButton:hover {"
        "  border-color: rgb(160,160,160);"
        "  background:rgb(92,92,92);"
        "}"
        "QPushButton:pressed {"
        "  background:rgb(64,64,64);"
        "}"
        "QPushButton:focus {"
        "  border-color: rgb(180,180,180);"
        "}"
        "QPushButton:disabled {"
        "  border-color: rgb(80,80,80);"
        "  background:rgb(60,60,60);"
        "  color: rgb(130,130,130);"
        "}";

    const char* checkCss =
        "QCheckBox { spacing: 8px; }"
        "QCheckBox::indicator { width:16px; height:16px; }"
        "QCheckBox::indicator:unchecked {"
        "  border:1px solid rgb(120,120,120); background:rgb(74,74,74); border-radius:3px; }"
        "QCheckBox::indicator:unchecked:hover { border-color: rgb(160,160,160); }"
        "QCheckBox::indicator:checked {"
        "  border:1px solid rgb(160,160,160); background:rgb(54,69,79); border-radius:3px; }";

    const char* comboCss =
        "QComboBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 28px 2px 6px;" /* room for arrow */
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

    const char* groupCss =
        "QGroupBox {"
        "  border:1px solid rgb(120,120,120); border-radius:6px; margin-top:18px; }"
        "QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 6px; }";

    const char* tableCss =
        "QTableView {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "  gridline-color: transparent;"
        /* Selection palette (applies to the whole view) */
        "  selection-background-color: rgb(45,110,190);"
        "  selection-color: rgb(235,235,235);"
        "}"
        /* Remove the cell focus rect so only the row highlight shows */
        "QTableView::item:focus { outline: none; }"
        /* Optional: no per-item override, let the palette above drive it */
        /* Header */
        "QHeaderView::section {"
        "  background-color: rgb(64,64,64);"
        "  color: rgb(220,220,220);"
        "  padding: 4px 8px;"
        "  border: 1px solid rgb(100,100,100);"
        "  font-weight: bold;"
        "}"
        "QHeaderView { border: none; }";

    ui->userNameLineEdit->setStyleSheet(lineCss);
    ui->nameLineEdit->setStyleSheet(lineCss);
    ui->surnameLineEdit->setStyleSheet(lineCss);
    ui->passwordLineEdit->setStyleSheet(lineCss);
    ui->confirmPasswordLineEdit->setStyleSheet(lineCss);
    ui->rolesComboBox->setStyleSheet(comboCss);
    ui->activeUsersComboBox->setStyleSheet(comboCss);
    ui->deactivatedUsersListComboBox->setStyleSheet(comboCss);
    ui->activeCheckBox->setStyleSheet(checkCss);
    
    ui->addUserBtn->setStyleSheet(buttonCss);
    ui->updateUserBtn->setStyleSheet(buttonCss);
    ui->deleteUserBtn->setStyleSheet(buttonCss);
    ui->okBtn->setStyleSheet(buttonCss);

}
