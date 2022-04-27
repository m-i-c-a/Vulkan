#ifndef LOADER_HPP
#define LOADER_HPP

#include <vector>
#include <string>

#include "Model.hpp"

std::vector<Model> loadModel(const std::string &filepath);

#endif // LOADER_HPP