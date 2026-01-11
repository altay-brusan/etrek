/**
 * @file TestExaminationStrategy.h
 * @brief Strategy implementation for device verification and testing mode.
 *
 * @details Implements the IExaminationModeStrategy interface for QA testing
 *          and device verification procedures.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see IExaminationModeStrategy
 * @see ExaminationMode
 * @see ExaminationModeStrategyFactory
 */

#ifndef ETREK_APPLICATION_STRATEGY_TESTEXAMINATIONSTRATEGY_H
#define ETREK_APPLICATION_STRATEGY_TESTEXAMINATIONSTRATEGY_H

#include "Examination/IExaminationModeStrategy.h"
#include "AppLogger.h"
#include <memory>

namespace Etrek::Application::Strategy {

/**
 * @class TestExaminationStrategy
 * @brief Strategy for device verification and quality assurance testing.
 *
 * @details This mode is used for internal testing and device verification:
 *          - Daily quality assurance (QA) tests
 *          - Device functionality verification
 *          - System integration testing
 *          - Service diagnostics
 *
 *          Key characteristics:
 *          - Test images persisted to separate QA database
 *          - Pass/fail tracking for test sequences
 *          - No patient DICOM objects created
 *          - No PACS transmission
 *          - No worklist status updates
 *          - Complete isolation from patient data
 *
 *          Test results are logged and can be reviewed for compliance
 *          with regulatory QA requirements.
 *
 * @see IExaminationModeStrategy
 * @see ClinicalExaminationStrategy
 * @see CalibrationExaminationStrategy
 *
 * @ingroup Examination
 */
class TestExaminationStrategy : public Etrek::Examination::IExaminationModeStrategy {
public:
    TestExaminationStrategy();
    ~TestExaminationStrategy() override = default;

    // --- IExaminationModeStrategy Implementation ---
    Etrek::Examination::ExaminationMode mode() const override;
    QString modeName() const override;

    void onExaminationStart() override;
    bool onBeforeExposure() override;
    void onAfterExposure(bool success) override;
    void onImageReceived(const QByteArray& imageData, const QString& imageId) override;
    void onExaminationComplete() override;

    bool shouldPersistImages() const override;
    bool shouldCreateDicom() const override;
    bool shouldUpdateWorklistStatus() const override;
    bool shouldSendToPacs() const override;

private:
    std::shared_ptr<Etrek::Core::Log::AppLogger> m_logger;
    int m_testExposureCount = 0;
    bool m_allTestsPassed = true;
};

} // namespace Etrek::Application::Strategy

#endif // ETREK_APPLICATION_STRATEGY_TESTEXAMINATIONSTRATEGY_H
