// OrientationDetailWidget.h
#ifndef ORIENTATIONDETAILWIDGET_H
#define ORIENTATIONDETAILWIDGET_H

#pragma once
#include <QWidget>
#include <QMap>
#include "TechniqueParameter.h"

namespace Ui { class OrientationDetailWidget; }
class ExposureDetailWidget;

using namespace Etrek::ScanProtocol;
using namespace Etrek::ScanProtocol::Data::Entity;

class OrientationDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OrientationDetailWidget(TechniqueProfile profile,
        const QVector<TechniqueParameter>* data,
        QWidget* parent = nullptr);
    ~OrientationDetailWidget();

    // NEW: snapshot all 4 size tabs for a given body part
    QVector<TechniqueParameter> collectForBodyPart(const BodyPart& bp) const;

public slots:
    void onBodyPartChanged(int bodyPartId);

private:
    Ui::OrientationDetailWidget* ui{};
    ExposureDetailWidget* m_large{};   // Fat
    ExposureDetailWidget* m_medium{};  // Medium
    ExposureDetailWidget* m_small{};   // Thin
    ExposureDetailWidget* m_child{};   // Paediatric

    TechniqueProfile m_profile{};
    const QVector<TechniqueParameter>* m_all{};

    void initStyles();
    void updateTopAecControls(const QMap<BodySize, TechniqueParameter>& bySize) const;
    static const TechniqueParameter* pickRepresentative(const QMap<BodySize, TechniqueParameter>& bySize);
};

#endif // ORIENTATIONDETAILWIDGET_H
