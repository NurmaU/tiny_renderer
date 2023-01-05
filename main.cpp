#include <chrono>
#include <iostream>
#include <memory>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const float c_value = 5;
const int depth = 255;

void print(std::string text, float value) {
    std::cerr << text << " " << value << std::endl;
}
void line(Vec2i first, Vec2i second, TGAImage& image, const TGAColor& color) {
    int x0 = first.x, y0 = first.y;
    int x1 = second.x, y1 = second.y;

    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);

        error2 += derror2;
        if (error2 > 0.5) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

Vec3f baryCentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i = 2; i--;) {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }
    Vec3f u = s[0] ^ s[1];
    if (std::abs(u[2]) < 1e-2) return Vec3f(-1, 1, 1);
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangle(Vec3f* pts, float* zbuffer, TGAImage& image, Vec2f uvs[3],
              std::shared_ptr<Model> model, float* intensity) {
    Vec2f bboxmin(image.get_width() - 1, image.get_height() - 1);
    Vec2f bboxmax(0, 0);
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

    for (int i = 0; i < 3; i++) {
        bboxmin.x = std::max(0.f, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0.f, std::min(bboxmin.y, pts[i].y));

        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }

    Vec3f P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            Vec3f bc_screen = baryCentric(pts[0], pts[1], pts[2], P);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;

            P.z = 0;
            float u = 0, v = 0, total_intensity = 0;
            for (int i = 0; i < 3; i++) {
                P.z += pts[i][2] * bc_screen[i];
                u += uvs[i][0] * bc_screen[i];
                v += uvs[i][1] * bc_screen[i];
                total_intensity += intensity[i] * bc_screen[i];
            }
            // TGAColor color = model->get_color(u, v);
            TGAColor color(255, 255, 255);
            color = color * total_intensity;
            if (zbuffer[int(P.x + P.y * image.get_width())] < P.z) {
                zbuffer[int(P.x + P.y * image.get_width())] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye - center).normalize();
    Vec3f x = (up ^ z).normalize();
    Vec3f y = (z ^ x).normalize();

    Matrix retval = Matrix::identity(4);

    for (int i = 0; i < 3; i++) {
        retval[0][i] = x[i];
        retval[1][i] = y[i];
        retval[2][i] = z[i];
        retval[i][3] = -center[i];
    }
    return retval;
}

Matrix getViewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x + w / 2.f;
    m[1][3] = y + h / 2.f;
    m[2][3] = depth / 2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    m[2][2] = depth / 2.f;
    return m;
}

int main() {
    int width = 1000;
    int height = 1000;
    TGAImage image(width, height, TGAImage::RGB);

    auto start = std::chrono::high_resolution_clock::now();

    auto model = std::make_shared<Model>("../data/african_head.obj",
                                         "../data/african_head_diffuse.tga");

    float* zbuffer = new float[width * height];
    for (int i = width * height; i--;
         zbuffer[i] = std::numeric_limits<float>::min())
        ;
    std::cerr << "ZBUFFER: " << zbuffer[0]
              << std::endl;  // -3.40282e+38 -- 1.17549e-38

    Matrix ModelView = lookat(Vec3f(1, 1, 3), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    Matrix Projection = Matrix::identity(4);
    Matrix ViewPort = getViewport(0, 0, width, height);
    //    Matrix Projection = Matrix::identity(4);
    //    Projection[3][2] = -1.0 / c_value;

    std::cerr << ModelView << std::endl;
    std::cerr << Projection << std::endl;
    std::cerr << ViewPort << std::endl;

    Matrix Z = ViewPort * Projection * ModelView;
    std::cerr << Z << std::endl;

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