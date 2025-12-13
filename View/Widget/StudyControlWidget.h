#ifndef STUDYCONTROLWIDGET_H
#define STUDYCONTROLWIDGET_H

#include <QWidget>
#include <QVector>

// Forward declarations
namespace Etrek::ScanProtocol::Data::Entity {
    class View;
}

namespace Ui {
class StudyControlWidget;
}

class StudyControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StudyControlWidget(QWidget *parent = nullptr);
    ~StudyControlWidget();

    /**
     * @brief Populates the view list table with the provided views.
     * @param views Vector of views to display in the table.
     */
    void setViews(const QVector<Etrek::ScanProtocol::Data::Entity::View>& views);

    /**
     * @brief Gets the currently selected view row index.
     * @return Selected row index, or -1 if no selection.
     */
    int getSelectedViewIndex() const;

signals:
    /**
     * @brief Emitted when user clicks Delete View button.
     * @param viewIndex The index of the view to delete.
     */
    void deleteViewRequested(int viewIndex);

    /**
     * @brief Emitted when user clicks Cancel Study button.
     */
    void cancelStudyRequested();

private slots:
    void onDeleteViewClicked();

private:
    Ui::StudyControlWidget *ui;
};

#endif // STUDYCONTROLWIDGET_H
