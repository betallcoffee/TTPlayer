//
//  TTEditGroup.cpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include <unistd.h>

#include "easylogging++.h"

#include "TTMutex.hpp"
#include "TTEditGroup.hpp"

using namespace TT;

EditGroup::EditGroup() : _isQuit(false),
_editMutex(PTHREAD_MUTEX_INITIALIZER) {
    pthread_create(&_editThread, nullptr, EditGroup::editThreadEntry, this);
}

EditGroup::~EditGroup() {
    
}

int EditGroup::materialCount() {
    return (int)_materials.size();
}

std::shared_ptr<Material> EditGroup::material(int index) {
    Mutex m(&_editMutex);
    if (0 <= index && index < _materials.size()) {
        return _materials[index];
    }
    return nullptr;
}

void EditGroup::addMaterial(std::shared_ptr<Material> edit) {
    Mutex m(&_editMutex);
    _materials.push_back(edit);
}

void EditGroup::removeMaterial(int index) {
    Mutex m(&_editMutex);
    if (index >= 0 && index < _materials.size()) {
        _materials.erase(_materials.begin() + index);
    }
}

void EditGroup::finish() {
    // TODO
}

void *EditGroup::editThreadEntry(void *opaque) {
    pthread_setname_np("edit group thread");
    EditGroup *self = (EditGroup *)opaque;
    self->editThreadLoop();
    return nullptr;
}

void EditGroup::editThreadLoop() {
    while (true) {
        Mutex m(&_editMutex);
        if (_isQuit) break;
        
        bool process = false;
        if (!_materials.empty()) {
            std::for_each(_materials.begin(), _materials.end(), [&](std::shared_ptr<Material> material) {
                process = material->process();
            });
        }
        
        if (!process) {
            usleep(1);
        }
    }
}




