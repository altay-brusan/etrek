#ifndef IFILELOGGERSETTING_H
#define IFILELOGGERSETTING_H

#include <QObject>
#include <QString>
#include <QDateTime>

namespace Etrek::Core::Data::Model
{
    class IFileLoggerSetting
    {
    public:
        
        virtual QString getLogDirectory() const= 0;
	    virtual size_t getFileSize() const = 0;
        virtual size_t getFileCount() const = 0;
            
    };
}
#endif // IFILELOGGERSETTING_H
