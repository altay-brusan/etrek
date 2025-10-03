#ifndef VIEWCONFIGURATIONWIDGET_H
#define VIEWCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QHash>
#include <QStandardItemModel>
#include <QComboBox>

#include "View.h"      // Etrek::ScanProtocol::Data::Entity::View
#include "BodyPart.h"  // included by View.h, but safe

namespace Ui { class ViewConfigurationWidget; }

// Short aliases to keep code tidy
namespace ESP = Etrek::ScanProtocol;
namespace ESPDE = Etrek::ScanProtocol::Data::Entity;
using ViewEntity = ESPDE::View;

class ViewConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewConfigurationWidget(const QVector<ViewEntity>& views, QWidget* parent = nullptr);
    ~ViewConfigurationWidget();

signals:
    void viewSelected(int viewId);

private slots:
    void onViewSelectionChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    // --- helpers ---
    void setStile();
    void buildViewList();
    void buildBodyPartComboFromViews();
    void buildAuxCombosFromViews(); // seed combos from existing data
    void loadViewToForm(const ViewEntity& v);

    // static utils so they can be called from lambdas without capturing 'this'
    static int  findComboIndexByText(QComboBox* cb, const QString& text,
        Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    static void setComboByText(QComboBox* cb, const QString& text);

private:
    enum Roles { ViewIdRole = Qt::UserRole + 410 };

    Ui::ViewConfigurationWidget* ui = nullptr;

    QVector<ViewEntity>   m_parameters;      // all views
    QStandardItemModel* m_viewModel = nullptr;
    QHash<int, ViewEntity> m_viewById;       // View.Id -> View
};

#endif // VIEWCONFIGURATIONWIDGET_H
