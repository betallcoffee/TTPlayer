//
//  TTImageView.h
//  TTPlayerExample
//
//  Created by liang on 17/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <UIKit/UIKit.h>

#include "TTFilter.hpp"
#include "TTClass.hpp"

@interface TTImageView : UIView

@property(readonly, nonatomic) CGSize sizeInPixels;

@end

namespace TT {
    class ImageView : public Filter {
    public:
        ImageView();
        ~ImageView();
        
        TTImageView *imageView() { return (__bridge TTImageView*)_imageView; }
        
    protected:
        virtual bool bindFramebuffer();
        virtual const GLfloat *positionVertices();
        virtual const GLfloat *texCoordForRotation(TexRotations rotation);
        virtual void draw();
        
    private:
        void *_imageView;
        
        NoCopy(ImageView);
    };
}
