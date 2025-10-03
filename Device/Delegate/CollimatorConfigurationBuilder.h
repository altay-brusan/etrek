#ifndef COLLIMATORCONFIGURATIONBUILDER_H
#define COLLIMATORCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "CollimatorConfigurationDelegate.h"
#include "CollimatorConfigurationWidget.h"
#include "DelegateParameter.h"
#include <memory>
#include <QMap>
#include <QObject>
#include <QWidget>

namespace Etrek::Device::Delegate
{
    class CollimatorConfigurationBuilder :
        public IWidgetDelegateBuilder<CollimatorConfigurationWidget, CollimatorConfigurationDelegate>
    {
    public:
        explicit CollimatorConfigurationBuilder();


        std::pair<CollimatorConfigurationWidget*,CollimatorConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
        
                ~CollimatorConfigurationBuilder();

    };
}

#endif // COLLIMATORCONFIGURATIONBUILDER_H
