#include <chrono>
#include <iostream>
#include <memory>

#include "model.h"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

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
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2]) < 1e-2) return Vec3f(-1, 1, 1);
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangle(Vec3f* pts, float* zbuffer, TGAImage& image, Vec2f uvs[3],
              std::shared_ptr<Model> model) {
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
            float u = 0, v = 0;
            for (int i = 0; i < 3; i++) {
                P.z += pts[i][2] * bc_screen[i];
                u += uvs[i][0] * bc_screen[i];
                v += uvs[i][1] * bc_screen[i];
            }
            TGAColor color = model->get_color(u, v);
            if (zbuffer[int(P.x + P.y * image.get_width())] < P.z) {
                zbuffer[int(P.x + P.y * image.get_width())] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

Vec3f world2screen(Vec3f v, int width, int height) {
    return Vec3f(int((v.x + 1.) * width / 2. + .5),
                 int((v.y + 1.) * height / 2. + .5), v.z);
}

int main() {
    int width = 1000;
    int height = 1000;
    TGAImage image(width, height, TGAImage::RGB);

    auto start = std::chrono::high_resolution_clock::now();

    auto model = std::make_shared<Model>("../data/african_head.obj",
                                         "../data/african_head_diffuse.tga");

    Vec3f ligth_dir(0, 0, -1);
    float* zbuffer = new float[width * height];
    for (int i = width * height; i--;
         zbuffer[i] = -std::numeric_limits<float>::max())
        ;

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f pts[3];
        Vec3f world_coords[3];
        Vec2f uvs[3];

        model->get_uvs(i, uvs);

        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            world_coords[j] = v;
            pts[j] = world2screen(v, width, height);
        }
        Vec3f n = cross((world_coords[2] - world_coords[0]),
                        (world_coords[1] - world_coords[0]));
        n.normalize();
        float intensivity = n * ligth_dir;
        if (intensivity > 0) triangle(pts, zbuffer, image, uvs, model);
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

int demo() {
    Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    int width = 200;
    int height = 200;
    TGAImage image(width, height, TGAImage::RGB);

    // triangle(t0, image, red);
    // triangle(t1, image, white);
    // triangle(t2, image, green);

    image.flip_vertically();
    image.write_tga_file("triangles.tga");

    return 1;
}