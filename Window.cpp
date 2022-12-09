/*Based on this orignal code : https://github.com/Isnao/TER_GenMaillage/blob/master/viewer/src/Window.cpp*/

#include "Window.h"
#include <QHBoxLayout>

Window::Window()
{
    if(DEBUG) std::cout << "WINDOW : start !" << std::endl;
    if(DEBUG) std::cout << "WINDOW : construct Viewers" << std::endl;

    m_viewer1 = new Viewer();
    m_viewer2 = new Viewer();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_viewer1);
    layout->addWidget(m_viewer2);

    QWidget *window = new QWidget();
    window->setLayout(layout);

    if(DEBUG) std::cout << "WINDOW : construct DocWidget" << std::endl;

    m_displayDockWidget = new DisplayDockWidget(m_viewer1, m_viewer2,this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_displayDockWidget);
    this->setCentralWidget(window);

}
