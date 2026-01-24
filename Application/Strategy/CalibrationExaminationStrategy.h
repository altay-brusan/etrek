/**
 * @file CalibrationExaminationStrategy.h
 * @brief Strategy implementation for equipment calibration mode.
 *
 * @details Implements the IExaminationModeStrategy interface for detector
 *          and generator calibration procedures.
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

#ifndef ETREK_APPLICATION_STRATEGY_CALIBRATIONEXAMINATIONSTRATEGY_H
#define ETREK_APPLICATION_STRATEGY_CALIBRATIONEXAMINATIONSTRATEGY_H

#include "Examination/IExaminationModeStrategy.h"
#include "AppLogger.h"
#include <memory>

namespace Etrek::Application::Strategy {

/**
 * @class CalibrationExaminationStrategy
 * @brief Strategy for detector and generator calibration mode.
 *
 * @details This mode is used for equipment calibration procedures including:
 *          - Flat-field (gain) calibration
 *          - Dark-field (offset) calibration
 *          - Defect pixel mapping
 *          - Generator output verification
 *
 *          Key characteristics:
 *          - Local persistence of calibration images for analysis
 *          - No DICOM patient objects created
 *          - No PACS transmission
 *          - No worklist status updates
 *          - Separate calibration data storage
 *
 *          Calibration data is stored locally and used by the detector
 *          and image processing systems to ensure optimal image quality.
 *
 * @see IExaminationModeStrategy
 * @see ClinicalExaminationStrategy
 * @see TestExaminationStrategy
 *
 * @ingroup Examination
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
