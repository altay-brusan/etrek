/**
 * @file ClinicalExaminationStrategy.h
 * @brief Strategy implementation for clinical patient examination mode.
 *
 * @details Implements the IExaminationModeStrategy interface for standard
 *          clinical patient examinations with full DICOM workflow.
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

#ifndef ETREK_APPLICATION_STRATEGY_CLINICALEXAMINATIONSTRATEGY_H
#define ETREK_APPLICATION_STRATEGY_CLINICALEXAMINATIONSTRATEGY_H

#include "Examination/IExaminationModeStrategy.h"
#include "AppLogger.h"
#include <memory>

/**
 * @namespace Etrek::Application::Strategy
 * @brief Contains strategy implementations for application behavior patterns.
 */
namespace Etrek::Application::Strategy {

/**
 * @class ClinicalExaminationStrategy
 * @brief Strategy for standard clinical patient examination mode.
 *
 * @details This is the primary examination mode used for real patient exams.
 *          It implements the full DICOM workflow including:
 *          - Creating DICOM Study, Series, and Image objects
 *          - Persisting images to the database
 *          - Sending images to configured PACS servers
 *          - Updating worklist entry status (MWL)
 *          - Complete audit trail logging
 *
 * @par Workflow:
 * 1. onExaminationStart() - Initializes DICOM study, sets MWL to IN_PROGRESS
 * 2. onBeforeExposure() - Validates patient data and technique parameters
 * 3. onAfterExposure() - Logs exposure, prepares for image reception
 * 4. onImageReceived() - Creates DICOM objects, stores in database
 * 5. onExaminationComplete() - Finalizes study, sends to PACS, updates MWL
 *
 * @see IExaminationModeStrategy
 * @see DemoExaminationStrategy
 * @see CalibrationExaminationStrategy
 * @see TestExaminationStrategy
 *
 * @ingroup Examination
 */
class ClinicalExaminationStrategy : public Etrek::Examination::IExaminationModeStrategy {
public:
    ClinicalExaminationStrategy();
    ~ClinicalExaminationStrategy() override = default;

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
    int m_exposureCount = 0;
};

} // namespace Etrek::Application::Strategy

#endif // ETREK_APPLICATION_STRATEGY_CLINICALEXAMINATIONSTRATEGY_H
