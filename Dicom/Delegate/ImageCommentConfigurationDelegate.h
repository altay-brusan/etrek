#ifndef IMAGECOMMENTCONFIGURATIONDELEGATE_H
#define IMAGECOMMENTCONFIGURATIONDELEGATE_H

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Dicom::Repository { class ImageCommentRepository; }
class ImageCommentConfigurationWidget;

namespace Etrek::Dicom::Delegate
{
    namespace rpo = Etrek::Dicom::Repository;

    class ImageCommentConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
    {
        Q_OBJECT
        Q_INTERFACES(IDelegate IPageAction)

    public:
        ImageCommentConfigurationDelegate(
            ImageCommentConfigurationWidget* widget,
            std::shared_ptr<rpo::ImageCommentRepository> repository,
            QObject* parent = nullptr);

        // Inherited via IDelegate
        QString name() const override;
        void attachDelegates(const QVector<QObject*>& delegates) override;

		~ImageCommentConfigurationDelegate();

    private:
        ImageCommentConfigurationWidget* m_widget = nullptr;
        std::shared_ptr<rpo::ImageCommentRepository> m_repository;

        // Inherited via IPageAction
        void apply() override;
        void accept() override;
        void reject() override;
    };
}

#endif // IMAGECOMMENTCONFIGURATIONDELEGATE_H
