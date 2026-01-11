/**
 * @file EntityStatusService.h
 * @brief Service for DICOM entity workflow status management.
 *
 * @details Provides high-level workflow operations for managing entity status
 *          including examination start, completion, cancellation, and status tracking.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see EntityStatus
 * @see DicomRepository
 */

#ifndef ENTITYSTATUSSERVICE_H
#define ENTITYSTATUSSERVICE_H

#include <QObject>
#include <memory>
#include <optional>
#include "Result.h"
#include "EntityStatus.h"

namespace Etrek::Dicom::Repository {
    class DicomRepository;
}

/**
 * @namespace Etrek::Dicom::Service
 * @brief Contains service classes for DICOM business logic.
 */
namespace Etrek::Dicom::Service {

    using namespace Etrek::Dicom::Data::Entity;
    using namespace Etrek::Specification;

    /**
     * @brief Service layer for managing entity workflow status.
     *
     * Provides high-level workflow operations (startExamination, completeExamination, etc.)
     * on top of the DicomRepository's status tracking methods.
     */
    class EntityStatusService : public QObject
    {
        Q_OBJECT

    public:
        explicit EntityStatusService(
            std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> repository,
            QObject* parent = nullptr);

        ~EntityStatusService();

        /**
         * @brief Start an examination - sets status to IN_PROGRESS.
         * @param entityType Type of entity (typically STUDY)
         * @param entityId ID of the entity
         * @param userId ID of the user starting the examination
         * @param priority Optional priority level (defaults to NORMAL)
         * @return Result with the created EntityStatus
         */
        Result<EntityStatus> startExamination(
            EntityType entityType,
            int entityId,
            int userId,
            Priority priority = Priority::NORMAL);

        /**
         * @brief Schedule a new entity - sets status to SCHEDULED.
         * @param entityType Type of entity (typically STUDY)
         * @param entityId ID of the entity
         * @param userId ID of the user creating the schedule
         * @param priority Optional priority level (defaults to NORMAL)
         * @return Result with the created EntityStatus
         */
        Result<EntityStatus> scheduleEntity(
            EntityType entityType,
            int entityId,
            int userId,
            Priority priority = Priority::NORMAL);

        /**
         * @brief Update status with custom values.
         * @param entityType Type of entity
         * @param entityId ID of the entity
         * @param newStatus New workflow status
         * @param userId ID of the user making the change
         * @param reason Optional reason for the status change
         * @param notes Optional notes
         * @return Result with the created EntityStatus
         */
        Result<EntityStatus> updateStatus(
            EntityType entityType,
            int entityId,
            WorkflowStatus newStatus,
            int userId,
            const QString& reason = QString(),
            const QString& notes = QString());

        /**
         * @brief Complete an examination - sets status to COMPLETED.
         * @param entityType Type of entity
         * @param entityId ID of the entity
         * @param userId ID of the user completing the examination
         * @param notes Optional completion notes
         * @return Result with the created EntityStatus
         */
        Result<EntityStatus> completeExamination(
            EntityType entityType,
            int entityId,
            int userId,
            const QString& notes = QString());

        /**
         * @brief Cancel an examination - sets status to CANCELLED.
         * @param entityType Type of entity
         * @param entityId ID of the entity
         * @param userId ID of the user cancelling
         * @param reason Reason for cancellation
         * @return Result with the created EntityStatus
         */
        Result<EntityStatus> cancelExamination(
            EntityType entityType,
            int entityId,
            int userId,
            const QString& reason);

        /**
         * @brief Abort an examination - sets status to ABORTED.
         * @param entityType Type of entity
         * @param entityId ID of the entity
         * @param userId ID of the user aborting
         * @param reason Reason for abortion
         * @return Result with the created EntityStatus
         */
        Result<EntityStatus> abortExamination(
            EntityType entityType,
            int entityId,
            int userId,
            const QString& reason);

        /**
         * @brief Get the current status of an entity.
         * @param entityType Type of entity
         * @param entityId ID of the entity
         * @return Result with optional EntityStatus (nullopt if no status exists)
         */
        Result<std::optional<EntityStatus>> getCurrentStatus(
            EntityType entityType,
            int entityId) const;

        /**
         * @brief Get full status history for an entity.
         * @param entityType Type of entity
         * @param entityId ID of the entity
         * @return Result with vector of EntityStatus records
         */
        Result<QVector<EntityStatus>> getStatusHistory(
            EntityType entityType,
            int entityId) const;

        /**
         * @brief Get all entities with a specific status.
         * @param entityType Type of entity
         * @param status Status to filter by
         * @return Result with vector of EntityStatus records
         */
        Result<QVector<EntityStatus>> getEntitiesByStatus(
            EntityType entityType,
            WorkflowStatus status) const;

        /**
         * @brief Get entities assigned to a specific user with a given status.
         * @param userId User ID
         * @param status Status to filter by
         * @return Result with vector of EntityStatus records
         */
        Result<QVector<EntityStatus>> getAssignedEntities(
            int userId,
            WorkflowStatus status) const;

    signals:
        /**
         * @brief Emitted when an entity's status changes.
         * @param entityType Type of entity
         * @param entityId ID of the entity
         * @param oldStatus Previous status (nullopt if first status)
         * @param newStatus New status
         */
        void statusChanged(
            EntityType entityType,
            int entityId,
            std::optional<WorkflowStatus> oldStatus,
            WorkflowStatus newStatus);

    private:
        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> m_repository;
    };

} // namespace Etrek::Dicom::Service

#endif // ENTITYSTATUSSERVICE_H
