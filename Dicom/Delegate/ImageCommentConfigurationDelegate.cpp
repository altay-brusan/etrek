#include "ImageCommentConfigurationDelegate.h"
#include "ImageCommentConfigurationDelegate.h"
#include "ImageCommentConfigurationDelegate.h"
#include "ImageCommentConfigurationDelegate.h"
#include "ImageCommentConfigurationDelegate.h"
#include "ImageCommentConfigurationDelegate.h"
#include "ImageCommentConfigurationDelegate.h"

namespace Etrek::Dicom::Delegate 
{
	ImageCommentConfigurationDelegate::ImageCommentConfigurationDelegate(QWidget* widget, QObject* parent)
		: QObject{ parent }, m_widget(widget)
	{
	}

	QString ImageCommentConfigurationDelegate::name() const
	{
		return QString();
	}

	void ImageCommentConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
	{
	}

	ImageCommentConfigurationDelegate::~ImageCommentConfigurationDelegate()
	{
	}

	void ImageCommentConfigurationDelegate::apply()
	{
	}

	void ImageCommentConfigurationDelegate::accept()
	{
	}

	void ImageCommentConfigurationDelegate::reject()
	{
	}

}

