#include "AppLoggerFactory.h"

namespace Etrek::Core::Log {
    AppLoggerFactory::AppLoggerFactory(LoggerProvider& provider, TranslationProvider* translator)
		:m_provider(provider), translator(translator)
    {
    }

    std::shared_ptr<AppLogger> AppLoggerFactory::CreateLogger(const QString& serviceName) {
        return std::make_shared<AppLogger>(serviceName, m_provider, translator);
    }
}
