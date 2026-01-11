#include "ClinicalExaminationStrategy.h"
#include "AppLoggerFactory.h"
#include "TranslationProvider.h"

namespace Etrek::Application::Strategy {

using Etrek::Examination::ExaminationMode;
using Etrek::Core::Log::AppLoggerFactory;
using Etrek::Core::Log::LoggerProvider;
using Etrek::Core::Globalization::TranslationProvider;

ClinicalExaminationStrategy::ClinicalExaminationStrategy()
    : m_exposureCount(0)
{
    AppLoggerFactory factory(LoggerProvider::Instance(), &TranslationProvider::Instance());
    m_logger = factory.CreateLogger("ClinicalExaminationStrategy");
}

ExaminationMode ClinicalExaminationStrategy::mode() const
{
    return ExaminationMode::CLINICAL;
}

QString ClinicalExaminationStrategy::modeName() const
{
    return QStringLiteral("Clinical");
}

void ClinicalExaminationStrategy::onExaminationStart()
{
    m_logger->LogInfo("Clinical examination started - preparing for patient imaging");
    m_exposureCount = 0;
}

bool ClinicalExaminationStrategy::onBeforeExposure()
{
    m_logger->LogDebug("Clinical mode: Pre-exposure validation");
    // In clinical mode, we allow the exposure to proceed
    // Additional validation (patient positioning, etc.) can be added here
    return true;
}

void ClinicalExaminationStrategy::onAfterExposure(bool success)
{
    if (success) {
        m_exposureCount++;
        m_logger->LogInfo(QString("Clinical exposure #%1 completed successfully").arg(m_exposureCount));
    } else {
        m_logger->LogWarning("Clinical exposure failed - may need to retry");
    }
}

void ClinicalExaminationStrategy::onImageReceived(const QByteArray& imageData, const QString& imageId)
{
    m_logger->LogInfo(QString("Clinical mode: Image received [%1], size: %2 bytes")
        .arg(imageId)
        .arg(imageData.size()));
    // Image will be processed by the delegate:
    // - Apply processing pipeline
    // - Create DICOM Image object
    // - Queue for PACS transmission
}

void ClinicalExaminationStrategy::onExaminationComplete()
{
    m_logger->LogInfo(QString("Clinical examination completed - %1 exposures taken")
        .arg(m_exposureCount));
    // Finalization handled by delegate:
    // - Complete DICOM Study
    // - Update worklist status to COMPLETED
    // - Trigger PACS C-STORE
}

bool ClinicalExaminationStrategy::shouldPersistImages() const
{
    return true; // Clinical images are always persisted
}

bool ClinicalExaminationStrategy::shouldCreateDicom() const
{
    return true; // Clinical mode creates full DICOM objects
}

bool ClinicalExaminationStrategy::shouldUpdateWorklistStatus() const
{
    return true; // Update MWL entry status to reflect examination progress
}

bool ClinicalExaminationStrategy::shouldSendToPacs() const
{
    return true; // Clinical images are sent to PACS
}

} // namespace Etrek::Application::Strategy
