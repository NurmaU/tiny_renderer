#include <chrono>
#include <iostream>
#include <memory>

#include "geometry.h"
#include "model.h"
#include "our_gl.h"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const float c_value = 5;
const int depth = 255;

int main() {
    int width = 1000;
    int height = 1000;
    TGAImage image(width, height, TGAImage::RGB);

    auto start = std::chrono::high_resolution_clock::now();

    auto model = std::make_shared<Model>("../data/african_head.obj",
                                         "../data/african_head_diffuse.tga");

    auto* zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
        zbuffer[i] = std::numeric_limits<float>::min();

    Matrix ModelView = lookat(Vec3f(1, 1, 3), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    Matrix Projection = Matrix::identity(4);
    Matrix ViewPort = viewport(0, 0, width, height);
    //    Matrix Projection = Matrix::identity(4);
    //    Projection[3][2] = -1.0 / c_value;

    //    std::cerr << ModelView << std::endl;
    //    std::cerr << Projection << std::endl;
    //    std::cerr << ViewPort << std::endl;

    Matrix Z = ViewPort * Projection * ModelView;

    Vec3f light_dir = Vec3f(1, -1, 1).normalize();
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        float intensivity[3];
        Vec3f pts[3];
        Vec2f uvs[3];

        model->get_uvs(i, uvs);
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            pts[j] = Vec3i(Vec3f(Z * Matrix(v)));
            intensivity[j] = model->get_normal(i, j) * light_dir;
        }

        triangle(pts, zbuffer, image, uvs, model, intensivity);
    }
    image.flip_vertically();
    image.write_tga_file("output.tga");

    auto delta = std::chrono::high_resolution_clock::now() - start;

    std::cout
        << "Time spent: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()
        << " ms" << std::endl;

    return 0;
}