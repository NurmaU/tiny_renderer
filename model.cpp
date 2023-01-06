#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

Model::Model(const char* filename, const char* texture_filename,
             const char* norm_filename, const char* spec_filename) {
    // read texture
    texture.read_tga_file(texture_filename);
    texture.flip_vertically();

    // read normal mapping
    norm_map.read_tga_file(norm_filename);
    norm_map.flip_vertically();

    // read spec
    specular.read_tga_file(spec_filename);
    specular.flip_vertically();

    std::cerr << "TEXTURE IMAGE HEIGHT: " << texture.get_height() << std::endl;
    std::cerr << "TEXTURE IMAGE WIDTH: " << texture.get_width() << std::endl;

    std::cerr << "NORMAL MAPPING HEIGHT: " << norm_map.get_height()
              << std::endl;
    std::cerr << "NORMAL MAPPING WIDTH: " << norm_map.get_width() << std::endl;

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
Vec2f Model::get_uvs(int iface, int nthvert) {
    int index = faces_[iface][nthvert][1];
    return {text_coord_[index].first, text_coord_[index].second};
}

TGAColor Model::diffuse(Vec2f uv) {
    int x = static_cast<int>(uv[0] * texture.get_width());
    int y = static_cast<int>(uv[1] * texture.get_height());
    return texture.get(x, y);
}

Vec3f Model::vert(int i, int j) { return verts_[faces_[i][j][0]]; }
Vec3f Model::get_normal(int iface, int nvert) {
    int index = faces_[iface][nvert][2];
    return vns_[index].normalize();
}

Vec3f Model::get_normmap(Vec2f uv) {
    int x = static_cast<int>(uv[0] * norm_map.get_width());
    int y = static_cast<int>(uv[1] * norm_map.get_height());
    auto c = norm_map.get(x, y);

    Vec3f c_vec = {float(c.bgra[0]), float(c.bgra[1]), float(c.bgra[2])};

    Vec3f retval;
    for (int i = 0; i < 3; i++) {
        retval[2 - i] = c_vec[i] / 255 * 2 - 1;
    }
    return retval;
}

float Model::get_specular(Vec2f uv) {
    int x = static_cast<int>(uv[0] * specular.get_width());
    int y = static_cast<int>(uv[1] * specular.get_height());
    return specular.get(x, y)[0];
}