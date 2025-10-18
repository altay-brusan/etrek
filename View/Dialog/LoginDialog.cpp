#include <QStringListModel>

#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "User.h"

using namespace Etrek::Core::Data::Entity;


LoginDialog::LoginDialog(const QVector<User>& users, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), m_users(users)
{
    ui->setupUi(this);

    const char* lineCss =
        "QLineEdit#passwordLineEdit {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "}";
	ui->passwordLineEdit->setStyleSheet(lineCss);

    const char* comboCss =
        "QComboBox#userNameComboBox {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"
        "}";

	ui->userNameComboBox->setStyleSheet(comboCss);

    // Populate combo box with user names and full user data
    for (const auto& user : m_users) {
        ui->userNameComboBox->addItem(user.Username, QVariant::fromValue(user));
    }
}

std::optional<std::pair<User, QString>> LoginDialog::getLoginCredentials() const
{
    const int index = ui->userNameComboBox->currentIndex();
    if (index < 0)                 // nothing selected
        return std::nullopt;

    User  user = ui->userNameComboBox->currentData().value<User>();
    QString password = ui->passwordLineEdit->text();

    return std::make_pair(std::move(user), std::move(password));
}

void LoginDialog::clearPassword()
{
    ui->passwordLineEdit->clear();
}


LoginDialog::~LoginDialog()
{
    delete ui;
}

