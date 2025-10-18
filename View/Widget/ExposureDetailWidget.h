#ifndef EXPOSUREDETAILWIDGET_H
#define EXPOSUREDETAILWIDGET_H

#include <QWidget>
#include "TechniqueParameter.h"

namespace Ui {
class ExposureDetailWidget;
}

class ExposureDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExposureDetailWidget(QWidget* parent = nullptr);
    ~ExposureDetailWidget();

    // fill & clear UI
    void load(const Etrek::ScanProtocol::Data::Entity::TechniqueParameter& tp);
    void clear();

    // read back UI into a TechniqueParameter (nullopt if all default/empty)
    std::optional<Etrek::ScanProtocol::Data::Entity::TechniqueParameter>
        snapshot(const Etrek::ScanProtocol::Data::Entity::BodyPart& bp,
            Etrek::ScanProtocol::TechniqueProfile profile,
            Etrek::ScanProtocol::BodySize size,
            Etrek::ScanProtocol::ExposureStyle exposureStyle,
            const QString& aecField,
            int focalSpot,
            int aecDensity) const;

private:
    Ui::ExposureDetailWidget *ui;
    void initStyles();
};

#endif // EXPOSUREDETAILWIDGET_H
