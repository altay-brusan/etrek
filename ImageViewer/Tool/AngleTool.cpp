#include "AngleTool.h"

#include <QIcon>

namespace Etrek::ImageViewer::Tool {

AngleTool::AngleTool(QObject* parent)
    : IImageTool(parent)
{
}

QIcon AngleTool::icon() const {
    return QIcon(":/Images/Asset/Icon/angle.png");
}

void AngleTool::activate() {
    m_active = true;
    emit cursorChanged(Qt::CrossCursor);
}

void AngleTool::deactivate() {
    m_active = false;
    m_editState = AngleEditState::NONE;
    m_currentAngle = MeasurementAngle();
    m_selectedAngleId = -1;
    m_dragAngleId = -1;
    m_dragHandleType = AngleHandleType::NONE;
    // Clear hover state
    if (m_lastHoveredAngleId != -1) {
        m_lastHoveredAngleId = -1;
        m_lastHoveredHandleType = AngleHandleType::NONE;
        emit handleHoverCleared();
    }
}

void AngleTool::onMousePress(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton) {
        // First, check if we hit any existing angle handle
        int hitAngleId = -1;
        AngleHandleType hitHandleType = AngleHandleType::NONE;

        if (hitTestHandle(pos, hitAngleId, hitHandleType)) {
            // Start dragging the handle
            switch (hitHandleType) {
                case AngleHandleType::POINT1:
                    m_editState = AngleEditState::DRAGGING_POINT1;
                    break;
                case AngleHandleType::VERTEX:
                    m_editState = AngleEditState::DRAGGING_VERTEX;
                    break;
                case AngleHandleType::POINT2:
                    m_editState = AngleEditState::DRAGGING_POINT2;
                    break;
                default:
                    break;
            }
            m_dragAngleId = hitAngleId;
            m_dragHandleType = hitHandleType;
            selectAngle(hitAngleId);
            emit cursorChanged(Qt::SizeAllCursor);
            return;
        }

        // Check if we hit an angle arm (for selection)
        int hitArmId = hitTestAngle(pos);
        if (hitArmId >= 0) {
            selectAngle(hitArmId);
            // Don't start drawing - just select
            return;
        }

        // No hit - continue with placing points or start new angle
        switch (m_editState) {
            case AngleEditState::NONE:
                // Start placing a new angle
                m_editState = AngleEditState::PLACING_POINT1;
                m_currentAngle = MeasurementAngle();
                m_currentAngle.id = m_nextAngleId++;
                m_currentAngle.point1 = pos;
                m_currentAngle.vertex = pos;
                m_currentAngle.point2 = pos;
                m_currentAngle.isComplete = false;

                // Deselect any selected angle
                selectAngle(-1);

                // Clear hover state when starting to place
                if (m_lastHoveredAngleId != -1) {
                    m_lastHoveredAngleId = -1;
                    m_lastHoveredHandleType = AngleHandleType::NONE;
                    emit handleHoverCleared();
                }

                emit measurementAngleUpdated(m_currentAngle);
                break;

            case AngleEditState::PLACING_POINT1:
                // Place vertex (second click)
                m_currentAngle.vertex = pos;
                m_currentAngle.point2 = pos;
                m_editState = AngleEditState::PLACING_VERTEX;
                emit measurementAngleUpdated(m_currentAngle);
                break;

            case AngleEditState::PLACING_VERTEX:
                // Place point2 and complete (third click)
                m_currentAngle.point2 = pos;
                m_currentAngle.calculateAngle();
                m_currentAngle.isComplete = true;
                m_editState = AngleEditState::PLACING_POINT2;
                // Will be finalized in mouse release
                emit measurementAngleUpdated(m_currentAngle);
                break;

            default:
                break;
        }
    } else if (button == Qt::RightButton) {
        // Cancel current measurement
        if (isPlacing()) {
            m_currentAngle = MeasurementAngle();
            m_editState = AngleEditState::NONE;
            emit measurementAngleUpdated(m_currentAngle);
        }
    }
}

