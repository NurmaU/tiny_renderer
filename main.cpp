#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>

#include "geometry.h"
#include "model.h"
#include "our_gl.h"

const int width = 800;
const int height = 800;

Matrix ModelView;
Matrix Viewport;
Matrix Projection;
Model* model = NULL;

Vec3f light_dir = Vec3f(1, 1, 1);
Vec3f eye = Vec3f(1, 1, 3);
Vec3f center = Vec3f(0, 0, 0);
Vec3f up = Vec3f(0, 1, 0);

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
        UV.set_col(nthvert, uv);
        return Vec3f(Viewport * Projection * ModelView * Matrix(v));
    }
    virtual bool fragment(Vec3f bar, TGAColor& color) const {
        // float total_intensity = bar * intensity;
        // color = TGAColor(255, 255, 255) * total_intensity;
        Vec2f uv = UV * bar;

        Vec3f n = model->get_normmap(uv);
        n = Vec3f(MIT * Matrix(n));
        n.normalize();

        Vec3f l = Vec3f(M * Matrix(light_dir));
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

int main() {
    // Image

    TGAImage image(width, height, TGAImage::RGB);

    // Model
    model = new Model(
        "../data/african_head.obj", "../data/african_head_diffuse.tga",
        "../data/african_head_nm.tga", "../data/african_head_spec.tga");
    // Matrices
    lookat(eye, center, up);
    float coeff = -1.f / (eye - center).norm();
    projection(coeff);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

    // Z-buffer
    auto* zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
        zbuffer[i] = std::numeric_limits<float>::min();

    light_dir.normalize();

    // Shader
    GouraudShader shader;
    shader.M = Projection * ModelView;
    shader.MIT = shader.M.inverse().transpose();

    // Process
    for (int i = 0; i < model->nfaces(); i++) {
        Vec3f pts[3];
        for (int j = 0; j < 3; j++) {
            pts[j] = shader.vertex(i, j);
        }
        triangle(pts, zbuffer, image, shader);
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}