//
//  TTEditGroup.hpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTEditGroup_hpp
#define TTEditGroup_hpp

#include <vector>

#include "TTFilterFrame.hpp"
#include "TTMaterial.hpp"
#include "TTVideo.hpp"

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
        std::vector<std::shared_ptr<Material>> _materials;
    };
}

#endif /* TTEditGroup_hpp */
