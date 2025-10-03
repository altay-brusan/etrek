#ifndef IMAGECOMMENTCONFIGURATIONDELEGATE_H
#define IMAGECOMMENTCONFIGURATIONDELEGATE_H

#include <QObject>
#include <QWidget>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Dicom::Delegate 
{
    class ImageCommentConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
    {
        Q_OBJECT
        Q_INTERFACES(IDelegate IPageAction)

    public:
        explicit ImageCommentConfigurationDelegate(QWidget* widget, QObject* parent = nullptr);

        // Inherited via IDelegate
        QString name() const override;
        void attachDelegates(const QVector<QObject*>& delegates) override;

		~ImageCommentConfigurationDelegate();

    private:
        QWidget* m_widget = nullptr;

        
        // Inherited via IPageAction
        void apply() override;
        void accept() override;
        void reject() override;
    };
}

#endif // IMAGECOMMENTCONFIGURATION_H
