//
//  TTEditGroup.cpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTEditGroup.hpp"

using namespace TT;

EditGroup::EditGroup() {
    
}

EditGroup::~EditGroup() {
    
}

int EditGroup::materialCount() {
    return (int)_materials.size();
}

std::shared_ptr<Material> EditGroup::material(int index) {
    if (0 <= index && index < _materials.size()) {
        return _materials[index];
    }
    return nullptr;
}

void EditGroup::addMaterial(std::shared_ptr<Material> edit) {
    _materials.push_back(edit);
}

void EditGroup::removeMaterial(int index) {
    if (index >= 0 && index < _materials.size()) {
        _materials.erase(_materials.begin() + index);
    }
}

void EditGroup::finish() {
    if (!_materials.empty()) {
        std::for_each(_materials.begin(), _materials.end(), [&](std::shared_ptr<Material> material) {
            material->process();
        });
    }
}




