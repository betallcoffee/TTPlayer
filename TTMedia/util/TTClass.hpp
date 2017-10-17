//
//  TTClass.hpp
//  TTPlayerExample
//
//  Created by liang on 15/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTClass_hpp
#define TTClass_hpp

namespace TT {
    
#define Singleton(class_name) \
private: \
class_name (); \
virtual ~class_name ();
    
#define NoCopy(class_name) \
private: \
class_name (const class_name &) = delete; \
class_name &operator=(const class_name &) = delete;
    
}


#endif /* TTClass_hpp */
