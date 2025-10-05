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

        bool operator==(const User& other) const noexcept { return Id == other.Id; }
        bool operator!=(const User& other) const noexcept { return !(*this == other); }

    };

}

Q_DECLARE_METATYPE(Etrek::Core::Data::Entity::User)
#endif // USERENTITYMODEL_H
