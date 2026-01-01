#ifndef IMAGEDISPLAYPARAMS_H
#define IMAGEDISPLAYPARAMS_H

#include <QString>

namespace Etrek::ImageViewer::Data::Entity {

/**
 * @struct ImageDisplayParams
 * @brief Parameters for image display configuration.
 */
struct ImageDisplayParams {
    // Display options
    bool showOverlay = true;
    bool showCrosshairs = false;
    bool showRulers = false;
    bool smoothInterpolation = true;

    // Overlay font settings
    QString overlayFontFamily = "Consolas";
    int overlayFontSize = 10;
    QString overlayColor = "#00FF00";  // Green (common in medical imaging)

    // Measurement settings
    QString measurementColor = "#FFFF00";  // Yellow
    int measurementLineWidth = 2;

    // Grid settings
    bool showGrid = false;
    int gridSpacing = 50;  // pixels
    QString gridColor = "#404040";
};

} // namespace Etrek::ImageViewer::Data::Entity

#endif // IMAGEDISPLAYPARAMS_H
