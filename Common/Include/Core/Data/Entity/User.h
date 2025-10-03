// User.h
#ifndef USERENTITYMODEL_H
#define USERENTITYMODEL_H

#include <QString>
#include <QDateTime>
#include <QVector>
#include <QMetaType>
#include "Role.h"

namespace Etrek::Core::Data::Entity {

    class User{
    public:
        int Id = -1;
        QString Username;
        QString Name;
        QString Surname;
        QString PasswordHash;
        bool IsActive = true;
        bool IsDeleted = false;
        QDateTime CreateDate;
        QDateTime UpdateDate;
        QVector<Role> Roles;

        User() = default;


        int getId() const { return Id; }
        QString getUsername() const { return Username; }
        QString getName() const  { return Name; }
        QString getSurname() const  { return Surname; }
        QString getPasswordHash() const  { return PasswordHash; }
        bool isActive() const  { return IsActive; }
        bool isDeleted() const  { return IsDeleted; }
        QDateTime getCreateDate() const  { return CreateDate; }
        QDateTime getUpdateDate() const  { return UpdateDate; }

        QVector<Role*> getRoles() const  {
            QVector<Role*> result;
            result.reserve(Roles.size());
            for (const auto& role : Roles) {
                result.push_back(const_cast<Role*>(&role)); // safe if Role implements IRole
            }
            return result;
        }
    };

}

Q_DECLARE_METATYPE(Etrek::Core::Data::Entity::User)
#endif // USERENTITYMODEL_H
