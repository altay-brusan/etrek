#ifndef VIEWPORTLAYOUT_H
#define VIEWPORTLAYOUT_H

namespace Etrek::ImageViewer {

/**
 * @enum ViewportLayout
 * @brief Defines the layout configuration for the image viewer grid.
 */
enum class ViewportLayout {
    SINGLE,      ///< 1x1 - Single viewport
    ONE_BY_TWO,  ///< 1x2 - Two viewports side by side
    TWO_BY_ONE,  ///< 2x1 - Two viewports stacked vertically
    TWO_BY_TWO   ///< 2x2 - Four viewports in a grid
};

/**
 * @brief Returns the number of viewports for a given layout.
 * @param layout The viewport layout
 * @return Number of visible viewports (1, 2, or 4)
 */
inline int viewportCount(ViewportLayout layout) {
    switch (layout) {
        case ViewportLayout::SINGLE:     return 1;
        case ViewportLayout::ONE_BY_TWO: return 2;
        case ViewportLayout::TWO_BY_ONE: return 2;
        case ViewportLayout::TWO_BY_TWO: return 4;
        default: return 1;
    }
}

/**
 * @brief Returns the grid dimensions for a given layout.
 * @param layout The viewport layout
 * @param rows Output: number of rows
 * @param cols Output: number of columns
 */
inline void layoutDimensions(ViewportLayout layout, int& rows, int& cols) {
    switch (layout) {
        case ViewportLayout::SINGLE:
            rows = 1; cols = 1;
            break;
        case ViewportLayout::ONE_BY_TWO:
            rows = 1; cols = 2;
            break;
        case ViewportLayout::TWO_BY_ONE:
            rows = 2; cols = 1;
            break;
        case ViewportLayout::TWO_BY_TWO:
            rows = 2; cols = 2;
            break;
        default:
            rows = 1; cols = 1;
    }
}

} // namespace Etrek::ImageViewer

#endif // VIEWPORTLAYOUT_H
