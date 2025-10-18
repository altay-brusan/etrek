// GeneratorConfigurationWidget.h

#ifndef GENERATORCONFIGURATIONWIDGET_H
#define GENERATORCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QComboBox>
#include <QGroupBox>
#include <QSpinBox>
#include "Generator.h"
#include "XRayTube.h"
#include <dcmtk/dcmrt/drtimage.h>
#include <dcmtk/dcmrt/seq/drtds.h>

namespace Ui {
    class GeneratorConfigurationWidget;
}

class GeneratorConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneratorConfigurationWidget(
        const QVector<Etrek::Device::Data::Entity::Generator>& generators,
		const QVector<Etrek::Device::Data::Entity::XRayTube>& xRayTubes,
        QWidget* parent = nullptr
    );
    ~GeneratorConfigurationWidget();


private slots:
    //void onOutput1SelectionChanged(int index);
    //void onOutput2ActiveToggled(bool checked);
  
private:

    //void initUiWiring();
    //void populateBothCombos();                 // put "Tube 1" & "Tube 2" in both combos
    //void enforceOutput2AgainstOutput1();       // core rule
    //int  selectedTubeId(QComboBox* cb) const;  // returns 1/2 or -1 if none
    //void setComboToTube(QComboBox* cb, int tubeId);

    // constants for clarity
    static constexpr int Tube1Id = 1;
    static constexpr int Tube2Id = 2;

    Ui::GeneratorConfigurationWidget* ui;
	QVector<Etrek::Device::Data::Entity::XRayTube> allXRayTubes;  // Store all X-Ray tubes
    QVector<Etrek::Device::Data::Entity::Generator> allGenerators;  // Store all generators


};

#endif // GENERATORCONFIGURATIONWIDGET_H
