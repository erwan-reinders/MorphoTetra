#ifndef REMESHINGDOCKWIDGET_H
#define REMESHINGDOCKWIDGET_H

#include <QDockWidget>

#include "MultiMeshViewer.h"

#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QCheckBox>


class RemeshingDockWidget : public QDockWidget
{
    Q_OBJECT
private:
    MultiMeshViewer*    m_viewer;
    QStatusBar * m_statusbar;
    QString m_filePath;

    QDoubleSpinBox *    m_cellRatioNumber;
    QDoubleSpinBox *    m_cellSizeNumber;
    QDoubleSpinBox *    m_facetAngleNumber;
    QDoubleSpinBox *    m_facetSizeNumber;
    QDoubleSpinBox *    m_facetApproximationNumber;
    QCheckBox *         m_perturbOption;
    QCheckBox *         m_exudeOption;

    QLabel * m_filePathLabel;

public:
    RemeshingDockWidget(MultiMeshViewer* _viewer, QString filePath, QStatusBar *statusbar, QWidget* parent);

public slots:
    void remesh();
};

#endif // REMESHINGDOCKWIDGET_H
