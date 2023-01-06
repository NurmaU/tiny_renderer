#include <chrono>
#include <iostream>
#include <memory>

#include "geometry.h"
#include "model.h"
#include "our_gl.h"

const int width = 1000;
const int height = 1000;

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

   public:
    virtual Vec3i vertex(int iface, int nthvert) {
        intensity[nthvert] = model->get_normal(iface, nthvert) * light_dir;
        intensity[nthvert] = std::max(0.f, intensity[nthvert]);
        Vec3f v = model->vert(iface, nthvert);
        Vec2f uv = model->get_uvs(iface, nthvert);
        UV.set_col(nthvert, uv);
        return Vec3i(Vec3f(Viewport * Projection * ModelView * Matrix(v)));
    }
    virtual bool fragment(Vec3f bar, TGAColor& color) const {
        float total_intensity = bar * intensity;
        // color = TGAColor(255, 255, 255) * total_intensity;
        Vec2f uv = UV * bar;
        color = model->get_color(uv[0], uv[1]) * total_intensity;
        return false;
    }
};

int main() {
    // Image

    TGAImage image(width, height, TGAImage::RGB);

    // Model
    model = new Model("../data/african_head.obj",
                      "../data/african_head_diffuse.tga");
    // Matrices
    lookat(eye, center, up);
    projection();
    viewport(0, 0, width, height);

    // Z-buffer
    auto* zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
        zbuffer[i] = std::numeric_limits<float>::min();

    light_dir.normalize();

    // Shader
    GouraudShader shader;

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