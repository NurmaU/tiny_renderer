//
// Created by nurma on 1/6/23.
//

#ifndef TINYRENDERER_DEPTH_SHADER_H
#define TINYRENDERER_DEPTH_SHADER_H

#include "model.h"
#include "our_gl.h"

extern Model* model;

class DepthShader : public IShader {
   public:
    Matrix TRI = {3, 3};

    virtual Vec3i vertex(int iface, int nthvert) {
        Vec3f v = model->vert(iface, nthvert);
        Vec3f pr = Viewport * Projection * ModelView * Matrix(v, 4);
        TRI.set_col(nthvert, Matrix(pr, 3));
        return pr;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        Vec3f p = TRI * Matrix(bar, 3);

        color = TGAColor(255, 255, 255) * (p.z / 255);
        return false;
    }
};

#endif  // TINYRENDERER_DEPTH_SHADER_H
