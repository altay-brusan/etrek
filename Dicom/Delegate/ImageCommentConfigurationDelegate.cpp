#include "ImageCommentConfigurationDelegate.h"
#include "ImageCommentConfigurationWidget.h"
#include "ImageCommentRepository.h"

namespace Etrek::Dicom::Delegate
{
	using Etrek::Dicom::Repository::ImageCommentRepository;

	ImageCommentConfigurationDelegate::ImageCommentConfigurationDelegate(
		ImageCommentConfigurationWidget* widget,
		std::shared_ptr<ImageCommentRepository> repository,
		QObject* parent)
		: QObject(parent), m_widget(widget), m_repository(repository)
	{
	}

	ImageCommentConfigurationDelegate::~ImageCommentConfigurationDelegate()
	{
	}

	QString ImageCommentConfigurationDelegate::name() const
	{
		return QStringLiteral("ImageCommentConfigurationDelegate");
	}

	void ImageCommentConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
		Q_UNUSED(delegates);
	}

	void ImageCommentConfigurationDelegate::apply()
	{
		// TODO: Implement when ImageCommentConfigurationWidget has getter methods
		// Example: auto comments = m_widget->getComments();
		// for (const auto& comment : comments) {
		//     m_repository->updateComment(comment);
		// }
	}

	void ImageCommentConfigurationDelegate::accept()
	{
		apply();
	}

	void ImageCommentConfigurationDelegate::reject()
	{
	}
}

