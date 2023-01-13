#include "PlayerDocWidget.h"
#include <QHBoxLayout>


PlayerDocWidget::PlayerDocWidget(MultiMeshViewer* _viewer, QWidget* parent)  : QDockWidget(parent) {
    QWidget* contents = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(contents);

    initPlayerButtons(layout, parent);
    initPlayerSlider(layout);

    m_viewer = _viewer;

    this->setWidget(contents);
}



void PlayerDocWidget::initPlayerButtons(QHBoxLayout* layout, QWidget* parentW) {
    m_icon_start      = QIcon("./icons/Play.png");
    m_icon_pause      = QIcon("./icons/Pause.png");

    m_playPauseButton = new QPushButton();
    QString buttonStyle = "QPushButton{border:none;background-color:rgba(50, 60, 250,100);}";
    m_playPauseButton->setStyleSheet(buttonStyle);
    m_playPauseButton->setIcon(m_icon_start);

    m_playPauseButton->setIconSize(QSize(50, 50));
    m_playPauseButton->setMinimumSize(50, 50);
    m_playPauseButton->setMaximumSize(50, 50);
    layout->addWidget(m_playPauseButton);

    m_spaceShortcut = new QShortcut(QKeySequence(' '), parentW);
}



void PlayerDocWidget::updateSliderValues() {
    m_slider->setMinimum(m_min);
    m_slider->setMaximum(m_max);
    m_slider->setSingleStep(m_step);
}



void PlayerDocWidget::initPlayerSlider(QHBoxLayout* layout) {
    m_min = 0;
    m_max = 5;
    m_step = 1;
    m_currentValue = 0;
    m_playState = false;

    m_slider = new QSlider(Qt::Horizontal);
    updateSliderValues();
    m_slider->setValue(m_currentValue);
    layout->addWidget(m_slider);

    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(notifyValueChanged(int)));

    connect(m_playPauseButton, SIGNAL(clicked()),this, SLOT(togglePlay()));
    connect(m_spaceShortcut, SIGNAL(activated()), this, SLOT(togglePlay()));
}






void PlayerDocWidget::setPLayerValues(int min, int max,int step) {
    m_min = min;
    m_max = max;
    m_step = step;
    updateSliderValues();
}


void PlayerDocWidget::setCurrentValue(int value) {
    m_currentValue = value;
    m_slider->setValue(m_currentValue);
}

void PlayerDocWidget::trySetDoubleValue(double value) {
    int newValue = (int)(value * (m_max - m_min) + ((float)(value - m_min)));
    //std::cout << "TRY DOUBLE : current = " << m_currentValue << " new = " << newValue << std::endl;
    if (newValue != m_currentValue) {
        setCurrentValue(newValue);
    }
}


void PlayerDocWidget::notifyValueChanged(int value) {
    if (value != m_currentValue) {
        m_currentValue = value;
        double doubleValue = ((float)(value - m_min)) / ((float)m_max - (float)m_min);
        emit doubleValueChanged(doubleValue);
    }
}


void PlayerDocWidget::playAction() {
    m_playState = true;
    m_playPauseButton->setIcon(m_icon_pause);

    emit played();
}

void PlayerDocWidget::pauseAction() {
    m_playState = false;
    m_playPauseButton->setIcon(m_icon_start);

    emit paused();
}

void PlayerDocWidget::togglePlay() {
    m_playState = !m_playState;

    if (m_playState) {
        playAction();
    }
    else {
        pauseAction();
    }
}
