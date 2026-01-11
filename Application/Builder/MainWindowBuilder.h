/**
 * @file MainWindowBuilder.h
 * @brief Builder for MainWindow and MainWindowDelegate construction.
 *
 * @details Implements the Builder pattern for creating the main application
 *          window and its delegate, wiring all necessary connections.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see MainWindow
 * @see MainWindowDelegate
 * @see IWidgetDelegateBuilder
 */

#ifndef MAINWINDOWBUILDER_H
#define MAINWINDOWBUILDER_H

#include <memory>
#include "IWidgetDelegateBuilder.h"
#include "MainWindowDelegate.h"
#include "SystemSettingPageBuilder.h"
#include "DelegateParameter.h"
#include "MainWindow.h"


/**
 * @namespace Etrek::Application::Delegate
 * @brief Contains delegate and builder classes for application-level components.
 */
namespace Etrek::Application::Delegate
{

    /**
     * @class MainWindowBuilder
     * @brief Builder for constructing MainWindow and MainWindowDelegate pairs.
     *
     * @details Implements IWidgetDelegateBuilder to create the main application
     *          window and its delegate. Responsible for:
     *          - Creating the MainWindow UI
     *          - Creating and configuring MainWindowDelegate
     *          - Building and integrating child pages (ExamPage, WorklistPage, etc.)
     *          - Wiring signals and slots between components
     *
     * @see MainWindow
     * @see MainWindowDelegate
     * @see IWidgetDelegateBuilder
     *
     * @ingroup Application
     */
    class MainWindowBuilder :
        public IWidgetDelegateBuilder<MainWindow, MainWindowDelegate>
    {
    public:
        /**
         * @brief Default constructor.
         */
        MainWindowBuilder();

        /**
         * @brief Destructor.
         */
        ~MainWindowBuilder();

        /**
         * @brief Builds the MainWindow and MainWindowDelegate pair.
         *
         * @param[in] params Parameters for delegate configuration.
         * @param[in] parentWidget Optional parent widget.
         * @param[in] parentDelegate Optional parent delegate.
         * @return Pair containing the created MainWindow and MainWindowDelegate.
         */
        std::pair<MainWindow*, MainWindowDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;

    private:
        // Add any member variables if needed (e.g., configuration)
    };
}
#endif // MAINWINDOWBUILDER_H
