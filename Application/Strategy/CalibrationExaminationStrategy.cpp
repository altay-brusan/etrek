#include "CalibrationExaminationStrategy.h"
#include "LoggerProvider.h"

namespace Etrek::Application::Strategy {

using Etrek::Examination::ExaminationMode;

CalibrationExaminationStrategy::CalibrationExaminationStrategy()
    : m_logger(Etrek::Core::Log::LoggerProvider::Instance().GetLogger("CalibrationExaminationStrategy"))
    , m_calibrationFrameCount(0)
{
}

ExaminationMode CalibrationExaminationStrategy::mode() const
{
    return ExaminationMode::CALIBRATION;
}

QString CalibrationExaminationStrategy::modeName() const
{
    return QStringLiteral("Calibration");
}

void CalibrationExaminationStrategy::onExaminationStart()
{
    m_logger->LogInfo("Calibration mode started - preparing for detector/generator calibration");
    m_calibrationFrameCount = 0;
}

bool CalibrationExaminationStrategy::onBeforeExposure()
{
    m_logger->LogDebug("Calibration mode: Pre-exposure check");
    // Calibration mode should verify:
    // - Detector is in calibration-ready state
    // - No patient is in the imaging area (safety interlock)
    // - Generator is properly configured for calibration exposure
    return true;
}

void CalibrationExaminationStrategy::onAfterExposure(bool success)
{
    if (success) {
        m_calibrationFrameCount++;
        m_logger->LogInfo(QString("Calibration frame #%1 acquired").arg(m_calibrationFrameCount));
    } else {
        m_logger->LogWarning("Calibration exposure failed - check equipment status");
    }
}

void CalibrationExaminationStrategy::onImageReceived(const QByteArray& imageData, const QString& imageId)
{
    m_logger->LogInfo(QString("Calibration mode: Frame received [%1], size: %2 bytes")
        .arg(imageId)
        .arg(imageData.size()));
    // Calibration images are processed for:
    // - Gain/offset coefficient calculation
    // - Defect pixel detection
    // - Uniformity analysis
    // Results are saved to the detector's calibration file
}

void CalibrationExaminationStrategy::onExaminationComplete()
{
    m_logger->LogInfo(QString("Calibration completed - %1 frames acquired")
        .arg(m_calibrationFrameCount));
    // Finalization:
    // - Calculate final calibration coefficients
    // - Save calibration data to detector configuration
    // - Optionally generate calibration report
}

bool CalibrationExaminationStrategy::shouldPersistImages() const
{
    // Calibration images are saved locally for analysis
    // but not to the main patient image database
    return true;
}

bool CalibrationExaminationStrategy::shouldCreateDicom() const
{
    return false; // No DICOM objects for calibration
}

bool CalibrationExaminationStrategy::shouldUpdateWorklistStatus() const
{
    return false; // Calibration is not associated with worklist
}

bool CalibrationExaminationStrategy::shouldSendToPacs() const
{
    return false; // Calibration images are not sent to PACS
}

} // namespace Etrek::Application::Strategy
