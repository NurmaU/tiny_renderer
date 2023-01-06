#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include "geometry.h"
#include "tgaimage.h"

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

void viewport(int x, int y, int w, int h);
void projection(float coeff = 0.f);
void lookat(Vec3f eye, Vec3f center, Vec3f up);

class IShader {
    virtual Vec3i vertex(int iface, int nthvert) = 0;
};

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image,
              const float *intensity);

#endif  // __OUR_GL_H__
