#ifndef ETREK_APPLICATION_STRATEGY_CLINICALEXAMINATIONSTRATEGY_H
#define ETREK_APPLICATION_STRATEGY_CLINICALEXAMINATIONSTRATEGY_H

#include "IExaminationModeStrategy.h"
#include "AppLogger.h"
#include <memory>

namespace Etrek::Application::Strategy {

/**
 * @class ClinicalExaminationStrategy
 * @brief Strategy for normal patient examination mode.
 *
 * This is the primary examination mode used for real patient exams.
 * It creates DICOM objects, persists images to the database,
 * sends to PACS, and updates worklist status.
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
