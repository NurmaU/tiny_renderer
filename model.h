#ifndef __MODEL_H__
#define __MODEL_H__

#include <utility>
#include <vector>

#include "geometry.h"
#include "tgaimage.h"

class Model {
   private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<Vec3i>> faces_;
    std::vector<std::pair<float, float>> text_coord_;
    std::vector<Vec3f> vns_;
    TGAImage texture;
    TGAImage norm_map;

   public:
    Model(const char* filename, const char* texture_filename,
          const char* norm_filename);
    ~Model();
    int nverts();
    int nfaces();
    int ntextcoords();
    int nnormals();
    Vec3f vert(int i, int j);
    std::vector<int> face(int idx);
    Vec2f get_uvs(int iface, int nthvert);
    TGAColor diffuse(Vec2f uv);
    Vec3f get_normal(int iface, int nvert);
    Vec3f get_normmap(Vec2f uv);
};

#endif  // __MODEL_H__