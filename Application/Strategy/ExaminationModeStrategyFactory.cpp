#include "ExaminationModeStrategyFactory.h"
#include "ClinicalExaminationStrategy.h"
#include "DemoExaminationStrategy.h"
#include "CalibrationExaminationStrategy.h"
#include "TestExaminationStrategy.h"

namespace Etrek::Application::Strategy {

using Etrek::Examination::ExaminationMode;
using Etrek::Examination::IExaminationModeStrategy;

std::unique_ptr<IExaminationModeStrategy>
ExaminationModeStrategyFactory::createStrategy(ExaminationMode mode)
{
    switch (mode) {
        case ExaminationMode::CLINICAL:
            return std::make_unique<ClinicalExaminationStrategy>();

        case ExaminationMode::DEMO:
            return std::make_unique<DemoExaminationStrategy>();

        case ExaminationMode::CALIBRATION:
            return std::make_unique<CalibrationExaminationStrategy>();

        case ExaminationMode::TEST:
            return std::make_unique<TestExaminationStrategy>();

        default:
            // Default to clinical mode for safety
            return std::make_unique<ClinicalExaminationStrategy>();
    }
}

std::unique_ptr<IExaminationModeStrategy>
ExaminationModeStrategyFactory::createDefaultStrategy()
{
    return std::make_unique<ClinicalExaminationStrategy>();
}

} // namespace Etrek::Application::Strategy