void AngleTool::onMouseMove(const QPointF& pos) {
    if (!m_active) return;

    switch (m_editState) {
        case AngleEditState::PLACING_POINT1: {
            // After first click, show line to vertex
            m_currentAngle.vertex = pos;
            m_currentAngle.point2 = pos;
            emit measurementAngleUpdated(m_currentAngle);
            break;
        }

        case AngleEditState::PLACING_VERTEX: {
            // After second click, show second arm
            m_currentAngle.point2 = pos;
            m_currentAngle.calculateAngle();
            emit measurementAngleUpdated(m_currentAngle);
            break;
        }

        case AngleEditState::DRAGGING_POINT1:
        case AngleEditState::DRAGGING_VERTEX:
        case AngleEditState::DRAGGING_POINT2: {
            // Dragging an existing angle's handle
            int index = findAngleIndex(m_dragAngleId);
            if (index >= 0) {
                switch (m_dragHandleType) {
                    case AngleHandleType::POINT1:
                        m_measurements[index].point1 = pos;
                        break;
                    case AngleHandleType::VERTEX:
                        m_measurements[index].vertex = pos;
                        break;
                    case AngleHandleType::POINT2:
                        m_measurements[index].point2 = pos;
                        break;
                    default:
                        break;
                }
                m_measurements[index].calculateAngle();
                emit angleModified(m_dragAngleId, m_measurements[index]);
                emit measurementsChanged();
            }
            break;
        }

        case AngleEditState::NONE: {
            // Check for handle hover to change cursor
            int hoverAngleId = -1;
            AngleHandleType hoverHandleType = AngleHandleType::NONE;
            if (hitTestHandle(pos, hoverAngleId, hoverHandleType)) {
                emit cursorChanged(Qt::SizeAllCursor);
                // Emit hover signal if hover state changed
                if (hoverAngleId != m_lastHoveredAngleId || hoverHandleType != m_lastHoveredHandleType) {
                    m_lastHoveredAngleId = hoverAngleId;
                    m_lastHoveredHandleType = hoverHandleType;
                    emit handleHovered(hoverAngleId, hoverHandleType);
                }
            } else if (hitTestAngle(pos) >= 0) {
                emit cursorChanged(Qt::PointingHandCursor);
                // Clear hover if we were hovering a handle
                if (m_lastHoveredAngleId != -1) {
                    m_lastHoveredAngleId = -1;
                    m_lastHoveredHandleType = AngleHandleType::NONE;
                    emit handleHoverCleared();
                }
            } else {
                emit cursorChanged(Qt::CrossCursor);
                // Clear hover if we were hovering a handle
                if (m_lastHoveredAngleId != -1) {
                    m_lastHoveredAngleId = -1;
                    m_lastHoveredHandleType = AngleHandleType::NONE;
                    emit handleHoverCleared();
                }
            }
            break;
        }

        default:
            break;
    }
}

void AngleTool::onMouseRelease(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton) {
        switch (m_editState) {
            case AngleEditState::PLACING_POINT2: {
                // Complete the new angle
                m_currentAngle.point2 = pos;
                m_currentAngle.calculateAngle();
                m_currentAngle.isComplete = true;

                m_measurements.append(m_currentAngle);
                emit measurementAngleCompleted(m_currentAngle);
                emit measurementsChanged();

                m_currentAngle = MeasurementAngle();
                m_editState = AngleEditState::NONE;

                // Clear hover state after completing
                if (m_lastHoveredAngleId != -1) {
                    m_lastHoveredAngleId = -1;
                    m_lastHoveredHandleType = AngleHandleType::NONE;
                    emit handleHoverCleared();
                }
                break;
            }

            case AngleEditState::DRAGGING_POINT1:
            case AngleEditState::DRAGGING_VERTEX:
            case AngleEditState::DRAGGING_POINT2: {
                // Finish dragging
                int index = findAngleIndex(m_dragAngleId);
                if (index >= 0) {
                    switch (m_dragHandleType) {
                        case AngleHandleType::POINT1:
                            m_measurements[index].point1 = pos;
                            break;
                        case AngleHandleType::VERTEX:
                            m_measurements[index].vertex = pos;
                            break;
                        case AngleHandleType::POINT2:
                            m_measurements[index].point2 = pos;
                            break;
                        default:
                            break;
                    }
                    m_measurements[index].calculateAngle();
                    emit angleModified(m_dragAngleId, m_measurements[index]);
                    emit measurementsChanged();
                }

                m_dragAngleId = -1;
                m_dragHandleType = AngleHandleType::NONE;
                m_editState = AngleEditState::NONE;
                emit cursorChanged(Qt::CrossCursor);

                // Clear hover state after dropping the handle
                if (m_lastHoveredAngleId != -1) {
                    m_lastHoveredAngleId = -1;
                    m_lastHoveredHandleType = AngleHandleType::NONE;
                    emit handleHoverCleared();
                }
                break;
            }

            default:
                break;
        }
    }
}

