// ViewSelectionDialogDelegate.h
#ifndef VIEWSELECTIONDIALOGDELEGATE_H
#define VIEWSELECTIONDIALOGDELEGATE_H

#include <QObject>
#include <QPointer>
#include <memory>
#include "IDelegate.h"
#include "ViewSelectionDialog.h"

namespace Etrek::ScanProtocol::Repository {
    class ScanProtocolRepository;
}

namespace Etrek::ScanProtocol::Data::Entity {
    class Procedure;
    class View;
    class BodyPart;
}

namespace Etrek::Context {
    class IExaminationContext;
    class IContextManager;
}

namespace Etrek::Application::Delegate {

class ViewSelectionDialogDelegate : public QObject, public IDelegate
{
    Q_OBJECT
    Q_INTERFACES(IDelegate)

public:
    explicit ViewSelectionDialogDelegate(
        ViewSelectionDialog* dialog,
        std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanRepository,
        std::weak_ptr<Etrek::Context::IContextManager> contextManager,
        QObject* parent = nullptr);

    ~ViewSelectionDialogDelegate() override;

    QString name() const override;
    void attachDelegates(const QVector<QObject*>& delegates) override;

    /**
     * @brief Initializes the dialog with patient context and loads procedures
     */
    void initialize();

    /**
     * @brief Returns the selected procedure ID
     */
    int getSelectedProcedureId() const;

    /**
     * @brief Returns the selected view IDs
     */
    QVector<int> getSelectedViewIds() const;

signals:
    /**
     * @brief Emitted when examination should start with selected views
     * @param procedureId The selected procedure ID
     * @param viewIds The selected view IDs
     */
    void examinationReady(int procedureId, const QVector<int>& viewIds);

private slots:
    void onStartExamination();
    void onProcedureSelected(int procedureId);

private:
    void loadPatientInfoFromContext();
    void loadProceduresForBodyPart(const QString& bodyPartName);

    ViewSelectionDialog* m_dialog;
    std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> m_scanRepository;
    std::weak_ptr<Etrek::Context::IContextManager> m_contextManager;
    std::shared_ptr<Etrek::Context::IExaminationContext> m_examinationContext;
};

} // namespace Etrek::Application::Delegate

#endif // VIEWSELECTIONDIALOGDELEGATE_H
