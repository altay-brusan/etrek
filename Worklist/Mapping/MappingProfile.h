#ifndef MAPPINGPROFILE_H
#define MAPPINGPROFILE_H

#pragma once

#include <QMap>
#include <QString>
#include <functional>
#include "WorklistEntry.h"


using namespace Etrek::Worklist::Data::Entity;


using Setter = std::function<void(WorklistEntry&, const QString&)>;

QMap<QString, Setter> createWorklistFieldMap() {
    return {
        {"AccessionNumber", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "AccessionNumber";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientID", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "PatientID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientName", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "PatientName";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"OtherPatientID", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "OtherPatientID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientAge", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "PatientAge";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientSex", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "PatientSex";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"RequestingPhysician", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "RequestingPhysician";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"StudyInstanceUID", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "StudyInstanceUID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ProcedureCode", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ProcedureCode";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepID", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledAETitle", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ScheduledAETitle";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientBirthDate", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "PatientBirthDate";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientComments", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "PatientComments";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ProcedureDescription", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ProcedureDescription";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepDescription", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepDescription";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PatientAllergies", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "PatientAllergies";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ReferringPhysicianContact", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ReferringPhysicianContact";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"AdmissionID", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "AdmissionID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"VisitID", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "VisitID";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ModalityType", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ModalityType";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepStatus", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepStatus";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"Priority", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "Priority";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"StudyDescription", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "StudyDescription";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"StudyDate", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "StudyDate";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"StudyTime", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "StudyTime";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"InsuranceInformation", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "InsuranceInformation";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"Status", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "Status";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"PriorityCode", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "PriorityCode";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        // New entries
        {"ConsultingPhysicianName", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ConsultingPhysicianName";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepStartDate", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepStartDate";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepStartTime", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepStartTime";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledStationAETitle", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ScheduledStationAETitle";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }},
        {"ScheduledProcedureStepPriority", [](WorklistEntry& m, const QString& v) {
             WorklistAttribute attr;
             attr.Tag.Name = "ScheduledProcedureStepPriority";
             attr.TagValue = v;
             m.Attributes.append(attr);
         }}
    };
}




#endif // MAPPINGPROFILE_H
