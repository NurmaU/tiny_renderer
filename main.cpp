#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>

#include "geometry.h"
#include "model.h"
#include "our_gl.h"
#include "shaders/depth_shader.h"
#include "shaders/gourad_with_depth.h"

const int width = 800;
const int height = 800;

Matrix ModelView;
Matrix Viewport;
Matrix Projection;
Model* model = NULL;

Vec3f light_dir = Vec3f(1, 1, 1).normalize();
Vec3f eye = Vec3f(1, 1, 3);
Vec3f center = Vec3f(0, 0, 0);
Vec3f up = Vec3f(0, 1, 0);

int main() {
    // Model
    model = new Model(
        "../data/african_head.obj", "../data/african_head_diffuse.tga",
        "../data/african_head_nm.tga", "../data/african_head_spec.tga");

    // Z-buffers
    auto* zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
        zbuffer[i] = std::numeric_limits<float>::min();

    auto* shadow_buffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
        shadow_buffer[i] = std::numeric_limits<float>::min();

    // Depth image
    {
        TGAImage depth(width, height, TGAImage::RGB);
        DepthShader shader;

        lookat(light_dir, center, up);
        projection(0);
        viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
        for (int i = 0; i < model->nfaces(); i++) {
            Vec3f pts[3];
            for (int j = 0; j < 3; j++) {
                pts[j] = shader.vertex(i, j);
            }
            triangle(pts, shadow_buffer, depth, shader);
        }

        depth.flip_vertically();
        depth.write_tga_file("depth.tga");
    }
    Matrix M = Viewport * Projection * ModelView;

    GouraudWithDepthShader shader;

    float coeff = -1.f / (eye - center).norm();

    lookat(eye, center, up);
    projection(coeff);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

    shader.M = Projection * ModelView;
    shader.MIT = (Projection * ModelView).inverse().transpose();
    shader.MS = M * (Viewport * Projection * ModelView).inverse();
    shader.shadow_buffer = shadow_buffer;

    // Image
    TGAImage image(width, height, TGAImage::RGB);

    for (int i = 0; i < model->nfaces(); i++) {
        Vec3f pts[3];
        for (int j = 0; j < 3; j++) {
            pts[j] = shader.vertex(i, j);
        }
        triangle(pts, zbuffer, image, shader);
    }

    image.flip_vertically();
    image.write_tga_file("image.tga");

    return 0;
}