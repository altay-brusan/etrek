#include "SeriesThumbnailPanel.h"
#include "ui_SeriesThumbnailPanel.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

SeriesThumbnailPanel::SeriesThumbnailPanel(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SeriesThumbnailPanel)
    , m_listWidget(new QListWidget(this))
{
    ui->setupUi(this);
    setupUi();
}

SeriesThumbnailPanel::~SeriesThumbnailPanel() {
    delete ui;
}

void SeriesThumbnailPanel::setupUi() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Title label
    QLabel* titleLabel = new QLabel("Series", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   background-color: rgb(60, 60, 60);"
        "   color: rgb(200, 200, 200);"
        "   padding: 5px;"
        "   font-weight: bold;"
        "}"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // Configure list widget
    m_listWidget->setViewMode(QListWidget::ListMode);
    m_listWidget->setFlow(QListWidget::TopToBottom);
    m_listWidget->setSpacing(2);
    m_listWidget->setStyleSheet(
        "QListWidget {"
        "   background-color: rgb(40, 40, 40);"
        "   border: none;"
        "}"
        "QListWidget::item {"
        "   background-color: rgb(50, 50, 50);"
        "   border: 1px solid rgb(60, 60, 60);"
        "   border-radius: 3px;"
        "   padding: 5px;"
        "   color: rgb(200, 200, 200);"
        "}"
        "QListWidget::item:hover {"
        "   background-color: rgb(70, 70, 70);"
        "}"
        "QListWidget::item:selected {"
        "   background-color: rgb(0, 100, 180);"
        "   border: 1px solid rgb(0, 150, 255);"
        "}"
    );

    layout->addWidget(m_listWidget);
    setLayout(layout);

    // Connect signals
    connect(m_listWidget, &QListWidget::itemClicked,
            this, &SeriesThumbnailPanel::onItemClicked);
    connect(m_listWidget, &QListWidget::itemDoubleClicked,
            this, &SeriesThumbnailPanel::onItemDoubleClicked);
}

void SeriesThumbnailPanel::clear() {
    m_listWidget->clear();
    m_thumbnails.clear();
}

void SeriesThumbnailPanel::addThumbnail(const ThumbnailInfo& info) {
    m_thumbnails.append(info);

    QListWidgetItem* item = new QListWidgetItem(m_listWidget);
    item->setData(Qt::UserRole, info.id);
    item->setData(Qt::UserRole + 1, info.isSeries);

    // Create widget for item
    QWidget* widget = createThumbnailWidget(info);
    item->setSizeHint(widget->sizeHint());
    m_listWidget->setItemWidget(item, widget);
}

void SeriesThumbnailPanel::setThumbnails(const QVector<ThumbnailInfo>& thumbnails) {
    clear();
    for (const auto& info : thumbnails) {
        addThumbnail(info);
    }
}

int SeriesThumbnailPanel::selectedId() const {
    QListWidgetItem* current = m_listWidget->currentItem();
    if (current) {
        return current->data(Qt::UserRole).toInt();
    }
    return -1;
}

void SeriesThumbnailPanel::setSelectedId(int id) {
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        if (item->data(Qt::UserRole).toInt() == id) {
            m_listWidget->setCurrentItem(item);
            break;
        }
    }
}

int SeriesThumbnailPanel::count() const {
    return m_listWidget->count();
}

void SeriesThumbnailPanel::setExpanded(bool expanded) {
    m_expanded = expanded;
    setVisible(expanded);
}

bool SeriesThumbnailPanel::isExpanded() const {
    return m_expanded;
}

QWidget* SeriesThumbnailPanel::createThumbnailWidget(const ThumbnailInfo& info) {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(2);

    // Thumbnail image (if available)
    if (!info.thumbnail.isNull()) {
        QLabel* imageLabel = new QLabel(widget);
        imageLabel->setPixmap(info.thumbnail.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setStyleSheet("background-color: rgb(30, 30, 30); border: none;");
        layout->addWidget(imageLabel);
    } else {
        // Placeholder
        QLabel* placeholder = new QLabel(widget);
        placeholder->setFixedSize(80, 60);
        placeholder->setAlignment(Qt::AlignCenter);
        placeholder->setText(info.isSeries ? "Series" : "Image");
        placeholder->setStyleSheet(
            "QLabel {"
            "   background-color: rgb(30, 30, 30);"
            "   color: rgb(100, 100, 100);"
            "   border: 1px solid rgb(50, 50, 50);"
            "}"
        );
        layout->addWidget(placeholder);
    }

    // Label
    QLabel* label = new QLabel(info.label, widget);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    label->setStyleSheet("color: rgb(200, 200, 200); font-size: 10px; border: none; background: transparent;");
    layout->addWidget(label);

    // Image count (for series)
    if (info.isSeries && info.imageCount > 0) {
        QLabel* countLabel = new QLabel(QString("%1 images").arg(info.imageCount), widget);
        countLabel->setAlignment(Qt::AlignCenter);
        countLabel->setStyleSheet("color: rgb(150, 150, 150); font-size: 9px; border: none; background: transparent;");
        layout->addWidget(countLabel);
    }

    widget->setLayout(layout);
    widget->setMinimumWidth(100);
    return widget;
}

void SeriesThumbnailPanel::onItemClicked(QListWidgetItem* item) {
    if (item) {
        int id = item->data(Qt::UserRole).toInt();
        bool isSeries = item->data(Qt::UserRole + 1).toBool();
        emit thumbnailClicked(id, isSeries);
    }
}

void SeriesThumbnailPanel::onItemDoubleClicked(QListWidgetItem* item) {
    if (item) {
        int id = item->data(Qt::UserRole).toInt();
        bool isSeries = item->data(Qt::UserRole + 1).toBool();
        emit thumbnailDoubleClicked(id, isSeries);
    }
}
