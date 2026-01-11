/**
 * @file DemoExaminationStrategy.h
 * @brief Strategy implementation for demonstration/exhibition mode.
 *
 * @details Implements the IExaminationModeStrategy interface for demo mode
 *          which displays sample images without real X-ray acquisition.
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

#ifndef ETREK_APPLICATION_STRATEGY_DEMOEXAMINATIONSTRATEGY_H
#define ETREK_APPLICATION_STRATEGY_DEMOEXAMINATIONSTRATEGY_H

#include "Examination/IExaminationModeStrategy.h"
#include "AppLogger.h"
#include <memory>
#include <QStringList>

namespace Etrek::Application::Strategy {

/**
 * @class DemoExaminationStrategy
 * @brief Strategy for demonstration and exhibition mode.
 *
 * @details This mode is used for exhibitions, sales demos, and training.
 *          Instead of performing real X-ray acquisitions, it loads and
 *          displays pre-captured sample images from disk.
 *
 *          Key characteristics:
 *          - No real X-ray exposure
 *          - Loads sample images from configured directory
 *          - No database persistence
 *          - No DICOM object creation
 *          - No PACS transmission
 *          - No worklist status updates
 *
 *          This mode is safe to use without a patient and without
 *          X-ray equipment being operational.
 *
 * @see IExaminationModeStrategy
 * @see ClinicalExaminationStrategy
 * @see CalibrationExaminationStrategy
 *
 * @ingroup Examination
 */
class DemoExaminationStrategy : public Etrek::Examination::IExaminationModeStrategy {
public:
    DemoExaminationStrategy();
    ~DemoExaminationStrategy() override = default;

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

    /**
     * @brief Gets the next sample image path for demo display.
     * @return Path to a sample image file.
     */
    QString getNextSampleImagePath();

private:
    void loadSampleImagePaths();

    std::shared_ptr<Etrek::Core::Log::AppLogger> m_logger;
    QStringList m_sampleImagePaths;
    int m_currentImageIndex = 0;
};

} // namespace Etrek::Application::Strategy

#endif // ETREK_APPLICATION_STRATEGY_DEMOEXAMINATIONSTRATEGY_H
