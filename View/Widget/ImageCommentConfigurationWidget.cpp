#include "ImageCommentConfigurationWidget.h"
#include "ui_ImageCommentConfigurationWidget.h"

#include <QComboBox>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QVariant>

static bool parseIsReject(const QString& s)
{
    const auto t = s.trimmed().toLower();
    return (t == "1" || t == "true" || t == "yes" || t == "y");
}

ImageCommentConfigurationWidget::ImageCommentConfigurationWidget(
    const QVector<ImageComment>& comments, QWidget* parent)
    : QWidget(parent)
    , m_comments(comments)
    , ui(new Ui::ImageCommentConfigurationWidget)
{
    ui->setupUi(this);

    // Editable for free typing (new headings)
    ui->acceptedCommentHeadingComboBox->setEditable(true);
    ui->acceptedCommentHeadingComboBox->setInsertPolicy(QComboBox::NoInsert);
    ui->rejectedCommentHeadingComboBox->setEditable(true);
    ui->rejectedCommentHeadingComboBox->setInsertPolicy(QComboBox::NoInsert);

    // 1) Split
    m_acceptedComments.clear();
    m_rejectedComments.clear();
    m_acceptedComments.reserve(m_comments.size());
    m_rejectedComments.reserve(m_comments.size());

    for (const auto& c : m_comments) {
        if (parseIsReject(c.IsRejectComment))
            m_rejectedComments.push_back(c);
        else
            m_acceptedComments.push_back(c);
    }

    // 2) Fill combos
    auto fillCombo = [](QComboBox* box, const QVector<ImageComment>& list) {
        box->clear();
        for (const auto& c : list) {
            const int idx = box->count();
            box->addItem(c.Heading);
            box->setItemData(idx, c.Id, Qt::UserRole);
            box->setItemData(idx, c.Comment, Qt::UserRole + 1);
        }
        box->setCurrentIndex(-1);
        };

    fillCombo(ui->acceptedCommentHeadingComboBox, m_acceptedComments);
    fillCombo(ui->rejectedCommentHeadingComboBox, m_rejectedComments);

    // Disable autocompletion to prevent snap-to-item
    if (ui->acceptedCommentHeadingComboBox->completer())
        ui->acceptedCommentHeadingComboBox->setCompleter(nullptr);
    if (ui->rejectedCommentHeadingComboBox->completer())
        ui->rejectedCommentHeadingComboBox->setCompleter(nullptr);

    // Helper: load selected item's text ? editor
    auto applySelection = [](QComboBox* box, auto* edit, int idx) {
        if (idx >= 0) {
            const auto text = box->itemData(idx, Qt::UserRole + 1).toString();
            edit->setPlainText(text);
        }
        else {
            edit->clear();
        }
        };

    // Only on explicit pick from dropdown
    auto wireActivated = [&](QComboBox* box, auto* edit) {
        QObject::connect(box, qOverload<int>(&QComboBox::activated),
            box, [=](int idx) { applySelection(box, edit, idx); });
        };

    // While typing:
    // - If exact match exists ? preview the stored comment text (do NOT change index)
    // - Else ? clear the text box (new comment flow)
    auto wireEditText = [&](QComboBox* box, auto* edit) {
        // make typing UX nicer
        if (auto* le = box->lineEdit()) {
            le->setPlaceholderText(QStringLiteral("Type a new heading or pick from list…"));
            le->setClearButtonEnabled(true);
        }

        QObject::connect(box, &QComboBox::editTextChanged, box,
            [=](const QString& t) {
                const int found = box->findText(t, Qt::MatchFixedString); // exact, case-insensitive
                if (found >= 0) {
                    // preview only; DO NOT call setCurrentIndex here
                    const auto txt = box->itemData(found, Qt::UserRole + 1).toString();
                    edit->setPlainText(txt);
                }
                else {
                    edit->clear(); // brand-new heading path
                }
            });
        };

    // Wire up
    wireActivated(ui->acceptedCommentHeadingComboBox, ui->acceptedCommentTextEdit);
    wireActivated(ui->rejectedCommentHeadingComboBox, ui->rejectedCommentTextEdit);
    wireEditText(ui->acceptedCommentHeadingComboBox, ui->acceptedCommentTextEdit);
    wireEditText(ui->rejectedCommentHeadingComboBox, ui->rejectedCommentTextEdit);
}

ImageCommentConfigurationWidget::~ImageCommentConfigurationWidget()
{
	delete ui;
}
bool ImageCommentConfigurationWidget::isNewAcceptedHeading() const
{
    const auto* box = ui->acceptedCommentHeadingComboBox;
    return (box->currentIndex() == -1) && !box->currentText().trimmed().isEmpty();
}

bool ImageCommentConfigurationWidget::isNewRejectedHeading() const
{
    const auto* box = ui->rejectedCommentHeadingComboBox;
    return (box->currentIndex() == -1) && !box->currentText().trimmed().isEmpty();
}

