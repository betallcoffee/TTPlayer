//
//  TTRender.hpp
//  TTPlayerExample
//
//  Created by liang on 9/12/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTRender_hpp
#define TTRender_hpp

#include <memory>
#include <OpenGLES/ES2/gl.h>

#include "TTFrame.hpp"

#define STRINGIZE(x) #x

namespace TT {
    
    class Render;
    
    typedef enum {
        kNoRotation, // 无旋转
        kRotateLeft, // 向左旋
        kRotateRight, // 向右旋
        kFlipVertical, // 垂直翻转
        kFlipHorizonal, // 水平翻转
        kRotate180 // 旋转 180 度
    } Rotations;
    
    typedef enum {
        kContentModeScaleToFill,
        kContentModeScaleAspectFit,
        kContentModeScaleAspectFill,
    } ContentMode;
    
    typedef struct RenderContext {
        void *opaque;
        bool (*setup)(void *opaque, Render *render);
        void (*teardown)(void *opaque);
        bool (*display)(void *opaque, std::shared_ptr<Frame> frame);
    } RenderContext;
    
    class Shader {
    public:
        Shader();
        virtual ~Shader() {}
        
        DataType type() { return _type; }
        
        virtual bool resolveUniforms(GLuint program);
        virtual bool uploadTexture(std::shared_ptr<Frame> frame) = 0;
        
        const GLchar *vertexShader() { return _vertexShader; }
        const GLchar *fragmentShader() { return _fragmentShader; }
        
        int vertexLocation() { return _vertexLocation; }
        int texCoordLocation() { return _texCoordLocation; }
        
    protected:
        DataType _type;
        
        const GLchar *_vertexShader;
        const GLchar *_fragmentShader;
        
        int  _vertexLocation;
        int  _texCoordLocation;
        int  _modelViewProjectionMatrixLocation;
    };
    
    class Render {
    public:
        explicit Render();
        ~Render();
        
        void bindContext(const RenderContext &context);
        bool displayFrame(std::shared_ptr<Frame> frame);
        
        bool createFrameBuffer();
        bool bindFrameBuffer();
        bool createRenderBuffer();
        bool bindRenderBuffer();
        bool attachRenderBuffer();
        
        bool loadShader();
        
        bool createProjectMatrix();
        bool createModelViewMatrix();
        bool bindMatrix();
        
        bool updateBuffers(int sarNum, int sarDen,
                           float width, float height);
        
        bool uploadTexture(std::shared_ptr<Frame> frame);
        
    private:
        Render(const Render &r);
        const Render & operator=(const Render &r);
        
        GLuint compileShader(GLenum type, const GLchar *source);
        bool validateProgram(GLuint prog);
        
        void updateVerticesBuffers(int sarNum, int sarDen, float width, float height);
        void updateTexCoordsBuffers(float width, float heigth);
        void updateIndicesBuffers();
        void generateSquare(int sarNum, int sarDen,
                       float width, float height,
                       GLfloat **vertices,GLfloat **texCoords,
                       GLuint **indices, GLsizeiptr *verticesSize,
                       GLsizeiptr *texCoordsSize, GLsizeiptr *indicesSize);
        
    private:
        RenderContext _renderCtx;
        DataType _textureType;
        
        GLuint _framebuffer;
        GLuint _renderbuffer;
        GLint  _backingWidth;
        GLint  _backingHeight;
        
        GLuint _program;
        std::shared_ptr<Shader> _shader;
        
        GLuint _verticesBuffer;
        GLuint _texCoordsBuffer;
        GLuint _indicesBuffer;
        GLsizei _indicesCount;
        
        ContentMode _contentMode;
        Rotations _rotationMode;
    };
}

#endif /* TTRender_hpp */
