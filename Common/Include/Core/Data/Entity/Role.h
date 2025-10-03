#ifndef ROLE_H
#define ROLE_H

#include <QString>
#include <QMetaType>


namespace Etrek::Core::Data::Entity {

    class Role {
    public:
        int Id = -1;
        QString Name;

        Role() = default;
        Role(int id, const QString& name) : Id(id), Name(name) {}

        // Implement IRole interface
        int getId() const  { return Id; }
        QString getName() const  { return Name; }

        bool operator==(const Role& other) const  {
            return Id == other.getId();
        }
    };

}

Q_DECLARE_METATYPE(Etrek::Core::Data::Entity::Role)

#endif // ROLE_H
