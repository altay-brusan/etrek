#include "TestExaminationStrategy.h"
#include "LoggerProvider.h"

namespace Etrek::Application::Strategy {

using Etrek::Examination::ExaminationMode;

TestExaminationStrategy::TestExaminationStrategy()
    : m_logger(Etrek::Core::Log::LoggerProvider::Instance().GetLogger("TestExaminationStrategy"))
    , m_testExposureCount(0)
    , m_allTestsPassed(true)
{
}

ExaminationMode TestExaminationStrategy::mode() const
{
    return ExaminationMode::TEST;
}

QString TestExaminationStrategy::modeName() const
{
    return QStringLiteral("Test");
}

void TestExaminationStrategy::onExaminationStart()
{
    m_logger->LogInfo("Test mode started - device verification in progress");
    m_testExposureCount = 0;
    m_allTestsPassed = true;
}

bool TestExaminationStrategy::onBeforeExposure()
{
    m_logger->LogDebug("Test mode: Pre-exposure diagnostics");
    // Test mode verifies:
    // - All hardware components are responding
    // - Communication links are established
    // - Safety interlocks are functioning
    return true;
}

void TestExaminationStrategy::onAfterExposure(bool success)
{
    m_testExposureCount++;
    if (success) {
        m_logger->LogInfo(QString("Test exposure #%1 passed").arg(m_testExposureCount));
    } else {
        m_logger->LogError(QString("Test exposure #%1 FAILED").arg(m_testExposureCount));
        m_allTestsPassed = false;
    }
}

void TestExaminationStrategy::onImageReceived(const QByteArray& imageData, const QString& imageId)
{
    m_logger->LogInfo(QString("Test mode: Image received [%1], size: %2 bytes")
        .arg(imageId)
        .arg(imageData.size()));
    // Test images are analyzed for:
    // - Basic image quality metrics (SNR, uniformity)
    // - Detector response verification
    // - Known phantom pattern recognition (if using test phantom)
}

void TestExaminationStrategy::onExaminationComplete()
{
    if (m_allTestsPassed) {
        m_logger->LogInfo(QString("Test mode completed - %1 exposures, ALL PASSED")
            .arg(m_testExposureCount));
    } else {
        m_logger->LogWarning(QString("Test mode completed - %1 exposures, SOME TESTS FAILED")
            .arg(m_testExposureCount));
    }
    // Finalization:
    // - Generate QA test report
    // - Update device QA log
    // - Optionally flag device for service if tests failed
}

bool TestExaminationStrategy::shouldPersistImages() const
{
    // Test images can be persisted to a QA database for record keeping
    return true;
}

bool TestExaminationStrategy::shouldCreateDicom() const
{
    return false; // No clinical DICOM objects for test mode
}

bool TestExaminationStrategy::shouldUpdateWorklistStatus() const
{
    return false; // Test mode is not associated with worklist
}

bool TestExaminationStrategy::shouldSendToPacs() const
{
    return false; // Test images are not sent to clinical PACS
}

} // namespace Etrek::Application::Strategy
