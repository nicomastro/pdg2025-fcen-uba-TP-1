//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:12:21 gtaubin>
//------------------------------------------------------------------------
//
// LoaderStl.cpp
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

#include <stdio.h>
#include "TokenizerFile.hpp"
#include "LoaderStl.hpp"
#include "StrException.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

// reference
// https://en.wikipedia.org/wiki/STL_(file_format)

const char* LoaderStl::_ext = "stl";



bool parse_face(TokenizerFile& tkn, Vec3f& n, Vec3f* v){
    if(!(tkn == "facet" && tkn.expecting("normal")))
        throw new StrException("Expecting facet normal");
    if(!tkn.getVec3f(n))
        throw new StrException("Expecting Vec3f");
    if(!(tkn.expecting("outer") && tkn.expecting("loop")))
        throw new StrException("Expecting outer loop");
    for(unsigned char j = 0; j<3; ++j)
        if(!(tkn.expecting("vertex") && tkn.getVec3f(v[j])))
            throw new StrException("Expecting vertex Vec3f");
    if(!(tkn.expecting("endloop") && tkn.expecting("endfacet")))
        throw new StrException("Expecting endfacet");
    return true;
}



bool LoaderStl::load(const char* filename, SceneGraph& wrl) {
  bool success = false;

  // clear the scene graph
  wrl.clear();
  wrl.setUrl("");

  FILE* fp = (FILE*)0;
  try {
    // open the file
    if(filename==(char*)0) throw new StrException("filename==null");
    fp = fopen(filename,"r");
    if(fp==(FILE*)0) throw new StrException("fp==(FILE*)0");

    // use the io/Tokenizer class to parse the input ascii file
    TokenizerFile tkn(fp);

    // first token should be "solid"
    if(tkn.expecting("solid") && tkn.get()) {

        // second token should be the solid name
        string stlName = tkn;

        // create the scene graph structure :
        // 1) the SceneGraph should have a single Shape node a child
        // 2) the Shape node should have an Appearance node in its appea
        Shape* shape_child = new Shape();
        Appearance* app = new Appearance();
        shape_child->setAppearance(app);

        // 3) the Appearance node should have a Material node in its material field
        Material* material = new Material();
        app->setMaterial(material);

        // 4) the Shape node should have an IndexedFaceSet node in its geometry node
        IndexedFaceSet* ixFaceSet = new IndexedFaceSet();
        shape_child->setGeometry(ixFaceSet);
        wrl.addChild(shape_child);

        // from the IndexedFaceSet
        // 5) get references to the coordIndex, coord, and normal arrays
        vector<int>& coordIndex = ixFaceSet->getCoordIndex();
        vector<float>&normals = ixFaceSet->getNormal();
        vector<float>&coords = ixFaceSet->getCoord();

        // 6) set the normalPerVertex variable to false (i.e., normals per face)
        ixFaceSet->setNormalPerVertex(false);

        // ASSUMES .stl file ends with "endsolid" (!)
        Vec3f n;
        Vec3f* v = new Vec3f[3];
        int vertex = 0;
        while(tkn.get() && tkn != "endsolid"){

            // - write a private method to parse each face within the loop
            if(parse_face(tkn, n, v)){
                for(unsigned char j = 0; j<3; ++j)
                    normals.push_back(n[j]);
                for(unsigned char i = 0; i<3; ++i)
                    for(unsigned char j = 0; j<3; ++j)
                        coords.push_back(v[i][j]);
                for(unsigned char j = 0; j<3; ++j)
                    coordIndex.push_back(vertex++);
                coordIndex.push_back(-1);
            }
        }

        // close the file
        success = true;
        delete[] v;
        fclose(fp);
    }
  } catch(StrException* e) {

    if(fp!=(FILE*)0) fclose(fp);
    printf("ERROR | %s\n",e->what());
    delete e;

  }

  return success;
}

