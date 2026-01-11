#ifndef ETREK_APPLICATION_STRATEGY_EXAMINATIONMODESTRATEGYFACTORY_H
#define ETREK_APPLICATION_STRATEGY_EXAMINATIONMODESTRATEGYFACTORY_H

#include "Examination/IExaminationModeStrategy.h"
#include <memory>

namespace Etrek::Application::Strategy {

/**
 * @class ExaminationModeStrategyFactory
 * @brief Factory for creating examination mode strategy instances.
 *
 * This factory creates the appropriate strategy implementation based
 * on the requested examination mode. It encapsulates the strategy
 * creation logic and allows for easy extension with new modes.
 */
class ExaminationModeStrategyFactory {
public:
    /**
     * @brief Creates a strategy for the specified examination mode.
     * @param mode The examination mode.
     * @return Unique pointer to the strategy implementation.
     */
    static std::unique_ptr<Etrek::Examination::IExaminationModeStrategy>
        createStrategy(Etrek::Examination::ExaminationMode mode);

    /**
     * @brief Creates the default strategy (CLINICAL mode).
     * @return Unique pointer to ClinicalExaminationStrategy.
     */
    static std::unique_ptr<Etrek::Examination::IExaminationModeStrategy>
        createDefaultStrategy();

private:
    ExaminationModeStrategyFactory() = delete;
};

} // namespace Etrek::Application::Strategy

#endif // ETREK_APPLICATION_STRATEGY_EXAMINATIONMODESTRATEGYFACTORY_H
