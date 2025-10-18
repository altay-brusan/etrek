// OrientationDetailWidget.h
#ifndef ORIENTATIONDETAILWIDGET_H
#define ORIENTATIONDETAILWIDGET_H

#pragma once
#include <QWidget>
#include <QMap>
#include "TechniqueParameter.h"

namespace Ui { class OrientationDetailWidget; }
class ExposureDetailWidget;

class OrientationDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OrientationDetailWidget(Etrek::ScanProtocol::TechniqueProfile profile,
        const QVector<Etrek::ScanProtocol::Data::Entity::TechniqueParameter>* data,
        QWidget* parent = nullptr);
    ~OrientationDetailWidget();

    // NEW: snapshot all 4 size tabs for a given body part
    QVector<Etrek::ScanProtocol::Data::Entity::TechniqueParameter> collectForBodyPart(const Etrek::ScanProtocol::Data::Entity::BodyPart& bp) const;

public slots:
    void onBodyPartChanged(int bodyPartId);

private:
    Ui::OrientationDetailWidget* ui{};
    ExposureDetailWidget* m_large{};   // Fat
    ExposureDetailWidget* m_medium{};  // Medium
    ExposureDetailWidget* m_small{};   // Thin
    ExposureDetailWidget* m_child{};   // Paediatric

    Etrek::ScanProtocol::TechniqueProfile m_profile{};
    const QVector<Etrek::ScanProtocol::Data::Entity::TechniqueParameter>* m_all{};

    void initStyles();
    void updateTopAecControls(const QMap<Etrek::ScanProtocol::BodySize, Etrek::ScanProtocol::Data::Entity::TechniqueParameter>& bySize) const;
    static const Etrek::ScanProtocol::Data::Entity::TechniqueParameter* pickRepresentative(const QMap<Etrek::ScanProtocol::BodySize, Etrek::ScanProtocol::Data::Entity::TechniqueParameter>& bySize);
};

#endif // ORIENTATIONDETAILWIDGET_H
