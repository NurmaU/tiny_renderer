#include "our_gl.h"

#include <memory>

#include "geometry.h"
#include "tgaimage.h"

const int depth = 255;

Vec3f baryCentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i = 2; i--;) {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }
    Vec3f u = s[0] ^ s[1];
    if (std::abs(u[2]) < 1e-2) return {-1, 1, 1};
    return {1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z};
}

void triangle(Vec3f* pts, float* zbuffer, TGAImage& image,
              const IShader& shader) {
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
                //                u += uvs[i][0] * bc_screen[i];
                //                v += uvs[i][1] * bc_screen[i];
                //                total_intensity += intensity[i] *
                //                bc_screen[i];
            }
            // TGAColor color = model->get_color(u, v);
            TGAColor color(255, 255, 255);
            shader.fragment(bc_screen, color);
            if (zbuffer[int(P.x + P.y * image.get_width())] < P.z) {
                zbuffer[int(P.x + P.y * image.get_width())] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye - center).normalize();
    Vec3f x = (up ^ z).normalize();
    Vec3f y = (z ^ x).normalize();

    ModelView = Matrix::identity(4);

    for (int i = 0; i < 3; i++) {
        ModelView[0][i] = x[i];
        ModelView[1][i] = y[i];
        ModelView[2][i] = z[i];
        ModelView[i][3] = -center[i];
    }
}

void viewport(int x, int y, int w, int h) {
    Viewport = Matrix::identity(4);
    Viewport[0][3] = x + w / 2.f;
    Viewport[1][3] = y + h / 2.f;
    Viewport[2][3] = depth / 2.f;

    Viewport[0][0] = w / 2.f;
    Viewport[1][1] = h / 2.f;
    Viewport[2][2] = depth / 2.f;
}

void projection(float coeff) {
    Projection = Matrix::identity(4);
    Projection[3][2] = coeff;
}
