#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <optional>
#include <utility>
#include "User.h"

namespace Ui {
class LoginDialog;
}

using namespace Etrek::Core::Data;
using namespace Etrek::Core::Data::Entity;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(const QVector<Entity::User>& users, QWidget *parent = nullptr);
    std::optional<std::pair<Entity::User, QString>>getLoginCredentials() const;
    void clearPassword();
    ~LoginDialog();


private:
    Ui::LoginDialog *ui;
    const QVector<Entity::User>& m_users;
};

#endif // LOGINDIALOG_H
