/*Based on this orignal code : https://github.com/Isnao/TER_GenMaillage/blob/master/viewer/src/Window.h*/

#ifndef WINDOW_H
#define WINDOW_H
#include "DisplayDockWidget.h"

#include <QMainWindow>

class Window : public QMainWindow
  {
    Q_OBJECT

  public:
    Window();

  private:
        DisplayDockWidget *m_displayDockWidget;
        Viewer * m_viewer1;
        Viewer * m_viewer2;
};

#endif // WINDOW_H
