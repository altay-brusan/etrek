#ifndef ETREK_APPLICATION_STRATEGY_TESTEXAMINATIONSTRATEGY_H
#define ETREK_APPLICATION_STRATEGY_TESTEXAMINATIONSTRATEGY_H

#include "Examination/IExaminationModeStrategy.h"
#include "AppLogger.h"
#include <memory>

namespace Etrek::Application::Strategy {

/**
 * @class TestExaminationStrategy
 * @brief Strategy for device verification and testing mode.
 *
 * This mode is used for internal testing and device verification:
 * - Daily quality assurance (QA) tests
 * - Device functionality verification
 * - System integration testing
 * - Service diagnostics
 *
 * Test images may be persisted to a separate test database for
 * QA record keeping but are never mixed with patient data.
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
