#ifndef ETREK_DICOM_DATA_ENTITY_ENTITYSTATUS_H
#define ETREK_DICOM_DATA_ENTITY_ENTITYSTATUS_H

#include <QString>
#include <QDateTime>

namespace Etrek::Dicom::Data::Entity {

    /**
     * @brief Entity type enumeration for status tracking
     */
    enum class EntityType {
        PATIENT,
        STUDY,
        SERIES,
        IMAGE
    };

    /**
     * @brief Workflow status enumeration
     */
    enum class WorkflowStatus {
        SCHEDULED,
        PENDING,
        COMPLETED,
        CANCELLED,
        IN_PROGRESS,
        ABORTED
    };

    /**
     * @brief Priority level for workflow management
     */
    enum class Priority {
        URGENT,
        HIGH,
        NORMAL,
        LOW
    };

    /**
     * @brief Represents a status record for any DICOM entity (Patient, Study, Series, or Image)
     *
     * This class provides unified status tracking with full audit trail across the DICOM hierarchy.
     * Multiple status records can exist for each entity, enabling complete status history tracking.
     */
    class EntityStatus {
    public:
        int Id = -1;
        EntityType Type;                          // Type of entity (PATIENT, STUDY, SERIES, IMAGE)
        int EntityId = -1;                        // ID of the entity being tracked
        WorkflowStatus Status;                    // Current workflow status
        QString StatusReason;                     // Optional reason for status (cancellation, delay, etc.)
        Priority PriorityLevel = Priority::NORMAL; // Priority level for workflow management
        int AssignedTo = -1;                      // User ID of assigned person (FK to users table)
        int TransitionedBy = -1;                  // User ID who made this change (FK to users table)
        QDateTime TransitionedAt;                 // When status changed
        QString Notes;                            // Additional notes from technician/user

        /**
         * @brief Default constructor
         */
        EntityStatus() = default;

        /**
         * @brief Checks if this status entity is valid (has been persisted to database)
         * @return true if the status has a valid database ID (Id >= 0)
         */
        bool IsValid() const { return Id >= 0; }

        /**
         * @brief Convert EntityType enum to database string representation
         */
        static QString EntityTypeToString(EntityType type) {
            switch (type) {
                case EntityType::PATIENT: return "PATIENT";
                case EntityType::STUDY: return "STUDY";
                case EntityType::SERIES: return "SERIES";
                case EntityType::IMAGE: return "IMAGE";
                default: return "UNKNOWN";
            }
        }

        /**
         * @brief Convert database string to EntityType enum
         */
        static EntityType StringToEntityType(const QString& str) {
            if (str == "PATIENT") return EntityType::PATIENT;
            if (str == "STUDY") return EntityType::STUDY;
            if (str == "SERIES") return EntityType::SERIES;
            if (str == "IMAGE") return EntityType::IMAGE;
            return EntityType::PATIENT; // Default fallback
        }

        /**
         * @brief Convert WorkflowStatus enum to database string representation
         */
        static QString WorkflowStatusToString(WorkflowStatus status) {
            switch (status) {
                case WorkflowStatus::SCHEDULED: return "SCHEDULED";
                case WorkflowStatus::PENDING: return "PENDING";
                case WorkflowStatus::COMPLETED: return "COMPLETED";
                case WorkflowStatus::CANCELLED: return "CANCELLED";
                case WorkflowStatus::IN_PROGRESS: return "IN_PROGRESS";
                case WorkflowStatus::ABORTED: return "ABORTED";
                default: return "PENDING";
            }
        }

        /**
         * @brief Convert database string to WorkflowStatus enum
         */
        static WorkflowStatus StringToWorkflowStatus(const QString& str) {
            if (str == "SCHEDULED") return WorkflowStatus::SCHEDULED;
            if (str == "PENDING") return WorkflowStatus::PENDING;
            if (str == "COMPLETED") return WorkflowStatus::COMPLETED;
            if (str == "CANCELLED") return WorkflowStatus::CANCELLED;
            if (str == "IN_PROGRESS") return WorkflowStatus::IN_PROGRESS;
            if (str == "ABORTED") return WorkflowStatus::ABORTED;
            return WorkflowStatus::PENDING; // Default fallback
        }

        /**
         * @brief Convert Priority enum to database string representation
         */
        static QString PriorityToString(Priority priority) {
            switch (priority) {
                case Priority::URGENT: return "URGENT";
                case Priority::HIGH: return "HIGH";
                case Priority::NORMAL: return "NORMAL";
                case Priority::LOW: return "LOW";
                default: return "NORMAL";
            }
        }

        /**
         * @brief Convert database string to Priority enum
         */
        static Priority StringToPriority(const QString& str) {
            if (str == "URGENT") return Priority::URGENT;
            if (str == "HIGH") return Priority::HIGH;
            if (str == "NORMAL") return Priority::NORMAL;
            if (str == "LOW") return Priority::LOW;
            return Priority::NORMAL; // Default fallback
        }
    };

} // namespace Etrek::Dicom::Data::Entity

#endif // ETREK_DICOM_DATA_ENTITY_ENTITYSTATUS_H
