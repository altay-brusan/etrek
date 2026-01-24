/**
 * @file IExaminationModeStrategy.h
 * @brief Interface for examination mode-specific behavior strategies.
 *
 * @details Defines the Strategy pattern interface for handling different
 *          examination modes (Clinical, Demo, Calibration, Test). Each mode
 *          implements this interface to provide mode-specific behavior for
 *          image persistence, DICOM creation, and workflow management.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see ExaminationMode
 * @see ClinicalExaminationStrategy
 * @see DemoExaminationStrategy
 * @see CalibrationExaminationStrategy
 * @see TestExaminationStrategy
 */

#ifndef ETREK_EXAMINATION_IEXAMINATIONMODESTRATEGY_H
#define ETREK_EXAMINATION_IEXAMINATIONMODESTRATEGY_H

#include "ExaminationMode.h"
#include <QString>
#include <QByteArray>
#include <memory>

namespace Etrek::Examination {

/**
 * @interface IExaminationModeStrategy
 * @brief Strategy interface for examination mode-specific behavior.
 *
 * This interface defines hooks that are called at key points during
 * an examination workflow. Each examination mode (Clinical, Demo,
 * Calibration, Test) implements this interface with mode-specific logic.
 *
 * The delegate calls these hooks instead of hardcoding mode-specific behavior,
 * allowing clean separation between workflow orchestration and mode logic.
 */
class IExaminationModeStrategy {
public:
    virtual ~IExaminationModeStrategy() = default;

    /**
     * @brief Returns the examination mode this strategy handles.
     * @return The ExaminationMode enum value.
     */
    virtual ExaminationMode mode() const = 0;

    /**
     * @brief Returns a display name for this mode.
     * @return Human-readable mode name.
     */
    virtual QString modeName() const = 0;

    // --- Lifecycle Hooks ---

    /**
     * @brief Called when the examination session starts.
     *
     * Mode-specific initialization logic. For example:
     * - CLINICAL: Validate patient data, prepare DICOM objects
     * - DEMO: Load sample image paths
     * - CALIBRATION: Initialize calibration parameters
     * - TEST: Set up test conditions
     */
    virtual void onExaminationStart() = 0;

    /**
     * @brief Called before an exposure is initiated.
     * @return true if the exposure should proceed, false to cancel.
     *
     * Allows mode-specific validation before triggering an exposure.
     * - CLINICAL: Verify patient positioning, technique parameters
     * - DEMO: Always returns true (no real exposure)
     * - CALIBRATION: Check calibration prerequisites
     * - TEST: Verify test conditions
     */
    virtual bool onBeforeExposure() = 0;

    /**
     * @brief Called after an exposure completes (X-ray fired).
     * @param success true if the exposure was successful.
     *
     * Handle post-exposure logic:
     * - CLINICAL: Log exposure, update procedure status
     * - DEMO: No-op or simulate exposure
     * - CALIBRATION: Record calibration data
     * - TEST: Record test results
     */
    virtual void onAfterExposure(bool success) = 0;

    /**
     * @brief Called when an image is received from the detector.
     * @param imageData The raw image data.
     * @param imageId A unique identifier for this image.
     *
     * Process the received image according to mode:
     * - CLINICAL: Apply processing, create DICOM, queue for storage
     * - DEMO: Load and display sample image instead
     * - CALIBRATION: Process for calibration analysis
     * - TEST: Validate image quality metrics
     */
    virtual void onImageReceived(const QByteArray& imageData, const QString& imageId) = 0;

    /**
     * @brief Called when the examination session ends.
     *
     * Cleanup and finalization:
     * - CLINICAL: Finalize DICOM objects, send to PACS, update MWL status
     * - DEMO: Clear sample images
     * - CALIBRATION: Save calibration results
     * - TEST: Generate test report
     */
    virtual void onExaminationComplete() = 0;

    // --- Persistence Control ---

    /**
     * @brief Determines if images should be persisted to storage.
     * @return true if images should be saved to database/PACS.
     */
    virtual bool shouldPersistImages() const = 0;

    /**
     * @brief Determines if DICOM objects should be created.
     * @return true if DICOM Study/Series/Images should be created.
     */
    virtual bool shouldCreateDicom() const = 0;

    /**
     * @brief Determines if MWL status should be updated.
     * @return true if the worklist entry status should be updated.
     */
    virtual bool shouldUpdateWorklistStatus() const = 0;

    /**
     * @brief Determines if images should be sent to PACS.
     * @return true if images should be transmitted to PACS nodes.
     */
    virtual bool shouldSendToPacs() const = 0;
};

/**
 * @brief Factory function type for creating strategy instances.
 */
using StrategyFactory = std::unique_ptr<IExaminationModeStrategy>(*)();

} // namespace Etrek::Examination

#endif // ETREK_EXAMINATION_IEXAMINATIONMODESTRATEGY_H
