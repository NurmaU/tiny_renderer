#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

Model::Model(const char* filename, const char* texture_filename) {
    // read texture
    texture.read_tga_file(texture_filename);
    texture.flip_vertically();

    std::cerr << "TEXTURE IMAGE HEIGHT: " << texture.get_height() << std::endl;
    std::cerr << "TEXTURE IMAGE WIDTH: " << texture.get_width() << std::endl;

    // read points
    std::ifstream file;
    file.open(filename, std::ifstream::in);

    if (file.fail()) return;

    std::string line;

    while (!file.eof()) {
        std::getline(file, line);
        std::istringstream iss(line.c_str());
        char trash;

        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v[i];

            verts_.push_back(v);

        } else if (!line.compare(0, 2, "f ")) {
            std::vector<Vec3i> f;
            Vec3i tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                for (int i = 0; i < 3; i++)
                    tmp[i]--;  // in wavefront obj all indices start at 1, not
                               // zero
                f.push_back(tmp);
            }
            faces_.push_back(f);
        } else if (!line.compare(0, 2, "vt")) {
            float u, v;
            iss >> trash >> trash >> u >> v;
            std::pair<float, float> coord = std::make_pair(u, v);
            text_coord_.push_back(coord);
        } else if (!line.compare(0, 2, "vn")) {
            Vec3f vn;
            iss >> trash >> trash;
            for (int i = 0; i < 3; i++) iss >> vn[i];
            vns_.push_back(vn);
        }
    }
    std::cerr << "NUMBER OF FACES: " << nfaces() << std::endl;
    std::cerr << "NUMBER OF VERTICES: " << nverts() << std::endl;
    std::cerr << "NUMBER OF TEXTURE COORDS: " << ntextcoords() << std::endl;
    std::cerr << "NUMBER OF NORMALS: " << nnormals() << std::endl;
}

Model::~Model() {}

int Model::nverts() { return (int)verts_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

int Model::ntextcoords() { return (int)text_coord_.size(); }

int Model::nnormals() { return (int)vns_.size(); }

std::vector<int> Model::face(int iface) {
    std::vector<int> retval;
    for (auto x : faces_[iface]) {
        retval.push_back(x[0]);
    }

    return retval;
}
void Model::get_uvs(int iface, Vec2f* uvs) {
    float u, v;
    int index;
    auto face = faces_[iface];

    for (int i = 0; i < 3; i++) {
        index = face[i][1];
        u = text_coord_[index].first;
        v = text_coord_[index].second;
        uvs[i] = Vec2f(u, v);
    }
}

TGAColor Model::get_color(float u, float v) {
    int x = static_cast<int>(u * texture.get_width());
    int y = static_cast<int>(v * texture.get_height());
    return texture.get(x, y);
}

Vec3f Model::vert(int i) { return verts_[i]; }
Vec3f Model::get_normal(int iface, int nvert) {
    int index = faces_[iface][nvert][2];
    return vns_[index].normalize();
}