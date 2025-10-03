#include "ConnectionSettingBuilder.h"

using namespace Etrek::Device::Delegate;

ConnectionSettingBuilder::ConnectionSettingBuilder()
{}

ConnectionSettingBuilder::~ConnectionSettingBuilder()
{}

std::pair<ConnectionSettingWidget*, QObject*>
ConnectionSettingBuilder::build(const DelegateParameter& params,
    QWidget* parentWidget,
    QObject* parentDelegate)
{
    // Use params.dbConnection if needed
    auto widget = new ConnectionSettingWidget(parentWidget);        
    return { widget, nullptr };
}

