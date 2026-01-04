#include "DemoExaminationStrategy.h"
#include "LoggerProvider.h"
#include <QDir>
#include <QCoreApplication>

namespace Etrek::Application::Strategy {

using Etrek::Examination::ExaminationMode;

DemoExaminationStrategy::DemoExaminationStrategy()
    : m_logger(Etrek::Core::Log::LoggerProvider::Instance().GetLogger("DemoExaminationStrategy"))
    , m_currentImageIndex(0)
{
}

ExaminationMode DemoExaminationStrategy::mode() const
{
    return ExaminationMode::DEMO;
}

QString DemoExaminationStrategy::modeName() const
{
    return QStringLiteral("Demo");
}

void DemoExaminationStrategy::onExaminationStart()
{
    m_logger->LogInfo("Demo examination started - loading sample images");
    m_currentImageIndex = 0;
    loadSampleImagePaths();
}

bool DemoExaminationStrategy::onBeforeExposure()
{
    m_logger->LogDebug("Demo mode: Simulating pre-exposure (no real X-ray)");
    // Demo mode always allows "exposure" since it's just loading sample images
    return true;
}

void DemoExaminationStrategy::onAfterExposure(bool success)
{
    Q_UNUSED(success)
    m_logger->LogDebug("Demo mode: Simulated exposure complete");
    // In demo mode, success is simulated
}

void DemoExaminationStrategy::onImageReceived(const QByteArray& imageData, const QString& imageId)
{
    Q_UNUSED(imageData)
    m_logger->LogInfo(QString("Demo mode: Displaying sample image [%1]").arg(imageId));
    // In demo mode, we typically override the image source entirely
    // The actual sample image path is provided via getNextSampleImagePath()
}

void DemoExaminationStrategy::onExaminationComplete()
{
    m_logger->LogInfo("Demo examination completed");
    m_sampleImagePaths.clear();
    m_currentImageIndex = 0;
}

bool DemoExaminationStrategy::shouldPersistImages() const
{
    return false; // Demo images are not saved
}

bool DemoExaminationStrategy::shouldCreateDicom() const
{
    return false; // No DICOM objects in demo mode
}

bool DemoExaminationStrategy::shouldUpdateWorklistStatus() const
{
    return false; // Don't modify worklist entries in demo mode
}

bool DemoExaminationStrategy::shouldSendToPacs() const
{
    return false; // No PACS transmission in demo mode
}

QString DemoExaminationStrategy::getNextSampleImagePath()
{
    if (m_sampleImagePaths.isEmpty()) {
        loadSampleImagePaths();
    }

    if (m_sampleImagePaths.isEmpty()) {
        m_logger->LogWarning("No sample images available for demo mode");
        return QString();
    }

    QString path = m_sampleImagePaths.at(m_currentImageIndex);
    m_currentImageIndex = (m_currentImageIndex + 1) % m_sampleImagePaths.size();
    return path;
}

void DemoExaminationStrategy::loadSampleImagePaths()
{
    m_sampleImagePaths.clear();

    // Look for sample images in the application's demo directory
    QString appDir = QCoreApplication::applicationDirPath();
    QDir demoDir(appDir + "/demo/images");

    if (!demoDir.exists()) {
        m_logger->LogWarning(QString("Demo image directory not found: %1").arg(demoDir.absolutePath()));
        return;
    }

    QStringList filters;
    filters << "*.dcm" << "*.png" << "*.jpg" << "*.raw";
    QStringList files = demoDir.entryList(filters, QDir::Files, QDir::Name);

    for (const QString& file : files) {
        m_sampleImagePaths.append(demoDir.absoluteFilePath(file));
    }

    m_logger->LogInfo(QString("Loaded %1 sample images for demo mode").arg(m_sampleImagePaths.size()));
}

} // namespace Etrek::Application::Strategy
