#ifndef DICOMREPOSITORY_H
#define DICOMREPOSITORY_H

#include <QSqlDatabase>
#include <QVector>
#include <memory>

#include "Result.h"
#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "AppLogger.h"
#include "Study.h"

namespace Etrek::Dicom::Repository {

    class DicomRepository
    {
    public:
        explicit DicomRepository(std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
            Etrek::Core::Globalization::TranslationProvider* tr = nullptr);

        Etrek::Specification::Result<QVector<Etrek::Core::Data::Entity::Study>>
            getStudiesByAdmissionId(const QString& admissionId) const;

        ~DicomRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        Etrek::Core::Globalization::TranslationProvider* translator = nullptr;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };
}

#endif // DICOMREPOSITORY_H

