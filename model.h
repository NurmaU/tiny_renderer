#ifndef __MODEL_H__
#define __MODEL_H__

#include <utility>
#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
   private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int>> faces_;
    std::vector<std::vector<int>> text_indexes_;
    std::vector<std::pair<float, float>> text_coord_;
    std::vector<Vec3f> vns_;
    TGAImage texture;

   public:
    Model(const char* filename, const char* texture_filename);
    ~Model();
    int nverts();
    int nfaces();
    int ntextcoords();
    int ntextinds();
    int nnormals();
    Vec3f vert(int i);
    std::vector<int> face(int idx);
    void get_uvs(int idx, Vec2f* colors);
    TGAColor get_color(float u, float v);
};

#endif  // __MODEL_H__