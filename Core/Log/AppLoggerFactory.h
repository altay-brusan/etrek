#ifndef APPLOGGERFACTORY_H
#define APPLOGGERFACTORY_H

#include "LoggerProvider.h"
#include "AppLogger.h"
#include "TranslationProvider.h"

namespace  Etrek::Core::Log {

	namespace glb = Etrek::Core::Globalization;
    /**
     * @class AppLoggerFactory
     * @brief Factory for creating application-specific loggers.
     *
     * AppLoggerFactory encapsulates the creation logic for AppLogger instances.
     * It allows services and components to obtain loggers with consistent configuration
     * and dependency injection of the underlying LoggerProvider.
     *
     * The factory pattern is used to decouple logger creation from usage, enabling
     * future extension (e.g., different logger types or providers).
     */
    class AppLoggerFactory
    {
    public:
        /**
         * @brief Constructs the factory with a reference to a LoggerProvider.
         * @param provider Reference to the logger provider.
         */
        explicit AppLoggerFactory(LoggerProvider& provider,glb::TranslationProvider* translator);

        /**
         * @brief Creates an AppLogger for the specified service.
         * @param serviceName Name of the service.
         * @return Shared pointer to the AppLogger instance.
         */
        std::shared_ptr<AppLogger> CreateLogger(const QString& serviceName);

    private:
        LoggerProvider& m_provider;
		glb::TranslationProvider* translator;

    };

}

#endif // APPLOGGERFACTORY_H
