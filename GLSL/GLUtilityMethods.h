#ifndef GLUTILITYMETHODS_H
#define GLUTILITYMETHODS_H

#include <QGLViewer/qglviewer.h>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <math.h>


namespace RGBColor {
    extern float color[30][3];
    extern float color4[30][4];
    extern int nColor;


    void calc_RGB( float val , float val_min , float val_max , float & r , float & g , float & b );
    void get_random_RGB_from_HSV( float & r , float & g , float & b );
    void get_random_RGB_from_HSV( float & r , float & g , float & b , float H);
}

namespace GLTools{
    void initLights ();
    void setSunsetLight ();
    void setSunriseLight ();
    void setSingleSpotLight ();
    void setMovingSpotLight ();
    void setDefaultMaterial ();
    void setInverseDefaultMaterial ();


    void SetAmbient(float r, float g , float b,float alpha = 1.f);
    void SetDiffuse(float r, float g , float b,float alpha = 1.f);
    void SetSpecular(float r, float g , float b,float alpha = 1.f);
    void SetEmissive(float r, float g , float b,float alpha = 1.f);
    void SetShininess(float sh);

    void MoveSpot(int tetaStep, int phiStep, int tr);


    enum MaterialStandard {
        MY_MATERIAL_01 ,
        TAMY_MATERIAL_01 ,
        TAMY_MATERIAL ,
        EMERALD ,
        JADE ,
        OBSIDIAN ,
        PEARL ,
        RUBY ,
        TURQUOISE ,
        BRASS ,
        BRONZE ,
        CHROME ,
        COPPER ,
        GOLD ,
        POLISHED_GOLD ,
        SILVER ,
        POLISHED_SILVER ,
        BLACK_PLASTIC ,
        CYAN_PLASTIC ,
        GREEN_PLASTIC ,
        RED_PLASTIC ,
        WHITE_PLASTIC ,
        YELLOW_PLASTIC ,
        BLACK_RUBBER ,
        CYAN_RUBBER ,
        GREEN_RUBBER ,
        RED_RUBBER ,
        WHITE_RUBBER ,
        YELLOW_RUBBER ,
        PEWETER
    };

    void MLoadStandard( MaterialStandard typ );
}

#endif
