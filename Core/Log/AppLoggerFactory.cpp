#include "AppLoggerFactory.h"

namespace Etrek::Core::Log {

	using Etrek::Core::Globalization::TranslationProvider;

    AppLoggerFactory::AppLoggerFactory(LoggerProvider& provider, glb::TranslationProvider* translator)
		:m_provider(provider), translator(translator)
    {
    }

    std::shared_ptr<AppLogger> AppLoggerFactory::CreateLogger(const QString& serviceName) {
        return std::make_shared<AppLogger>(serviceName, m_provider, translator);
    }
}
