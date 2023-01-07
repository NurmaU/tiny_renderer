#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>

#include "depth_shader.h"
#include "geometry.h"
// #include "gourad_shader.h"
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
    DepthShader shader;
    //    GouraudShader shader;
    //    shader.M = Projection * ModelView;
    //    shader.MIT = shader.M.inverse().transpose();

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