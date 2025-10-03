#include "SystemSettingPageBuilder.h"
#include "WorkflowConfigurationBuilder.h"
#include "ConnectionSetupBuilder.h"
#include "ImageCommentConfigurationBuilder.h"
#include "DetectorConfigurationBuilder.h"
#include "GeneratorConfigurationBuilder.h"
#include "DapConfigurationBuilder.h"
#include "CollimatorConfigurationBuilder.h"
#include "WorkListConfigurationBuilder.h"
#include "PacsEntityConfigurationBuilder.h"
#include "ProcedureConfigurationBuilder.h"
#include "ViewConfigurationBuilder.h"
#include "TechniqueConfigurationBuilder.h"

using namespace Etrek::Worklist::Delegate;
using namespace Etrek::Device::Delegate;
using namespace Etrek::Dicom::Delegate;
using namespace Etrek::Pacs::Delegate;
using namespace Etrek::ScanProtocol::Delegate;

namespace Etrek::Application::Delegate
{

    SystemSettingPageBuilder::SystemSettingPageBuilder() = default;
    SystemSettingPageBuilder::~SystemSettingPageBuilder() = default;

    std::pair<SystemSettingPage*, SystemSettingPageDelegate*>
        SystemSettingPageBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto page = new SystemSettingPage(parentWidget);

        // Prepare delegate parameters for all sub-builders
        DelegateParameter delegateParameters;
        delegateParameters.dbConnection = params.dbConnection;

        // WorkflowConfiguration
        WorkflowConfigurationBuilder workflowConfigurationBuilder;
        auto workflowConfig = workflowConfigurationBuilder.build(delegateParameters, page, parentDelegate);
        auto* workflowConfigurationWidget = workflowConfig.first;
        auto& workflowConfigurationDelegate = workflowConfig.second;
        page->setWorkflowConfigurationWidget(workflowConfigurationWidget);

        // ConnectionSetup
        ConnectionSetupBuilder connectionSetupBuilder;
        auto connectionSetup = connectionSetupBuilder.build(delegateParameters, page, parentDelegate);
        auto* connectionSetupWidget = connectionSetup.first;
        auto& connectionSetupDelegate = connectionSetup.second;
        page->setConnectionSetupWidget(connectionSetupWidget);

        // ImageCommentConfiguration
        ImageCommentConfigurationBuilder imageCommentConfigBuilder;
        auto imageCommentConfig = imageCommentConfigBuilder.build(delegateParameters, page, parentDelegate);
        auto* imageCommentConfigWidget = imageCommentConfig.first;
        auto& imageCommentDelegate = imageCommentConfig.second;
        page->setImageCommentWidget(imageCommentConfigWidget);

        // DetectorConfiguration
        DetectorConfigurationBuilder detectorConfigBuilder;
        auto detectorConfig = detectorConfigBuilder.build(delegateParameters, page, parentDelegate);
        auto* detectorConfigWidget = detectorConfig.first;
        auto& detectorCommentDelegate = detectorConfig.second;
        page->setDetectorConfigurationWidget(detectorConfigWidget);

        // GeneratorConfiguration
        GeneratorConfigurationBuilder generatorConfigBuilder;
        auto generatorConfig = generatorConfigBuilder.build(delegateParameters, page, parentDelegate);
        auto* generatorConfigWidget = generatorConfig.first;
        auto& generatorDelegate = generatorConfig.second;
        page->setGeneratorConfigurationWidget(generatorConfigWidget);

        // DapConfiguration
        DapConfigurationBuilder dapConfigBuilder;
        auto dapConfig = dapConfigBuilder.build(delegateParameters, page, parentDelegate);
        auto* dapConfigWidget = dapConfig.first;
        auto& dapDelegate = dapConfig.second;
        page->setDapConfigurationWidget(dapConfigWidget);

        // CollimatorConfiguration
        CollimatorConfigurationBuilder collimatorConfigurationBuilder;
        auto collimatorConfig = collimatorConfigurationBuilder.build(delegateParameters, page, parentDelegate);
        auto* collimatorConfigWidget = collimatorConfig.first;
        auto& collimatorConfigDelegate = collimatorConfig.second;
        page->setCollimatorConfigurationWidget(collimatorConfigWidget);

        // WorkListConfiguration
        WorkListConfigurationBuilder workListConfigurationBuilder;
        auto worklistConfig = workListConfigurationBuilder.build(delegateParameters, page, parentDelegate);
        auto* worklistConfigWidget = worklistConfig.first;
        auto& worklistConfigDelegate = worklistConfig.second;
        page->setWorkListConfigurationWidget(worklistConfigWidget);

        // PacsEntityConfiguration
        PacsEntityConfigurationBuilder pacsEntityConfigurationBuilder;
        auto pacsEntityConfig = pacsEntityConfigurationBuilder.build(delegateParameters, page, parentDelegate);
        auto* pacsEntityConfigWidget = pacsEntityConfig.first;
        auto& pacsEntityConfigDelegate = pacsEntityConfig.second;
        page->setPacsEntityConfigurationWidget(pacsEntityConfigWidget);

        // ProcedureConfiguration
        ProcedureConfigurationBuilder procedureConfigurationBuilder;
        auto procedureConfig = procedureConfigurationBuilder.build(delegateParameters, page, parentDelegate);
        auto* procedureConfigWidget = procedureConfig.first;
        auto& procedureConfigDelegate = procedureConfig.second;
        page->setProcedureConfigurationWidget(procedureConfigWidget);

        // ViewConfiguration
        ViewConfigurationBuilder viewConfigurationBuilder;
        auto viewConfig = viewConfigurationBuilder.build(delegateParameters, page, parentDelegate);
        auto* viewConfigWidget = viewConfig.first;
        auto& viewConfigDelegate = viewConfig.second;
        page->setViewConfigurationWidget(viewConfigWidget);

        // TechniqueConfiguration
        TechniqueConfigurationBuilder techniqueConfigurationBuilder;
        auto techniqueConfig = techniqueConfigurationBuilder.build(delegateParameters, page, parentDelegate);
        auto* techniqueConfigWidget = techniqueConfig.first;
        auto& techniqueConfigDelegate = techniqueConfig.second;
        page->setTechniqueConfigurationWidget(techniqueConfigWidget);

        auto* systemSettingPageDelegate = new SystemSettingPageDelegate(
            page,
            parentDelegate
        );

        QVector<QObject*> delegates = {
            workflowConfigurationDelegate,
            connectionSetupDelegate,
            imageCommentDelegate,
            detectorCommentDelegate,
            generatorDelegate,
            dapDelegate,
            collimatorConfigDelegate,
            worklistConfigDelegate,
            pacsEntityConfigDelegate,
            procedureConfigDelegate,
            viewConfigDelegate,
            techniqueConfigDelegate
        };
        systemSettingPageDelegate->attachDelegates(delegates);
        
        return { page, systemSettingPageDelegate };
    }

}