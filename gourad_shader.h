//
// Created by nurma on 1/6/23.
//

#ifndef TINYRENDERER_GOURAD_SHADER_H
#define TINYRENDERER_GOURAD_SHADER_H

#include "model.h"
#include "our_gl.h"

extern Model* model;
extern Vec3f light_dir;

class GouraudShader : public IShader {
   public:
    Vec3f intensity;
    Matrix UV = {2, 3};  // 2 row, 3 cols
    Matrix M;
    Matrix MIT;

   public:
    virtual Vec3i vertex(int iface, int nthvert) {
        intensity[nthvert] = model->get_normal(iface, nthvert) * light_dir;
        intensity[nthvert] = std::max(0.f, intensity[nthvert]);
        Vec3f v = model->vert(iface, nthvert);
        Vec2f uv = model->get_uvs(iface, nthvert);
        UV.set_col(nthvert, Matrix({uv[0], uv[1], 1}, 3));
        return Vec3f(Viewport * Projection * ModelView * Matrix(v, 4));
    }
    virtual bool fragment(Vec3f bar, TGAColor& color) const {
        // float total_intensity = bar * intensity;
        // color = TGAColor(255, 255, 255) * total_intensity;
        Matrix uv_bar = UV * Matrix(bar, 3);  // size (2, 1)
        Vec2f uv(uv_bar[0][0], uv_bar[1][0]);

        Vec3f n = model->get_normmap(uv);
        n = Vec3f(MIT * Matrix(n, 4));
        n.normalize();

        Vec3f l = Vec3f(M * Matrix(light_dir, 4));
        l.normalize();

        Vec3f r = n * (n * l) * 2 - l;
        r.normalize();

        float spec = std::pow(std::max(r.z, 0.f), model->get_specular(uv));
        float diff = std::max(0.f, n * l);

        color = model->diffuse(uv);
        for (int i = 0; i < 3; i++) {
            color[i] =
                std::min<float>(255, 5.f + color[i] * (diff + 0.6 * spec));
        }
        return false;
    }
};

#endif  // TINYRENDERER_GOURAD_SHADER_H
