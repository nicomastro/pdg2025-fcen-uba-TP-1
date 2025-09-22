//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:09:56 gtaubin>
//------------------------------------------------------------------------
//
// Faces.cpp
//
// Written by: <Your Name>
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <math.h>
#include "Faces.hpp"
  
Faces::Faces(const int nV, const vector<int>& coordIndex) {
  // TODO agregar chequeos
  this->nV = nV;
  this->coordIndex = coordIndex;
  this->firstCornerFace.push_back(0);
  uint faceIndex = 0;
  for (uint i = 0; i < coordIndex.size(); ++i){
      if(coordIndex[i] == -1){
        this->firstCornerFace.push_back(i+1);
        this->coordIndex[i] *= ++faceIndex;
      }
  }
}

int Faces::getNumberOfVertices() const {
    return  this->nV;
}

int Faces::getNumberOfFaces() const {
    return this->firstCornerFace.size() - 1;
}

int Faces::getNumberOfCorners() const {
    return this->coordIndex.size();
}


int Faces::getFaceSize(const int iF) const {

    int size = -1;
    if(iF < this->getNumberOfFaces())
        while(this->coordIndex[this->firstCornerFace[iF] + (++size)] > -1);
    return size;
}

int Faces::getFaceFirstCorner(const int iF) const {
    int firstCorner = -1;
    if(iF < this->getNumberOfFaces())
        firstCorner = this->firstCornerFace[iF];
    return firstCorner;
}

int Faces::getFaceVertex(const int iF, const int j) const {
    int vertex = -1;
    if(iF < this->getNumberOfFaces() && j < this->getFaceSize(iF))
        vertex = this->coordIndex[this->getFaceFirstCorner(iF) + j];
    return vertex;
    }

int Faces::getCornerFace(const int iC) const {
  if (iC >= this->getNumberOfCorners()) // refactor
    return -1;
  int i = -1;
  int face = 0;
  while((face = this->coordIndex[iC + (++i)]) > 0);
  face = i == 0 ? -1 : -1*face - 1;
  return face;
}

int Faces::getNextCorner(const int iC) const {
    if (iC >= this->getNumberOfCorners()) // refactor
        return -1;
    int nextCoord = -1;
    if(this->coordIndex[iC] > 0)
        if((nextCoord = this->coordIndex[iC+1]) < 0)
            nextCoord = this->getFaceFirstCorner(-1*nextCoord - 1);
    return nextCoord;
}

