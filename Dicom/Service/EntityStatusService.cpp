#include "EntityStatusService.h"
#include "DicomRepository.h"
#include <QDateTime>

namespace Etrek::Dicom::Service {

    EntityStatusService::EntityStatusService(
        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> repository,
        QObject* parent)
        : QObject(parent)
        , m_repository(std::move(repository))
    {
    }

    EntityStatusService::~EntityStatusService() = default;

    Result<EntityStatus> EntityStatusService::startExamination(
        EntityType entityType,
        int entityId,
        int userId,
        Priority priority)
    {
        // Get current status to detect transition
        auto currentResult = m_repository->getCurrentStatus(entityType, entityId);
        std::optional<WorkflowStatus> oldStatus = std::nullopt;
        if (currentResult.isSuccess && currentResult.value.has_value()) {
            oldStatus = currentResult.value->Status;
        }

        EntityStatus status;
        status.Type = entityType;
        status.EntityId = entityId;
        status.Status = WorkflowStatus::IN_PROGRESS;
        status.PriorityLevel = priority;
        status.AssignedTo = userId;
        status.TransitionedBy = userId;
        status.TransitionedAt = QDateTime::currentDateTime();

        auto result = m_repository->insertEntityStatus(status);
        if (result.isSuccess) {
            emit statusChanged(entityType, entityId, oldStatus, WorkflowStatus::IN_PROGRESS);
        }
        return result;
    }

    Result<EntityStatus> EntityStatusService::scheduleEntity(
        EntityType entityType,
        int entityId,
        int userId,
        Priority priority)
    {
        EntityStatus status;
        status.Type = entityType;
        status.EntityId = entityId;
        status.Status = WorkflowStatus::SCHEDULED;
        status.PriorityLevel = priority;
        status.AssignedTo = -1; // Not assigned yet
        status.TransitionedBy = userId;
        status.TransitionedAt = QDateTime::currentDateTime();

        auto result = m_repository->insertEntityStatus(status);
        if (result.isSuccess) {
            emit statusChanged(entityType, entityId, std::nullopt, WorkflowStatus::SCHEDULED);
        }
        return result;
    }

    Result<EntityStatus> EntityStatusService::updateStatus(
        EntityType entityType,
        int entityId,
        WorkflowStatus newStatus,
        int userId,
        const QString& reason,
        const QString& notes)
    {
        // Get current status to detect transition
        auto currentResult = m_repository->getCurrentStatus(entityType, entityId);
        std::optional<WorkflowStatus> oldStatus = std::nullopt;
        if (currentResult.isSuccess && currentResult.value.has_value()) {
            oldStatus = currentResult.value->Status;
        }

        EntityStatus status;
        status.Type = entityType;
        status.EntityId = entityId;
        status.Status = newStatus;
        status.StatusReason = reason;
        status.Notes = notes;
        status.TransitionedBy = userId;
        status.TransitionedAt = QDateTime::currentDateTime();

        // Preserve assigned user if exists
        if (currentResult.isSuccess && currentResult.value.has_value()) {
            status.AssignedTo = currentResult.value->AssignedTo;
            status.PriorityLevel = currentResult.value->PriorityLevel;
        }

        auto result = m_repository->insertEntityStatus(status);
        if (result.isSuccess) {
            emit statusChanged(entityType, entityId, oldStatus, newStatus);
        }
        return result;
    }

    Result<EntityStatus> EntityStatusService::completeExamination(
        EntityType entityType,
        int entityId,
        int userId,
        const QString& notes)
    {
        return updateStatus(entityType, entityId, WorkflowStatus::COMPLETED, userId, QString(), notes);
    }

    Result<EntityStatus> EntityStatusService::cancelExamination(
        EntityType entityType,
        int entityId,
        int userId,
        const QString& reason)
    {
        return updateStatus(entityType, entityId, WorkflowStatus::CANCELLED, userId, reason, QString());
    }

    Result<EntityStatus> EntityStatusService::abortExamination(
        EntityType entityType,
        int entityId,
        int userId,
        const QString& reason)
    {
        return updateStatus(entityType, entityId, WorkflowStatus::ABORTED, userId, reason, QString());
    }

    Result<std::optional<EntityStatus>> EntityStatusService::getCurrentStatus(
        EntityType entityType,
        int entityId) const
    {
        return m_repository->getCurrentStatus(entityType, entityId);
    }

    Result<QVector<EntityStatus>> EntityStatusService::getStatusHistory(
        EntityType entityType,
        int entityId) const
    {
        return m_repository->getStatusHistory(entityType, entityId);
    }

    Result<QVector<EntityStatus>> EntityStatusService::getEntitiesByStatus(
        EntityType entityType,
        WorkflowStatus status) const
    {
        return m_repository->getEntitiesByStatus(entityType, status);
    }

    Result<QVector<EntityStatus>> EntityStatusService::getAssignedEntities(
        int userId,
        WorkflowStatus status) const
    {
        return m_repository->getAssignedEntities(userId, status);
    }

} // namespace Etrek::Dicom::Service
