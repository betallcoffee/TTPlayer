//
//  TTEditGroup.hpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTEditGroup_hpp
#define TTEditGroup_hpp

#include <pthread.h>
#include <vector>

#include "TTFilterFrame.hpp"
#include "TTMaterial.hpp"

namespace TT {
    class EditGroup {
    public:
        EditGroup();
        ~EditGroup();
        
        int materialCount();
        std::shared_ptr<Material> material(int index);
        
        void addMaterial(std::shared_ptr<Material> material);
        void removeMaterial(int index);
        
        void finish();
        
    private:
        static void * editThreadEntry(void *opaque);
        void editThreadLoop();
        
        std::vector<std::shared_ptr<Material>> _materials;
        
        bool _isQuit;
        pthread_t _editThread;
        pthread_mutex_t _editMutex;
    };
}

#endif /* TTEditGroup_hpp */
