#ifndef IIMAGEREPOSITORY_H
#define IIMAGEREPOSITORY_H

#include "Core/Data/Entity/Image.h"
#include "Specification/Result.h"
#include "Worklist/Specification/WorklistEnum.h"
#include <QString>

namespace Etrek::Core::Repository {

/**
 * @interface IImageRepository
 * @brief Interface for image data access operations with status tracking.
 */
class IImageRepository {
public:
    virtual ~IImageRepository() = default;

    /**
     * @brief Updates the status of an image.
     * @param imageId The ID of the image to update.
     * @param status The new status.
     * @param reason Optional reason for the status change.
     * @return Result indicating success or failure.
     */
    virtual Etrek::Specification::Result<bool> updateStatus(
        int imageId,
        ProcedureStepStatus status,
        const QString& reason = QString()) = 0;

    /**
     * @brief Retrieves an image by ID.
     * @param imageId The ID of the image.
     * @return Result containing the Image entity or an error.
     */
    virtual Etrek::Specification::Result<Etrek::Core::Data::Entity::Image> getImage(int imageId) const = 0;
};

} // namespace Etrek::Core::Repository

#endif // IIMAGEREPOSITORY_H
