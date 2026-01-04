#ifndef ETREK_APPLICATION_STRATEGY_CALIBRATIONEXAMINATIONSTRATEGY_H
#define ETREK_APPLICATION_STRATEGY_CALIBRATIONEXAMINATIONSTRATEGY_H

#include "IExaminationModeStrategy.h"
#include "AppLogger.h"
#include <memory>

namespace Etrek::Application::Strategy {

/**
 * @class CalibrationExaminationStrategy
 * @brief Strategy for detector and generator calibration mode.
 *
 * This mode is used for equipment calibration procedures such as:
 * - Flat-field (gain) calibration
 * - Dark-field (offset) calibration
 * - Defect pixel mapping
 * - Generator output verification
 *
 * Calibration images may be persisted locally for analysis but are
 * not sent to PACS or associated with patient records.
 */
class CalibrationExaminationStrategy : public Etrek::Examination::IExaminationModeStrategy {
public:
    CalibrationExaminationStrategy();
    ~CalibrationExaminationStrategy() override = default;

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
    int m_calibrationFrameCount = 0;
};

} // namespace Etrek::Application::Strategy

#endif // ETREK_APPLICATION_STRATEGY_CALIBRATIONEXAMINATIONSTRATEGY_H
