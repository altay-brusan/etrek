#include "RulerTool.h"

#include <QIcon>

namespace Etrek::ImageViewer::Tool {

RulerTool::RulerTool(QObject* parent)
    : IImageTool(parent)
{
}

QIcon RulerTool::icon() const {
    return QIcon(":/Images/Asset/Icon/ruler.png");
}

void RulerTool::activate() {
    m_active = true;
    emit cursorChanged(Qt::CrossCursor);
}

void RulerTool::deactivate() {
    m_active = false;
    m_editState = RulerEditState::NONE;
    m_currentLine = MeasurementLine();
    m_selectedRulerId = -1;
    m_dragRulerId = -1;
    // Clear hover state
    if (m_lastHoveredRulerId != -1) {
        m_lastHoveredRulerId = -1;
        emit handleHoverCleared();
    }
}

void RulerTool::onMousePress(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton) {
        // First, check if we hit any existing ruler handle
        int hitRulerId = -1;
        bool hitIsStart = false;

        if (hitTestHandle(pos, hitRulerId, hitIsStart)) {
            // Start dragging the handle
            m_editState = hitIsStart ? RulerEditState::DRAGGING_START : RulerEditState::DRAGGING_END;
            m_dragRulerId = hitRulerId;
            m_dragIsStart = hitIsStart;
            selectRuler(hitRulerId);
            emit cursorChanged(Qt::SizeAllCursor);
            return;
        }

        // Check if we hit a ruler line (for selection)
        int hitLineId = hitTestRuler(pos);
        if (hitLineId >= 0) {
            selectRuler(hitLineId);
            // Don't start drawing - just select
            return;
        }

        // No hit - start drawing a new ruler
        m_editState = RulerEditState::DRAWING_NEW;
        m_currentLine = MeasurementLine();
        m_currentLine.id = m_nextRulerId++;
        m_currentLine.startPoint = pos;
        m_currentLine.endPoint = pos;
        m_currentLine.isComplete = false;

        // Deselect any selected ruler
        selectRuler(-1);

        // Clear hover state when starting to draw
        if (m_lastHoveredRulerId != -1) {
            m_lastHoveredRulerId = -1;
            emit handleHoverCleared();
        }

        emit measurementLineUpdated(m_currentLine);
    }
}

void RulerTool::onMouseMove(const QPointF& pos) {
    if (!m_active) return;

    switch (m_editState) {
        case RulerEditState::DRAWING_NEW: {
            // Drawing a new ruler
            m_currentLine.endPoint = pos;
            m_currentLine.calculateDistance(m_pixelSpacingX, m_pixelSpacingY);
            emit measurementLineUpdated(m_currentLine);
            break;
        }

        case RulerEditState::DRAGGING_START:
        case RulerEditState::DRAGGING_END: {
            // Dragging an existing ruler's handle
            int index = findRulerIndex(m_dragRulerId);
            if (index >= 0) {
                if (m_editState == RulerEditState::DRAGGING_START) {
                    m_measurements[index].startPoint = pos;
                } else {
                    m_measurements[index].endPoint = pos;
                }
                m_measurements[index].calculateDistance(m_pixelSpacingX, m_pixelSpacingY);
                emit rulerModified(m_dragRulerId, m_measurements[index]);
                emit measurementsChanged();
            }
            break;
        }

        case RulerEditState::NONE: {
            // Check for handle hover to change cursor and emit hover signal
            int hoverRulerId = -1;
            bool hoverIsStart = false;
            if (hitTestHandle(pos, hoverRulerId, hoverIsStart)) {
                emit cursorChanged(Qt::SizeAllCursor);
                // Emit hover signal if hover state changed
                if (hoverRulerId != m_lastHoveredRulerId || hoverIsStart != m_lastHoveredIsStart) {
                    m_lastHoveredRulerId = hoverRulerId;
                    m_lastHoveredIsStart = hoverIsStart;
                    emit handleHovered(hoverRulerId, hoverIsStart);
                }
            } else if (hitTestRuler(pos) >= 0) {
                emit cursorChanged(Qt::PointingHandCursor);
                // Clear hover if we were hovering a handle
                if (m_lastHoveredRulerId != -1) {
                    m_lastHoveredRulerId = -1;
                    emit handleHoverCleared();
                }
            } else {
                emit cursorChanged(Qt::CrossCursor);
                // Clear hover if we were hovering a handle
                if (m_lastHoveredRulerId != -1) {
                    m_lastHoveredRulerId = -1;
                    emit handleHoverCleared();
                }
            }
            break;
        }
    }
}

