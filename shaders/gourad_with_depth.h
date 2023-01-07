//
// Created by nurma on 1/7/23.
//

#ifndef TINYRENDERER_GOURAD_WITH_DEPTH_H
#define TINYRENDERER_GOURAD_WITH_DEPTH_H

#include "../model.h"
#include "../our_gl.h"

extern Model* model;
extern Vec3f light_dir;
extern const int width;

class GouraudWithDepthShader : public IShader {
   public:
    Vec3f intensity;
    Matrix UV = {2, 3};  // 2 row, 3 cols
    Matrix M;
    Matrix MIT;
    Matrix TRI = {3, 3};
    Matrix MS;
    float* shadow_buffer;

   public:
    virtual Vec3i vertex(int iface, int nthvert) {
        auto v = model->vert(iface, nthvert);
        auto uv = model->get_uvs(iface, nthvert);
        auto normal = model->get_normal(iface, nthvert);

        intensity[nthvert] = normal * light_dir;
        intensity[nthvert] = std::max(0.f, intensity[nthvert]);

        UV.set_col(nthvert, Matrix({uv[0], uv[1], 1}, 3));

        Vec3f pr = Viewport * Projection * ModelView * Matrix(v, 4);
        TRI.set_col(nthvert, Matrix(pr, 3));

        return pr;
    }
    virtual bool fragment(Vec3f bar, TGAColor& color) {
        Vec3f p = TRI * Matrix(bar, 3);
        Vec3f sb_p = MS * Matrix(p, 4);
        int idx = int(sb_p[0]) + int(sb_p[1]) * width;

        float shadow = .3 + .7 * (shadow_buffer[idx] < (sb_p[2] + 1));
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
            color[i] = std::min<float>(
                255, 1.f + color[i] * shadow * (1.2 * diff + 0.6 * spec));
        }
        return false;
    }
};

#endif  // TINYRENDERER_GOURAD_WITH_DEPTH_H
