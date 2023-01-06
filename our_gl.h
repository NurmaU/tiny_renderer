#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include "geometry.h"
#include "tgaimage.h"

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

Matrix viewport(int x, int y, int w, int h);
Matrix projection(float coeff = 0.f);
Matrix lookat(Vec3f eye, Vec3f center, Vec3f up);

struct IShader {
    virtual ~IShader();
    virtual Vec3i vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

// void triangle(Vec3f *pts, IShader &shader, TGAImage &image, TGAImage
// &zbuffer);
void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, Vec2f uvs[3],
              std::shared_ptr<Model> model, const float *intensity);

#endif  // __OUR_GL_H__
