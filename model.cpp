#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "model.h"

Model::Model(const char* filename, const char* texture_filename) {
    // read texture
    texture.read_tga_file(texture_filename);

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
            std::vector<int> f;
            std::vector<int> ti;
            int itrash, idx, text_idx;
            iss >> trash;
            while (iss >> idx >> trash >> text_idx >> trash >> itrash) {
                idx--;
                f.push_back(idx);
                ti.push_back(text_idx);
            }
            faces_.push_back(f);
            text_indexes_.push_back(ti);
        } else if (!line.compare(0, 2, "vt")) {
            std::vector<float> coord;
            iss >> trash;
            float value;

            for (int i = 0; i < 2; i++) {
                iss >> value;
                coord.push_back(value);
            }
            text_coord_.push_back(coord);
        }
    }
    std::cerr << "NUMBER OF FACES: " << nfaces() << std::endl;
    std::cerr << "NUMBER OF VERTICES: " << nverts() << std::endl;
    std::cerr << "NUMBER OF TEXTURE COORDS: " << ntextcoords() << std::endl;
    std::cerr << "NUMBER OF TEXTURE INDEXES: " << ntextinds() << std::endl;
}

Model::~Model() {}

int Model::nverts() { return (int)verts_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

int Model::ntextcoords() { return (int)text_coord_.size(); }

int Model::ntextinds() { return (int)text_indexes_.size(); }

std::vector<int> Model::face(int idx) { return faces_[idx]; }

Vec3f Model::vert(int i) { return verts_[i]; }