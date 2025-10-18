#ifndef USERMANGERDIALOG_H
#define USERMANGERDIALOG_H


#include <QDialog>
#include "TranslationProvider.h"
#include "User.h"
#include "Role.h"

namespace Ui {
class UserMangerDialog;
}

class UserMangerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserMangerDialog(const QVector<Etrek::Core::Data::Entity::User>& activeUsers,
                              const QVector<Etrek::Core::Data::Entity::User>& deactivatedUsers,
                              const QVector<Etrek::Core::Data::Entity::Role>& roles,
                              Etrek::Core::Globalization::TranslationProvider* translationProvider,
                              QWidget *parent = nullptr);
    void DisplayManager();
    ~UserMangerDialog();
    Etrek::Core::Data::Entity::Role GetSelectedRole() const;
    Etrek::Core::Data::Entity::User GetSelectedUser() const;

signals:
    void AddUserRequested(Etrek::Core::Data::Entity::User& user, const QString& password);
    void UpdateUserRequested(Etrek::Core::Data::Entity::User& user, const QString& password);
    void DeleteUserRequested(Etrek::Core::Data::Entity::User& user);

public slots:
    void onUserCreated(const Etrek::Core::Data::Entity::User& user);
    void onUserCreationFailed(const Etrek::Core::Data::Entity::User& user,QString& failReason);
    void onUserUpdated(const Etrek::Core::Data::Entity::User& user);
    void onUserUpdateFailed(const Etrek::Core::Data::Entity::User& user,QString& failReason);
    void onUserDeleted(const Etrek::Core::Data::Entity::User& user);
    void onUserDeletionFailed(const Etrek::Core::Data::Entity::User& user,QString& failReason);

private slots:
    void onAddUserBtnClicked();
    void onUpdateUserBtnClicked();
    void onDeleteUserBtnClicked();
    void onOkBtnClicked();
    void onActiveUserChanged(int index);
    void onDeactiveUserChanged(int index);




private:

    void UpdateActionButtonsState();
    void initStyles();

    Ui::UserMangerDialog *ui;
    QVector<Etrek::Core::Data::Entity::User> m_activeUsers;
    QVector<Etrek::Core::Data::Entity::User> m_deactivatedUsers;
    QVector<Etrek::Core::Data::Entity::Role> m_roles;
    Etrek::Core::Globalization::TranslationProvider* translator;
};

#endif // USERMANGERDIALOG_H
