#ifndef EXAMPAGE_H
#define EXAMPAGE_H

#include <QWidget>

// Forward declarations
class QVTKOpenGLNativeWidget;
class QFrame;
class QGroupBox;

namespace Ui {
class ExamPage;
}

class ExamPage : public QWidget
{
    Q_OBJECT

public:
    explicit ExamPage(QWidget *parent = nullptr);
    ~ExamPage();

    /**
     * @brief Gets the VTK image viewer widget for DICOM image display.
     * @return Pointer to the VTK OpenGL native widget.
     */
    QVTKOpenGLNativeWidget* getVtkImageViewer() const;

    /**
     * @brief Gets the title placeholder frame for patient info widget.
     * @return Pointer to the title placeholder frame.
     */
    QFrame* getTitlePlaceholder() const;

    /**
     * @brief Gets the thumbnail image list placeholder frame.
     * @return Pointer to the thumbnail placeholder frame.
     */
    QFrame* getThumbnailPlaceholder() const;

    /**
     * @brief Gets placeholder frames/groupboxes for various control widgets.
     */
    QGroupBox* getStudyPlaceholder() const;
    QGroupBox* getExposurePlaceholder() const;
    QGroupBox* getBodySizePlaceholder() const;
    QGroupBox* getGeneratorControlPlaceholder() const;

private:
    Ui::ExamPage *ui;
};

#endif // EXAMPAGE_H
