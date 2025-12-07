#ifndef EXAMPAGE_H
#define EXAMPAGE_H

#include <QWidget>

// Forward declarations
class QVTKOpenGLNativeWidget;
class QFrame;
class QGroupBox;
class ExamTitleWidget;
class StudyControlWidget;
class BodySizeWidget;
class ExposureApplicationControlWidget;
class ExposureControlWidget;

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
     * @brief Gets the exam title widget for displaying patient demographics.
     * @return Pointer to the exam title widget.
     */
    ExamTitleWidget* getExamTitleWidget() const;

    /**
     * @brief Gets the study control widget for displaying view list and study controls.
     * @return Pointer to the study control widget.
     */
    StudyControlWidget* getStudyControlWidget() const;

    /**
     * @brief Gets the body size widget for patient size selection.
     * @return Pointer to the body size widget.
     */
    BodySizeWidget* getBodySizeWidget() const;

    /**
     * @brief Gets the exposure application control widget for application mode selection.
     * @return Pointer to the exposure application control widget.
     */
    ExposureApplicationControlWidget* getExposureApplicationControlWidget() const;

    /**
     * @brief Gets the exposure control widget for technical parameters.
     * @return Pointer to the exposure control widget.
     */
    ExposureControlWidget* getExposureControlWidget() const;

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
    ExamTitleWidget *m_examTitleWidget{nullptr};
    StudyControlWidget *m_studyControlWidget{nullptr};
    BodySizeWidget *m_bodySizeWidget{nullptr};
    ExposureApplicationControlWidget *m_exposureApplicationControlWidget{nullptr};
    ExposureControlWidget *m_exposureControlWidget{nullptr};
};

#endif // EXAMPAGE_H
