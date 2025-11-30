#ifndef VIEWSELECTIONDIALOG_H
#define VIEWSELECTIONDIALOG_H

#include <QDialog>
#include <QVector>
#include <QMap>
#include <QCheckBox>

namespace Etrek::ScanProtocol::Data::Entity {
    class Procedure;
    class View;
    class BodyPart;
}

namespace Ui {
class ViewSelectionDialog;
}

class QVTKOpenGLNativeWidget;
class vtkRenderer;
class vtkGenericOpenGLRenderWindow;

class ViewSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewSelectionDialog(QWidget *parent = nullptr);
    ~ViewSelectionDialog();

    // Patient information display
    void setPatientInfo(const QString& patientName, const QString& patientId,
                       const QString& bodyPart, const QString& accessionNo);

    // Procedures and views
    void setProcedures(const QVector<Etrek::ScanProtocol::Data::Entity::Procedure>& procedures);

    // Get selected data
    int getSelectedProcedureId() const;
    QVector<int> getSelectedViewIds() const;

signals:
    void procedureSelected(int procedureId);
    void viewSelectionChanged();
    void startExamination();

private slots:
    void onProcedureChanged(int index);
    void onViewCheckboxToggled(int viewId, bool checked);
    void onStartExaminationClicked();
    void updateStartButtonState();

private:
    void setupConnections();
    void loadViewsForProcedure(int procedureIndex);
    void clearViewsLayout();
    QWidget* createViewWidget(const Etrek::ScanProtocol::Data::Entity::View& view);
    void initializeVTK();

    Ui::ViewSelectionDialog *ui;
    QVector<Etrek::ScanProtocol::Data::Entity::Procedure> m_procedures;
    QMap<int, QCheckBox*> m_viewCheckboxes;  // viewId -> checkbox
    int m_selectedProcedureId = -1;

    // VTK members for future 3D visualization
    QVTKOpenGLNativeWidget* m_vtkWidget = nullptr;
    vtkRenderer* m_renderer = nullptr;
    vtkGenericOpenGLRenderWindow* m_renderWindow = nullptr;
};

#endif // VIEWSELECTIONDIALOG_H
