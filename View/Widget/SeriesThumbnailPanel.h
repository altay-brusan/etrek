#ifndef SERIESTHUMBNAILPANEL_H
#define SERIESTHUMBNAILPANEL_H

#include <QWidget>
#include <QVector>

class QListWidget;
class QListWidgetItem;

namespace Ui {
class SeriesThumbnailPanel;
}

/**
 * @struct ThumbnailInfo
 * @brief Information for a thumbnail item.
 */
struct ThumbnailInfo {
    int id = -1;                // Series or Instance ID
    QString label;              // Display text
    QString description;        // Optional description
    QPixmap thumbnail;          // Thumbnail image (optional)
    int imageCount = 0;         // Number of images in series
    bool isSeries = true;       // True if series, false if instance
};

/**
 * @class SeriesThumbnailPanel
 * @brief Right panel showing series/image thumbnails for navigation.
 *
 * Displays a scrollable list of thumbnail items representing
 * series or individual images within a study.
 */
class SeriesThumbnailPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SeriesThumbnailPanel(QWidget* parent = nullptr);
    ~SeriesThumbnailPanel() override;

    /**
     * @brief Clear all thumbnails.
     */
    void clear();

    /**
     * @brief Add a thumbnail item.
     */
    void addThumbnail(const ThumbnailInfo& info);

    /**
     * @brief Set all thumbnails at once.
     */
    void setThumbnails(const QVector<ThumbnailInfo>& thumbnails);

    /**
     * @brief Get the selected thumbnail ID.
     */
    int selectedId() const;

    /**
     * @brief Set the selected thumbnail by ID.
     */
    void setSelectedId(int id);

    /**
     * @brief Get number of items.
     */
    int count() const;

    /**
     * @brief Set whether panel is visible.
     */
    void setExpanded(bool expanded);
    bool isExpanded() const;

signals:
    /**
     * @brief Emitted when a thumbnail is clicked.
     */
    void thumbnailClicked(int id, bool isSeries);

    /**
     * @brief Emitted when a thumbnail is double-clicked.
     */
    void thumbnailDoubleClicked(int id, bool isSeries);

private slots:
    void onItemClicked(QListWidgetItem* item);
    void onItemDoubleClicked(QListWidgetItem* item);

private:
    void setupUi();
    QWidget* createThumbnailWidget(const ThumbnailInfo& info);

    Ui::SeriesThumbnailPanel* ui;
    QListWidget* m_listWidget;
    QVector<ThumbnailInfo> m_thumbnails;
    bool m_expanded = true;
};

#endif // SERIESTHUMBNAILPANEL_H
