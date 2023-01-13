#ifndef MATH_UTILITIESFNC_H
#define MATH_UTILITIESFNC_H

#include <QGLViewer/vec.h>
#include "CGAL/cgal_headers.h"

inline void Catmull_Rom(std::vector<qglviewer::Vec> & outPoints, qglviewer::Vec p0,  qglviewer::Vec p1, qglviewer::Vec p2, qglviewer::Vec p3, float subdivisionNumber) {
    //std::cout << "Catmull_Rom ON : " << std::endl;
    //std::cout << p0[0] << "," << p0[1] << ","<< p0[2]<< std::endl;
    //std::cout << p1[0] << "," << p1[1] << ","<< p1[2]<< std::endl;
    //std::cout << p2[0] << "," << p2[1] << ","<< p2[2]<< std::endl;
    //std::cout << p3[0] << "," << p3[1] << ","<< p3[2]<< std::endl;


    outPoints.reserve(subdivisionNumber);
    float timeIncrement = 1.0f / subdivisionNumber;
    if (timeIncrement <= 1.e-6) {
        return;
    }
    float t = 0.0f;
    while (t < 1.0f) {
        qglviewer::Vec point = p1 + t * 0.5*(p2-p0) + t*t * 0.5*(2*p0-5*p1+4*p2-p3) + t*t*t * 0.5*(-p0+3*p1-3*p2+p3);
        outPoints.push_back(point);
        t += timeIncrement;
    }
}

inline qglviewer::Vec pointToVec(Point_3 point) {
    return qglviewer::Vec(point.x(), point.y(), point.z());
}

#endif // MATH_UTILITIESFNC_H
