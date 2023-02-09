// --------------------------------------------------------------------------
// gMini,
// a minimal Glut/OpenGL app to extend                              
//
// Copyright(C) 2007-2009                
// Tamy Boubekeur
//                                                                            
// All rights reserved.                                                       
//                                                                            
// This program is free software; you can redistribute it and/or modify       
// it under the terms of the GNU General Public License as published by       
// the Free Software Foundation; either version 2 of the License, or          
// (at your option) any later version.                                        
//                                                                            
// This program is distributed in the hope that it will be useful,            
// but WITHOUT ANY WARRANTY; without even the implied warranty of             
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              
// GNU General Public License (http://www.gnu.org/licenses/gpl.txt)           
// for more details.                                                          
//                                                                          
// --------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <vector>
#include <map>

// -------------------------------------------------
// Intermediate Face structure for hashed adjacency
// -------------------------------------------------

struct Face {
public:
    inline Face ( unsigned int v0, unsigned int v1, unsigned int v2) {
        if (v1 < v0) std::swap(v0,v1);
        if (v2 < v1) std::swap(v1,v2);
        if (v1 < v0) std::swap(v0,v1);
        v[0] = v0; v[1] = v1; v[2] = v2;
    }
    //inline void std::swap(unsigned int &a, unsigned int &b){unsigned int tmp = a; a=b; b=tmp;}
    inline Face (const Face & f) { v[0] = f.v[0]; v[1] = f.v[1]; v[2] = f.v[2]; }
    inline virtual ~Face () {}
    inline Face & operator= (const Face & f) { v[0] = f.v[0]; v[1] = f.v[1]; v[2] = f.v[2]; return (*this); }
    inline bool operator== (const Face & f) { return (v[0] == f.v[0] && v[1] == f.v[1] && v[2] == f.v[2]); }
    inline bool operator< (const Face & f) const { return (v[0] < f.v[0] || (v[0] == f.v[0] && v[1] < f.v[1]) || (v[0] == f.v[0] && v[1] == f.v[1] && v[2] < f.v[2])); }
    inline bool contains (unsigned int i) const { return (v[0] == i || v[1] == i || v[2] == i); }
    inline unsigned int getVertex (unsigned int i) const { return v[i]; }
    unsigned int v[3];
};

struct compareFace {
    inline bool operator()(const Face f1, const Face f2) const {
        if( f1.v[0]<f2.v[0] ||
           (f1.v[0]==f2.v[0] && f1.v[1]<f2.v[1]) ||
           (f1.v[0]==f2.v[0] && f1.v[1]==f2.v[1] && f1.v[2]<f2.v[2]))
            return true;
        return false;
    }
};

typedef std::map<Face, unsigned int, compareFace> FaceMapIndex;


class Triangle {
public:
  inline Triangle(){}
  inline Triangle (unsigned int v0, unsigned int v1, unsigned int v2) { init (v0, v1, v2); }
  inline Triangle (unsigned int v0, unsigned int v1, unsigned int v2, unsigned int l) { init (v0, v1, v2); label = l; }
  inline Triangle (unsigned int * vp) { init (vp[0], vp[1], vp[2]); }
  inline Triangle (const Triangle & it) { init (it.v[0], it.v[1], it.v[2]); label = it.label; }
  inline virtual ~Triangle () {}
  inline Triangle & operator= (const Triangle & it) { init (it.v[0], it.v[1], it.v[2]); label = it.label; return (*this); }
  inline bool operator== (const Triangle & t) const { return (v[0] == t.v[0] && v[1] == t.v[1] && v[2] == t.v[2]); }

  inline unsigned int getVertex (unsigned int i) const { return v[i]; }
  inline unsigned int getLabel () const { return label; }
  inline void setVertex (unsigned int i, unsigned int vertex) { v[i] = vertex; }
  inline void setLabel(int _label) {label = _label;}
  inline bool contains (unsigned int vertex) const { return (v[0] == vertex || v[1] == vertex || v[2] == vertex); }

  unsigned int & operator [] (unsigned int c)
  {
      if( c == 0 )
          return v[0];
      if( c == 1 )
          return v[1];
      if( c == 2 )
          return v[2];
  }
protected:
  inline void init (unsigned int v0, unsigned int v1, unsigned int v2) {
	v[0] = v0; v[1] = v1; v[2] = v2;  
  }
  
private:
  unsigned int v[3];
  int label;
};

class Surface_Triangle : public Triangle {
public:
    inline Surface_Triangle (unsigned int v0, unsigned int v1, unsigned int v2, unsigned int l) : Triangle(v0, v1, v2, l) {
        label2 = 0;
    }
    inline Surface_Triangle (unsigned int v0, unsigned int v1, unsigned int v2, unsigned int l1, unsigned int l2) : Triangle(v0, v1, v2, l1) {
        label2 = l2;
    }

    inline unsigned int getLabel2 () const { return label2; }

private:
    int label2;
};

extern std::ostream & operator<< (std::ostream & output, const Triangle & t);


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
