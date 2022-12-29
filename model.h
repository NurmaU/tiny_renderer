#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
   private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int>> faces_;
    std::vector<std::vector<int>> text_indexes_;
    std::vector<std::vector<float>> text_coord_;
    TGAImage texture;

   public:
    Model(const char* filename, const char* texture_filename);
    ~Model();
    int nverts();
    int nfaces();
    int ntextcoords();
    int ntextinds();
    Vec3f vert(int i);
    std::vector<int> face(int idx);
};

#endif  // __MODEL_H__