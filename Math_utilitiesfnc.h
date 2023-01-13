#ifndef MATH_UTILITIESFNC_H
#define MATH_UTILITIESFNC_H

#include <QGLViewer/vec.h>
#include "CGAL/cgal_headers.h"

inline void Catmull_Rom(std::vector<qglviewer::Vec> & outPoints, qglviewer::Vec p0,  qglviewer::Vec p1, qglviewer::Vec p2, qglviewer::Vec p3, float subdivisionNumber) {
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





// =====================================================





inline float projectPointOnSegment(qglviewer::Vec point, qglviewer::Vec segmentP0, qglviewer::Vec segmentP1, qglviewer::Vec& projectedPoint) {
    qglviewer::Vec segment = (segmentP1 - segmentP0);
    float segmentLength = segment.norm();
    float d = (point - segmentP0) * (segment) / segmentLength;
    if (d < 0) {
        projectedPoint = segmentP0;
        return (segmentP0 - point).norm();
    }
    else if (d > segmentLength) {
        projectedPoint = segmentP1;
        return (segmentP1 - point).norm();
    }
    else {
        projectedPoint = segmentP0 + segment * d / segmentLength;
        return (projectedPoint - point).norm();
    }
}


inline float projectPointOnPolyline(qglviewer::Vec point, std::vector<C3t3::Edge> polyline, qglviewer::Vec & projectedPoint) {
    float minError = 99999.0f;
    qglviewer::Vec minProjectedPoint;

    qglviewer::Vec currentProjectedPoint;
    for (int e = 0; e < polyline.size(); e++) {
        C3t3::Edge edge = polyline[e];

        float error = projectPointOnSegment(point, pointToVec(edge.first->vertex(edge.second)->point()), pointToVec(edge.first->vertex(edge.third)->point()), currentProjectedPoint);

        if (error < minError) {
            minError = error;
            minProjectedPoint = currentProjectedPoint;
        }
    }

    projectedPoint = minProjectedPoint;

    return minError;
}


inline float projectPolylineOnPolyline(std::vector<C3t3::Edge> polylineA, std::vector<C3t3::Edge> polylineB, std::vector<qglviewer::Vec> & projectedPolyline) {
    float totalError = 0.0f;
    projectedPolyline.resize(polylineA.size());
    qglviewer::Vec projectedPoint;

    if (polylineA.size() == 1) {
        totalError += projectPointOnPolyline(pointToVec(polylineA[0].first->vertex(polylineA[0].second)->point()), polylineB, projectedPoint);
        projectedPolyline.push_back(projectedPoint);
        totalError += projectPointOnPolyline(pointToVec(polylineA[0].first->vertex(polylineA[0].third)->point()), polylineB, projectedPoint);
        projectedPolyline.push_back(projectedPoint);
        return totalError;
    }

    Vertex_handle currentPoint;
    Vertex_handle previousNotDone;
    if (polylineA[0].first->vertex(polylineA[0].second) == polylineA[1].first->vertex(polylineA[1].second) || polylineA[0].first->vertex(polylineA[0].second) == polylineA[1].first->vertex(polylineA[1].third)) {
        currentPoint = polylineA[0].first->vertex(polylineA[0].third);
        previousNotDone = polylineA[0].first->vertex(polylineA[0].second);
    }
    else {
        currentPoint = polylineA[0].first->vertex(polylineA[0].second);
        previousNotDone = polylineA[0].first->vertex(polylineA[0].third);
    }

    float curentError = projectPointOnPolyline(pointToVec(currentPoint->point()), polylineB, projectedPoint);
    projectedPolyline.push_back(projectedPoint);
    totalError += curentError;

    for (int e = 1; e < polylineA.size(); e++) {
        C3t3::Edge edge = polylineA[e];
        if (edge.first->vertex(edge.second) == previousNotDone) {
            currentPoint = edge.first->vertex(edge.second);
            previousNotDone = edge.first->vertex(edge.third);
        }
        else {
            currentPoint = edge.first->vertex(edge.third);
            previousNotDone = edge.first->vertex(edge.second);
        }


        curentError = projectPointOnPolyline(pointToVec(currentPoint->point()), polylineB, projectedPoint);
        projectedPolyline.push_back(projectedPoint);
        totalError += curentError;
    }

    curentError = projectPointOnPolyline(pointToVec(previousNotDone->point()), polylineB, projectedPoint);
    projectedPolyline.push_back(projectedPoint);
    totalError += curentError;

    return totalError;
}


// Trouver la polyline similaire géométriquement parmis une liste de polyline
inline int findPolyline(std::vector<C3t3::Edge> polyline, std::vector<std::vector<C3t3::Edge>> polylinesToFind) {
    float minError = 99999.0f;
    int indexFound = 0;

    std::vector<qglviewer::Vec> newPolyline;
    for (int p = 0; p < polylinesToFind.size(); p++) {
        float error = projectPolylineOnPolyline(polyline, polylinesToFind[p], newPolyline);
        if (error < minError) {
            minError = error;
            indexFound = p;
        }
    }

    return indexFound;
}



#endif // MATH_UTILITIESFNC_H
