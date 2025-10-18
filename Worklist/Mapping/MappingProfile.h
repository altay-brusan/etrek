#ifndef MAPPINGPROFILE_H
#define MAPPINGPROFILE_H

#pragma once

#include <QMap>
#include <QString>
#include <functional>
#include "WorklistEntry.h"

using Setter = std::function<void(Etrek::Worklist::Data::Entity::WorklistEntry&, const QString&)>;

QMap<QString, Setter> createWorklistFieldMap() {
    return {
        {"AccessionNumber", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "AccessionNumber";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientID", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "PatientID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientName", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "PatientName";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"OtherPatientID", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "OtherPatientID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientAge", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "PatientAge";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientSex", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "PatientSex";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"RequestingPhysician", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "RequestingPhysician";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"StudyInstanceUID", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "StudyInstanceUID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ProcedureCode", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ProcedureCode";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepID", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledAETitle", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ScheduledAETitle";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientBirthDate", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "PatientBirthDate";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientComments", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "PatientComments";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ProcedureDescription", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ProcedureDescription";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepDescription", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepDescription";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientAllergies", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "PatientAllergies";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ReferringPhysicianContact", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ReferringPhysicianContact";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"AdmissionID", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "AdmissionID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"VisitID", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "VisitID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ModalityType", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ModalityType";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepStatus", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepStatus";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"Priority", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "Priority";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"StudyDescription", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "StudyDescription";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"StudyDate", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "StudyDate";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"StudyTime", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "StudyTime";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"InsuranceInformation", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "InsuranceInformation";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"Status", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "Status";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PriorityCode", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "PriorityCode";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        // New entries
        {"ConsultingPhysicianName", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ConsultingPhysicianName";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepStartDate", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepStartDate";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepStartTime", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepStartTime";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledStationAETitle", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ScheduledStationAETitle";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepPriority", [](Etrek::Worklist::Data::Entity::WorklistEntry& m, const QString& v) {
             Etrek::Worklist::Data::Entity::WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepPriority";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }}
    };
}




#endif // MAPPINGPROFILE_H
