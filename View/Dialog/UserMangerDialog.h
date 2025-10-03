#ifndef USERMANGERDIALOG_H
#define USERMANGERDIALOG_H


#include <QDialog>
#include "TranslationProvider.h"
#include "User.h"
#include "Role.h"

namespace Ui {
class UserMangerDialog;
}

using namespace Etrek::Core::Data;
using namespace Etrek::Core::Globalization;
class UserMangerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserMangerDialog(const QVector<Entity::User>& activeUsers,
                              const QVector<Entity::User>& deactivatedUsers,
                              const QVector<Entity::Role>& roles,
                              TranslationProvider* translationProvider,
                              QWidget *parent = nullptr);
    void DisplayManager();
    ~UserMangerDialog();
    Entity::Role GetSelectedRole() const;
    Entity::User GetSelectedUser() const;

signals:
    void AddUserRequested(Entity::User& user, const QString& password);
    void UpdateUserRequested(Entity::User& user, const QString& password);
    void DeleteUserRequested(Entity::User& user);

public slots:
    void onUserCreated(const Entity::User& user);
    void onUserCreationFailed(const Entity::User& user,QString& failReason);
    void onUserUpdated(const Entity::User& user);
    void onUserUpdateFailed(const Entity::User& user,QString& failReason);
    void onUserDeleted(const Entity::User& user);
    void onUserDeletionFailed(const Entity::User& user,QString& failReason);

private slots:
    void onAddUserBtnClicked();
    void onUpdateUserBtnClicked();
    void onDeleteUserBtnClicked();
    void onOkBtnClicked();
    void onActiveUserChanged(int index);
    void onDeactiveUserChanged(int index);




private:

    void UpdateActionButtonsState();

    Ui::UserMangerDialog *ui;
    QVector<Entity::User> m_activeUsers;
    QVector<Entity::User> m_deactivatedUsers;
    QVector<Entity::Role> m_roles;
    TranslationProvider* translator;
};

#endif // USERMANGERDIALOG_H