void RulerTool::onMouseRelease(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton) {
        switch (m_editState) {
            case RulerEditState::DRAWING_NEW: {
                // Complete the new ruler
                m_currentLine.endPoint = pos;
                m_currentLine.calculateDistance(m_pixelSpacingX, m_pixelSpacingY);
                m_currentLine.isComplete = true;

                // Only save measurements with some length
                if (m_currentLine.distancePixels > 2.0) {
                    m_measurements.append(m_currentLine);
                    emit measurementLineCompleted(m_currentLine);
                    emit measurementsChanged();
                }

                m_currentLine = MeasurementLine();
                m_editState = RulerEditState::NONE;

                // Clear hover state after completing the line
                if (m_lastHoveredRulerId != -1) {
                    m_lastHoveredRulerId = -1;
                    emit handleHoverCleared();
                }
                break;
            }

            case RulerEditState::DRAGGING_START:
            case RulerEditState::DRAGGING_END: {
                // Finish dragging
                int index = findRulerIndex(m_dragRulerId);
                if (index >= 0) {
                    if (m_editState == RulerEditState::DRAGGING_START) {
                        m_measurements[index].startPoint = pos;
                    } else {
                        m_measurements[index].endPoint = pos;
                    }
                    m_measurements[index].calculateDistance(m_pixelSpacingX, m_pixelSpacingY);
                    emit rulerModified(m_dragRulerId, m_measurements[index]);
                    emit measurementsChanged();
                }

                m_dragRulerId = -1;
                m_editState = RulerEditState::NONE;
                emit cursorChanged(Qt::CrossCursor);

                // Clear hover state after dropping the handle
                if (m_lastHoveredRulerId != -1) {
                    m_lastHoveredRulerId = -1;
                    emit handleHoverCleared();
                }
                break;
            }

            case RulerEditState::NONE:
                // Nothing to do
                break;
        }
    }
}

void RulerTool::onMouseWheel(int delta, const QPointF& pos) {
    Q_UNUSED(delta)
    Q_UNUSED(pos)
    // Ruler tool doesn't use wheel
}

void RulerTool::reset() {
    m_editState = RulerEditState::NONE;
    m_currentLine = MeasurementLine();
    m_selectedRulerId = -1;
    m_dragRulerId = -1;
    // Clear hover state
    if (m_lastHoveredRulerId != -1) {
        m_lastHoveredRulerId = -1;
        emit handleHoverCleared();
    }
}

void RulerTool::setPixelSpacing(double spacingX, double spacingY) {
    m_pixelSpacingX = spacingX;
    m_pixelSpacingY = spacingY;

    // Recalculate all measurements with new spacing
    for (auto& line : m_measurements) {
        line.calculateDistance(m_pixelSpacingX, m_pixelSpacingY);
    }
    if (!m_measurements.isEmpty()) {
        emit measurementsChanged();
    }
}

void RulerTool::clearMeasurements() {
    m_measurements.clear();
    m_currentLine = MeasurementLine();
    m_editState = RulerEditState::NONE;
    m_selectedRulerId = -1;
    m_dragRulerId = -1;
    emit measurementsChanged();
}

bool RulerTool::hitTestHandle(const QPointF& pos, int& outRulerId, bool& outIsStartHandle) const {
    // Check handles on all measurements (check selected ruler first for priority)
    for (const auto& line : m_measurements) {
        if (line.isNearStartHandle(pos, HANDLE_HIT_THRESHOLD)) {
            outRulerId = line.id;
            outIsStartHandle = true;
            return true;
        }
        if (line.isNearEndHandle(pos, HANDLE_HIT_THRESHOLD)) {
            outRulerId = line.id;
            outIsStartHandle = false;
            return true;
        }
    }
    return false;
}

int RulerTool::hitTestRuler(const QPointF& pos) const {
    for (const auto& line : m_measurements) {
        if (line.isNearLine(pos, LINE_HIT_THRESHOLD)) {
            return line.id;
        }
    }
    return -1;
}

void RulerTool::selectRuler(int rulerId) {
    if (m_selectedRulerId != rulerId) {
        // Deselect previous
        int prevIndex = findRulerIndex(m_selectedRulerId);
        if (prevIndex >= 0) {
            m_measurements[prevIndex].isSelected = false;
        }

        m_selectedRulerId = rulerId;

        // Select new
        int newIndex = findRulerIndex(rulerId);
        if (newIndex >= 0) {
            m_measurements[newIndex].isSelected = true;
        }

        emit rulerSelected(rulerId);
        emit measurementsChanged();
    }
}

void RulerTool::deleteRuler(int rulerId) {
    int index = findRulerIndex(rulerId);
    if (index >= 0) {
        m_measurements.removeAt(index);

        if (m_selectedRulerId == rulerId) {
            m_selectedRulerId = -1;
        }
        if (m_dragRulerId == rulerId) {
            m_dragRulerId = -1;
            m_editState = RulerEditState::NONE;
        }

        emit rulerDeleted(rulerId);
        emit measurementsChanged();
    }
}

void RulerTool::deleteSelectedRuler() {
    if (m_selectedRulerId >= 0) {
        deleteRuler(m_selectedRulerId);
    }
}

int RulerTool::findRulerIndex(int rulerId) const {
    if (rulerId < 0) return -1;

    for (int i = 0; i < m_measurements.size(); ++i) {
        if (m_measurements[i].id == rulerId) {
            return i;
        }
    }
    return -1;
}

} // namespace Etrek::ImageViewer::Tool
