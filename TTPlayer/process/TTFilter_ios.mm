//
//  TTFilter_ios.m
//  TTPlayerExample
//
//  Created by liang on 20/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTFilter_ios.h"

@interface TTFilter ()
{
    std::shared_ptr<TT::Filter> _filter;
}

@end

@implementation TTFilter

- (void)dealloc {
    if (_filter) {
        _filter.reset();
    }
}

- (std::shared_ptr<TT::Filter>)filter {
    // TODO no thread safe
    if (_filter == nullptr) {
        std::shared_ptr<TT::Filter_ios> filter = std::make_shared<TT::Filter_ios>();
        if (filter) {
            filter->setObject(self);
        }
        _filter = filter;
    }
    return _filter;
}

@end

#define CHECK_SEL(func_name)  ([object() respondsToSelector:@selector(func_name)])

#define CALL_SEL(func_name) \
do { \
if (CHECK_SEL(func_name)) { \
[object() func_name]; \
} else { \
Filter::func_name(); \
} \
}while(0)

#define CALL_SEL_1(func_name, first) \
do { \
if (CHECK_SEL(func_name:)) { \
[object() func_name:first]; \
} else { \
Filter::func_name(first); \
} \
}while(0)

#define CALL_SEL_RETURN(func_name) \
do { \
if (CHECK_SEL(func_name)) { \
return [object() func_name]; \
} else { \
return Filter::func_name(); \
} \
}while(0)

#define CALL_SEL_RETURN_1(func_name, first) \
do { \
if (CHECK_SEL(func_name:)) { \
return [object() func_name:first]; \
} else { \
return Filter::func_name(first); \
} \
}while(0)

using namespace TT;

bool Filter_ios::bindFramebuffer() {
    CALL_SEL_RETURN(bindFramebuffer);
}

void Filter_ios::compileShader() {
    CALL_SEL(compileShader);
}

const GLchar *Filter_ios::vertexShader() {
    CALL_SEL_RETURN(vertexShader);
}

const GLchar *Filter_ios::fragmentShader() {
    CALL_SEL_RETURN(fragmentShader);
}

void Filter_ios::getAttribLocations() {
    CALL_SEL(getAttribLocations);
}

void Filter_ios::getUniformLocations() {
    CALL_SEL(getUniformLocations);
}

void Filter_ios::resolveAttribLocations() {
    CALL_SEL(resolveAttribLocations);
}

void Filter_ios::resolveUniformLocations() {
    CALL_SEL(resolveUniformLocations);
}

const GLfloat *Filter_ios::positionVertices() {
    CALL_SEL_RETURN(positionVertices);
}

const GLfloat *Filter_ios::texCoordForRotation(TexRotations rotation) {
    CALL_SEL_RETURN_1(texCoordForRotation, rotation);
}

void Filter_ios::updateTexture() {
    CALL_SEL(updateTexture);
}

void Filter_ios::draw() {
    CALL_SEL(draw);
}

void Filter_ios::notifyFramebufferToFilters(int64_t timestamp) {
    CALL_SEL_1(notifyFramebufferToFilters, timestamp);
}