void AngleTool::onMouseWheel(int delta, const QPointF& pos) {
    Q_UNUSED(delta)
    Q_UNUSED(pos)
    // Angle tool doesn't use wheel
}

void AngleTool::reset() {
    m_editState = AngleEditState::NONE;
    m_currentAngle = MeasurementAngle();
    m_selectedAngleId = -1;
    m_dragAngleId = -1;
    m_dragHandleType = AngleHandleType::NONE;
    // Clear hover state
    if (m_lastHoveredAngleId != -1) {
        m_lastHoveredAngleId = -1;
        m_lastHoveredHandleType = AngleHandleType::NONE;
        emit handleHoverCleared();
    }
}

void AngleTool::clearMeasurements() {
    m_measurements.clear();
    m_currentAngle = MeasurementAngle();
    m_editState = AngleEditState::NONE;
    m_selectedAngleId = -1;
    m_dragAngleId = -1;
    m_dragHandleType = AngleHandleType::NONE;
    emit measurementsChanged();
}

bool AngleTool::hitTestHandle(const QPointF& pos, int& outAngleId, AngleHandleType& outHandleType) const {
    // Check handles on all measurements
    for (const auto& angle : m_measurements) {
        if (angle.isNearPoint1Handle(pos, HANDLE_HIT_THRESHOLD)) {
            outAngleId = angle.id;
            outHandleType = AngleHandleType::POINT1;
            return true;
        }
        if (angle.isNearVertexHandle(pos, HANDLE_HIT_THRESHOLD)) {
            outAngleId = angle.id;
            outHandleType = AngleHandleType::VERTEX;
            return true;
        }
        if (angle.isNearPoint2Handle(pos, HANDLE_HIT_THRESHOLD)) {
            outAngleId = angle.id;
            outHandleType = AngleHandleType::POINT2;
            return true;
        }
    }
    return false;
}

int AngleTool::hitTestAngle(const QPointF& pos) const {
    for (const auto& angle : m_measurements) {
        if (angle.isNearArm(pos, ARM_HIT_THRESHOLD)) {
            return angle.id;
        }
    }
    return -1;
}

void AngleTool::selectAngle(int angleId) {
    if (m_selectedAngleId != angleId) {
        // Deselect previous
        int prevIndex = findAngleIndex(m_selectedAngleId);
        if (prevIndex >= 0) {
            m_measurements[prevIndex].isSelected = false;
        }

        m_selectedAngleId = angleId;

        // Select new
        int newIndex = findAngleIndex(angleId);
        if (newIndex >= 0) {
            m_measurements[newIndex].isSelected = true;
        }

        emit angleSelected(angleId);
        emit measurementsChanged();
    }
}

void AngleTool::deleteAngle(int angleId) {
    int index = findAngleIndex(angleId);
    if (index >= 0) {
        m_measurements.removeAt(index);

        if (m_selectedAngleId == angleId) {
            m_selectedAngleId = -1;
        }
        if (m_dragAngleId == angleId) {
            m_dragAngleId = -1;
            m_dragHandleType = AngleHandleType::NONE;
            m_editState = AngleEditState::NONE;
        }

        emit angleDeleted(angleId);
        emit measurementsChanged();
    }
}

void AngleTool::deleteSelectedAngle() {
    if (m_selectedAngleId >= 0) {
        deleteAngle(m_selectedAngleId);
    }
}

int AngleTool::findAngleIndex(int angleId) const {
    if (angleId < 0) return -1;

    for (int i = 0; i < m_measurements.size(); ++i) {
        if (m_measurements[i].id == angleId) {
            return i;
        }
    }
    return -1;
}

} // namespace Etrek::ImageViewer::Tool
