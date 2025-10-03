#ifndef IWIDGETDELEGATEBUILDER_H
#define IWIDGETDELEGATEBUILDER_H

#include <QWidget>
#include <QObject>
#include <memory>

#include "DelegateParameter.h"

/**
 * @brief Template interface for building a widget and its corresponding delegate.
 *
 * In the project, each UI element (widget) has an associated delegate
 * which acts as the controller (MVC-like separation). This interface
 * standardizes the construction of these widget-delegate pairs.
 *
 * @tparam WidgetType The QWidget-derived class type.
 * @tparam DelegateType The delegate class type implementing IDelegate or similar interface.
 *
 * Builders are responsible for:
 * - Creating the widget and delegate.
 * - Wiring signals and slots between the widget and delegate.
 * - Configuring the delegate based on provided parameters.
 *
 * Note:
 * - Raw pointers are returned so that Qt's parent-child mechanism
 *   can manage object lifetimes automatically.
 * - The builder receives:
 *   - `parentWidget` to set as the QWidget parent.
 *   - `parentDelegate` to set as the delegate parent (QObject-based).
 *   - `DelegateParameter` containing settings or configuration read from
 *     settings files or runtime context.
 */
template<typename WidgetType, typename DelegateType>
class IWidgetDelegateBuilder {
public:
    /**
     * @brief Build the widget and its delegate.
     *
     * @param params Parameters for delegate configuration.
     * @param parentWidget Optional parent widget (Qt parent for lifetime management).
     * @param parentDelegate Optional parent delegate (QObject parent for lifetime management).
     * @return std::pair<WidgetType*, DelegateType*> The created widget and delegate pointers.
     */
    virtual std::pair<WidgetType*, DelegateType*>
        build(const DelegateParameter& params,
            QWidget* parentWidget = nullptr,
            QObject* parentDelegate = nullptr) = 0;

    virtual ~IWidgetDelegateBuilder() = default;
};

#endif // IWIDGETDELEGATEBUILDER_H
