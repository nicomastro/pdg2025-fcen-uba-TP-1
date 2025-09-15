//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:14:44 gtaubin>
//------------------------------------------------------------------------
//
// SaverStl.cpp
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

#include "SaverStl.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

#include "core/Faces.hpp"

const char* SaverStl::_ext = "stl";

//////////////////////////////////////////////////////////////////////
bool SaverStl::save(const char* filename, SceneGraph& wrl) const {
    bool success = false;
    if(filename!=(char*)0) {
         bool isShape = false;
         bool isIndexedFaceSet = false;
         bool isTriangleMesh = false;
         bool hasNormals = false;
         IndexedFaceSet* ixFaceSet = NULL;
         vector<pNode> children = wrl.getChildren();

         // 1) the SceneGraph should have a single child
         bool onlyChild = children.size() == 1;
         if(onlyChild){
            // 2) the child should be a Shape node
            pNode ch = children[0];
            isShape = ch->isShape();
            if (isShape){
                // 3) the geometry of the Shape node should be an IndexedFaceSet node
                Shape* shape_ch = (Shape*)ch;
                isIndexedFaceSet = shape_ch->hasGeometryIndexedFaceSet();
                if(isIndexedFaceSet){
                    // 4) the IndexedFaceSet should be a triangle mesh
                    // 5) the IndexedFaceSet should have normals per face
                   ixFaceSet = (IndexedFaceSet*) shape_ch->getGeometry();
                   isTriangleMesh = ixFaceSet->isTriangleMesh();
                   hasNormals = ixFaceSet->getNormalBinding() == IndexedFaceSet::PB_PER_FACE;
                 }
            }

        }
        bool isValidFile = isShape && isIndexedFaceSet && isTriangleMesh && hasNormals;
        if (isValidFile) {
            FILE* fp = fopen(filename,"w");
            if(	fp!=(FILE*)0) {

                // if set, use ifs->getName() otherwise use filename,
                const string& name = ixFaceSet->getName();
                string sfilename = string(filename);
                std::size_t init_pos = sfilename.find_last_of("/") + 1;
                sfilename = sfilename.substr(init_pos, sfilename.find_last_of(".") - init_pos);
                sfilename = (name == "" ? sfilename : name);
                fprintf(fp,"solid %s\n",sfilename.c_str());

                // - construct an instance of the Faces class from the IndexedFaceSet
                Faces faces = Faces(ixFaceSet->getNumberOfCoord(), ixFaceSet->getCoordIndex());
                auto normals = ixFaceSet->getNormal();
                auto coords = ixFaceSet->getCoord();
                float face_normal[3];
                float vertex_coords[3];
                for(int iF = 0; iF < faces.getNumberOfFaces(); ++iF){
                    int iV = 0;
                    int vertex = -1;

                    // write normal
                    for(int i = 0; i < 3; ++i) face_normal[i] = normals[3*iF + i];
                    fprintf(fp,"facet normal %f %f %f\n", face_normal[0], face_normal[1], face_normal[2]);

                    // write face vertex
                    fprintf(fp, "  outer loop\n");
                    while((vertex = faces.getFaceVertex(iF, iV++)) > -1){
                        for(int i = 0; i < 3; ++i) vertex_coords[i] = coords[3*vertex + i];
                        fprintf(fp,"    vertex %f %f %f\n", vertex_coords[0], vertex_coords[1], vertex_coords[2]);
                    }
                    fprintf(fp, "  endloop\n");
                    fprintf(fp, "endfacet\n");
                }
                fprintf(fp, "endsolid %s", sfilename.c_str());

            // - remember to delete it when you are done with it (if necessary)
            //   before returning
            fclose(fp);
            success = true;
            }
        }
    }

    return success;
}
