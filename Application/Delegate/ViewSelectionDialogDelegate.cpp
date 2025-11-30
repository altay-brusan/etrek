#include "ViewSelectionDialogDelegate.h"
#include "ScanProtocolRepository.h"
#include "IExaminationContext.h"
#include "IContextManager.h"
#include "Procedure.h"
#include "View.h"
#include "BodyPart.h"

#include <QMessageBox>

namespace Etrek::Application::Delegate {

namespace spc_ent = Etrek::ScanProtocol::Data::Entity;

ViewSelectionDialogDelegate::ViewSelectionDialogDelegate(
    ViewSelectionDialog* dialog,
    std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanRepository,
    std::weak_ptr<Etrek::Context::IContextManager> contextManager,
    QObject* parent)
    : QObject(parent)
    , m_dialog(dialog)
    , m_scanRepository(scanRepository)
    , m_contextManager(contextManager)
{
    // Connect dialog signals
    connect(m_dialog, &ViewSelectionDialog::startExamination,
            this, &ViewSelectionDialogDelegate::onStartExamination);
    connect(m_dialog, &ViewSelectionDialog::procedureSelected,
            this, &ViewSelectionDialogDelegate::onProcedureSelected);
}

ViewSelectionDialogDelegate::~ViewSelectionDialogDelegate()
{
}

QString ViewSelectionDialogDelegate::name() const
{
    return QStringLiteral("ViewSelectionDialogDelegate");
}

void ViewSelectionDialogDelegate::attachDelegates(const QVector<QObject*>& delegates)
{
    Q_UNUSED(delegates);
    // No child delegates needed for this dialog
}

void ViewSelectionDialogDelegate::initialize()
{
    // Retrieve examination context from ContextManager
    if (auto ctxMgr = m_contextManager.lock()) {
        auto workflowCtx = ctxMgr->workflowContext("Examination");
        if (workflowCtx) {
            m_examinationContext = std::dynamic_pointer_cast<Etrek::Context::IExaminationContext>(workflowCtx);

            if (m_examinationContext) {
                loadPatientInfoFromContext();

                // Load procedures for the patient's body part
                QString bodyPart = m_examinationContext->bodyPartExamined();
                if (!bodyPart.isEmpty()) {
                    loadProceduresForBodyPart(bodyPart);
                } else {
                    QMessageBox::warning(m_dialog, "Missing Information",
                                       "Body part information is not available in the examination context.");
                }
            } else {
                QMessageBox::warning(m_dialog, "Context Error",
                                   "Failed to retrieve examination context.");
            }
        } else {
            QMessageBox::warning(m_dialog, "Context Error",
                               "No examination context is available. Please select a patient from the worklist first.");
        }
    } else {
        QMessageBox::warning(m_dialog, "Context Error",
                           "Context manager is not available.");
    }
}

void ViewSelectionDialogDelegate::loadPatientInfoFromContext()
{
    if (!m_examinationContext)
        return;

    QString patientName = m_examinationContext->patientName();
    QString patientId = m_examinationContext->patientId();
    QString bodyPart = m_examinationContext->bodyPartExamined();
    QString accessionNo = m_examinationContext->accessionNumber();

    m_dialog->setPatientInfo(patientName, patientId, bodyPart, accessionNo);
}

void ViewSelectionDialogDelegate::loadProceduresForBodyPart(const QString& bodyPartName)
{
    if (!m_scanRepository) {
        QMessageBox::warning(m_dialog, "Repository Error",
                           "Scan protocol repository is not available.");
        return;
    }

    // First, get all body parts and find the one by name
    auto allBodyPartsResult = m_scanRepository->getAllBodyParts();
    if (!allBodyPartsResult.isSuccess) {
        QMessageBox::warning(m_dialog, tr("Error"),
                           QString(tr("Failed to load body parts: %1")).arg(allBodyPartsResult.message));
        return;
    }

    // Find the body part by name
    spc_ent::BodyPart bodyPart;
    bool found = false;
    for (const auto& bp : allBodyPartsResult.value) {
        if (bp.Name == bodyPartName) {
            bodyPart = bp;
            found = true;
            break;
        }
    }

    if (!found) {
        QMessageBox::warning(m_dialog, tr("Body Part Not Found"),
                           QString(tr("Could not find body part: %1")).arg(bodyPartName));
        return;
    }

    // Get all procedures for this body part
    auto proceduresResult = m_scanRepository->getProceduresByBodyPart(bodyPart.Id);
    if (!proceduresResult.isSuccess) {
        QMessageBox::warning(m_dialog, tr("Error"),
                           QString(tr("Failed to load procedures: %1")).arg(proceduresResult.message));
        return;
    }

    if (proceduresResult.value.isEmpty()) {
        QMessageBox::information(m_dialog, "No Procedures",
                                QString("No procedures are configured for body part: %1").arg(bodyPartName));
        return;
    }

    // For each procedure, load its views
    QVector<spc_ent::Procedure> proceduresWithViews;
    for (const auto& proc : proceduresResult.value) {
        spc_ent::Procedure procedure = proc;  // Make a copy to modify
        // Get views for this procedure
        auto viewsResult = m_scanRepository->getViewsForProcedure(procedure.Id);
        if (viewsResult.isSuccess) {
            procedure.Views = viewsResult.value;
        }
        proceduresWithViews.append(procedure);
    }

    m_dialog->setProcedures(proceduresWithViews);
}

void ViewSelectionDialogDelegate::onProcedureSelected(int procedureId)
{
    Q_UNUSED(procedureId);
    // Could add additional logic here when procedure changes
}

void ViewSelectionDialogDelegate::onStartExamination()
{
    int procedureId = m_dialog->getSelectedProcedureId();
    QVector<int> viewIds = m_dialog->getSelectedViewIds();

    if (procedureId <= 0 || viewIds.isEmpty()) {
        QMessageBox::warning(m_dialog, "Invalid Selection",
                           "Please select a procedure and at least one view.");
        return;
    }

    emit examinationReady(procedureId, viewIds);
}

int ViewSelectionDialogDelegate::getSelectedProcedureId() const
{
    return m_dialog->getSelectedProcedureId();
}

QVector<int> ViewSelectionDialogDelegate::getSelectedViewIds() const
{
    return m_dialog->getSelectedViewIds();
}

} // namespace Etrek::Application::Delegate
