#ifndef EXAMPAGEBUILDER_H
#define EXAMPAGEBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "ExamPageDelegate.h"
#include "ExamPage.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Application::Delegate {

/**
 * @class ExamPageBuilder
 * @brief Builder for ExamPage and ExamPageDelegate following MVD pattern.
 *
 * Responsibilities:
 * - Create ExamPage widget
 * - Instantiate all required repositories
 * - Create ExamPageDelegate with injected dependencies
 * - Return widget-delegate pair
 */
class ExamPageBuilder
    : public IWidgetDelegateBuilder<ExamPage, ExamPageDelegate>
{
public:
    ExamPageBuilder();
    ~ExamPageBuilder();

    /**
     * @brief Builds ExamPage and ExamPageDelegate with all dependencies.
     * @param params DelegateParameter containing dbConnection and contextManager
     * @param parentWidget Optional parent widget
     * @param parentDelegate Optional parent delegate
     * @return Pair of ExamPage widget and ExamPageDelegate
     */
    std::pair<ExamPage*, ExamPageDelegate*>
        build(const DelegateParameter& params,
              QWidget* parentWidget = nullptr,
              QObject* parentDelegate = nullptr) override;
};

} // namespace Etrek::Application::Delegate

#endif // EXAMPAGEBUILDER_H
