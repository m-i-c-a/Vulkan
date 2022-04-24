#ifndef MICA_LOADER_HPP
#define MICA_LOADER_HPP

#include <vector>
#include <string>

#include "Model.hpp"

std::vector<Model> processGLTF(const std::string &filepath);

#endif // MICA_LOADER_HPP