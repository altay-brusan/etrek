#ifndef IMAGETOOLPANEL_H
#define IMAGETOOLPANEL_H

#include <QWidget>
#include <QMap>

#include "ImageViewerTypes.h"
#include "ViewportLayout.h"

class QToolButton;
class QButtonGroup;

namespace Ui {
class ImageToolPanel;
}

/**
 * @class ImageToolPanel
 * @brief Left toolbar panel with imaging tool buttons and layout selection.
 *
 * Contains:
 * - Tool selection buttons (zoom, pan, window/level, ruler, angle, reset)
 * - Layout selection buttons (1x1, 1x2, 2x2)
 * - Additional action buttons (open file, invert, etc.)
 */
class ImageToolPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ImageToolPanel(QWidget* parent = nullptr);
    ~ImageToolPanel() override;

    /**
     * @brief Get the currently selected tool.
     */
    Etrek::ImageViewer::ToolType selectedTool() const;

    /**
     * @brief Set the selected tool.
     */
    void setSelectedTool(Etrek::ImageViewer::ToolType tool);

    /**
     * @brief Get the currently selected layout.
     */
    Etrek::ImageViewer::ViewportLayout selectedLayout() const;

    /**
     * @brief Set the selected layout.
     */
    void setSelectedLayout(Etrek::ImageViewer::ViewportLayout layout);

    /**
     * @brief Enable/disable specific tool buttons.
     */
    void setToolEnabled(Etrek::ImageViewer::ToolType tool, bool enabled);

signals:
    /**
     * @brief Emitted when a tool is selected.
     */
    void toolSelected(Etrek::ImageViewer::ToolType tool);

    /**
     * @brief Emitted when layout is changed.
     */
    void layoutSelected(Etrek::ImageViewer::ViewportLayout layout);

    /**
     * @brief Emitted when Open File button is clicked.
     */
    void openFileRequested();

    /**
     * @brief Emitted when Invert button is clicked.
     */
    void invertRequested();

    /**
     * @brief Emitted when Reset View button is clicked.
     */
    void resetViewRequested();

    /**
     * @brief Emitted when Fit to Window button is clicked.
     */
    void fitToWindowRequested();

private slots:
    void onToolButtonClicked(int id);
    void onLayoutButtonClicked(int id);

private:
    void setupToolButtons();
    void setupLayoutButtons();
    void setupActionButtons();
    QToolButton* createToolButton(const QString& iconPath, const QString& tooltip);

    Ui::ImageToolPanel* ui;
    QButtonGroup* m_toolButtonGroup;
    QButtonGroup* m_layoutButtonGroup;
    QMap<Etrek::ImageViewer::ToolType, QToolButton*> m_toolButtons;
    QMap<Etrek::ImageViewer::ViewportLayout, QToolButton*> m_layoutButtons;
    Etrek::ImageViewer::ToolType m_selectedTool = Etrek::ImageViewer::ToolType::WINDOW_LEVEL;
    Etrek::ImageViewer::ViewportLayout m_selectedLayout = Etrek::ImageViewer::ViewportLayout::SINGLE;
};

#endif // IMAGETOOLPANEL_H
