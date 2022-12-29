#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "model.h"

Model::Model(const char* filename) {
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
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx--;
                f.push_back(idx);
            }
            faces_.push_back(f);
        }
    }
    // std::cerr << "# v# " << verts_.size() << " f# " << faces_.size()
    //           << std::endl;
}

Model::~Model() {}

int Model::nverts() { return (int)verts_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

std::vector<int> Model::face(int idx) { return faces_[idx]; }

Vec3f Model::vert(int i) { return verts_[i]; }