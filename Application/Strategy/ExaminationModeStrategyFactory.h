/**
 * @file ExaminationModeStrategyFactory.h
 * @brief Factory for creating examination mode strategy instances.
 *
 * @details Implements the Factory pattern for examination mode strategies,
 *          creating the appropriate strategy based on the requested mode.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see IExaminationModeStrategy
 * @see ExaminationMode
 * @see ClinicalExaminationStrategy
 * @see DemoExaminationStrategy
 * @see CalibrationExaminationStrategy
 * @see TestExaminationStrategy
 */

#ifndef ETREK_APPLICATION_STRATEGY_EXAMINATIONMODESTRATEGYFACTORY_H
#define ETREK_APPLICATION_STRATEGY_EXAMINATIONMODESTRATEGYFACTORY_H

#include "Examination/IExaminationModeStrategy.h"
#include <memory>

namespace Etrek::Application::Strategy {

/**
 * @class ExaminationModeStrategyFactory
 * @brief Factory for creating examination mode strategy instances.
 *
 * @details This factory creates the appropriate strategy implementation based
 *          on the requested examination mode. It encapsulates the strategy
 *          creation logic and allows for easy extension with new modes.
 *
 *          Supported modes:
 *          - CLINICAL: ClinicalExaminationStrategy (full DICOM workflow)
 *          - DEMO: DemoExaminationStrategy (sample images, no persistence)
 *          - CALIBRATION: CalibrationExaminationStrategy (equipment calibration)
 *          - TEST: TestExaminationStrategy (QA testing)
 *
 * @par Usage:
 * @code
 * auto strategy = ExaminationModeStrategyFactory::createStrategy(ExaminationMode::CLINICAL);
 * strategy->onExaminationStart();
 * @endcode
 *
 * @see IExaminationModeStrategy
 * @see ExaminationMode
 *
 * @ingroup Examination
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
