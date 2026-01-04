#ifndef ETREK_DICOM_DATA_ENTITY_MWLTASKMAPPING_H
#define ETREK_DICOM_DATA_ENTITY_MWLTASKMAPPING_H

namespace Etrek::Dicom::Data::Entity {

/**
 * @brief Maps MWL entries to the DICOM objects created during examination.
 *
 * This entity links a worklist entry and procedure to all the DICOM records
 * created during an examination: study, series, images, SOP common, and acquisition.
 * This enables tracking the complete chain from worklist to final images.
 */
class MwlTaskMapping {
public:
    int MwlEntryId = -1;      // FK to mwl_entries
    int ProcedureId = -1;     // FK to procedures (composite PK with MwlEntryId)
    int StudyId = -1;         // FK to studies
    int SeriesId = -1;        // FK to series
    int ImagesId = -1;        // FK to images
    int SopCommonId = -1;     // FK to sop_commons
    int AcquisitionId = -1;   // FK to acquisitions

    MwlTaskMapping() = default;

    /**
     * @brief Checks if this mapping has valid primary key values
     * @return true if both MwlEntryId and ProcedureId are set
     */
    bool isValid() const { return MwlEntryId >= 0 && ProcedureId >= 0; }

    /**
     * @brief Checks if all required DICOM chain IDs are populated
     * @return true if all foreign keys are set
     */
    bool isComplete() const {
        return MwlEntryId >= 0 &&
               ProcedureId >= 0 &&
               StudyId >= 0 &&
               SeriesId >= 0 &&
               ImagesId >= 0 &&
               SopCommonId >= 0 &&
               AcquisitionId >= 0;
    }
};

/**
 * @brief Full DICOM chain lookup result from MWL to acquisition
 */
struct DicomChain {
    int MwlEntryId = -1;
    int ProcedureId = -1;
    int StudyId = -1;
    int SeriesId = -1;
    int ImageId = -1;
    int SopCommonId = -1;
    int AcquisitionId = -1;

    bool isValid() const { return MwlEntryId >= 0 && StudyId >= 0; }
};

} // namespace Etrek::Dicom::Data::Entity

#endif // ETREK_DICOM_DATA_ENTITY_MWLTASKMAPPING_H
