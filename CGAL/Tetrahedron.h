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
// GNU General Public License (htvp://www.gnu.org/licenses/gpl.txt)
// for more details.
//
// --------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <vector>

class Tetrahedron {
public:
  inline Tetrahedron(){
    for(int i = 0 ; i < 4 ; i ++)
        neighbors[i] = -1;
  }
  inline Tetrahedron (unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) { init (v0, v1, v2, v3); }
  inline Tetrahedron (unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int _label) { init (v0, v1, v2, v3, _label); }
  inline Tetrahedron (unsigned int * vp, unsigned int _label) { init (vp[0], vp[1], vp[2], vp[3], _label); }
  inline Tetrahedron (const Tetrahedron & it) { init (it.v[0], it.v[1], it.v[2], it.v[3], it.label); for(int i = 0 ; i < 4 ; i ++)neighbors[i] = it.neighbors[i]; }
  inline virtual ~Tetrahedron () {}

  inline Tetrahedron & operator= (const Tetrahedron & it) { init (it.v[0], it.v[1], it.v[2], it.v[3], it.label); for(int i = 0 ; i < 4 ; i ++)neighbors[i] = it.neighbors[i]; return (*this); }
  inline bool operator== (const Tetrahedron & _t) const { return (v[0] == _t.v[0] && v[1] == _t.v[1] && v[2] == _t.v[2] && v[3] == _t.v[3]); }

  inline unsigned int getVertex (unsigned int i) const { return v[i]; }
  inline unsigned int getLabel () const { return label; }

  inline int isNeighbor( int ni ){ for(int i = 0 ; i < 4 ; i ++) if(neighbors[i]==ni) return i; return -1; }

  inline int getNeighbor( unsigned int i ){ return neighbors[i]; }

  inline bool contains (unsigned int vertex) const { return (v[0] == vertex || v[1] == vertex || v[2] == vertex || v[3] == vertex); }
  inline int indexOf (unsigned int vertex) const { for(int j=0; j<4; j++) if(v[j]==vertex){ return j;} return -1; }

  inline void setVertex (unsigned int i, unsigned int vertex) { v[i] = vertex; }
  inline void setLabel(unsigned int _label) {label = _label;}

  inline void setNeighbor( unsigned int i, int n ){ neighbors[i] = n; }

  inline bool replace(unsigned int vertex, unsigned int by){ for(int j=0; j<4; j++) if(v[j]==vertex){ v[j]=by; return true;} return false;}
  inline void decrement(unsigned int i){ for(int j=0; j<4; j++) if(v[j]>i) v[j]-=1; } 

  unsigned int operator [] (unsigned int c) const
  {
      return v[c];
  }
protected:

  inline void init (unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) {
      v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; label = 0;
      for(int i = 0 ; i < 4 ; i ++)
          neighbors[i] = -1;
  }
  inline void init (unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int _label) {
      v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; label = _label;
      for(int i = 0 ; i < 4 ; i ++)
          neighbors[i] = -1;
  }

private:
  unsigned int v[4];
  unsigned int label;
  int neighbors[4];

};

extern std::ostream & operator<< (std::ostream & output, const Tetrahedron & t);

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:

