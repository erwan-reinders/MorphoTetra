#ifndef PLAYERDOCWIDGET_H
#define PLAYERDOCWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QShortcut>
#include "MultiMeshViewer.h"
#include <QSlider>
#include <QDockWidget>
#include <QHBoxLayout>


class PlayerDocWidget : public QDockWidget{
    Q_OBJECT
private :

    QIcon m_icon_start, m_icon_pause;
    QPushButton*        m_playPauseButton;
    QShortcut*          m_spaceShortcut;
    MultiMeshViewer*    m_viewer;

    QSlider*            m_slider;

    int m_min;
    int m_max;
    int m_step;
    int m_currentValue;

    bool m_playState;


protected :
    void initPlayerSlider(QHBoxLayout* layout);
    void initPlayerButtons(QHBoxLayout* layout, QWidget* parentW);
    void updateSliderValues();


public:
    PlayerDocWidget(MultiMeshViewer* _viewer, QWidget* parent);


public slots :
    void setPLayerValues(int min, int max,int step);
    void setCurrentValue(int value);
    void trySetDoubleValue(double value);
    void notifyValueChanged(int value);

    void pauseAction();
    void playAction();
    void togglePlay();

signals :
    void doubleValueChanged(double doubleValue);
    void paused();
    void played();
};

#endif // PLAYERDOCWIDGET_H
