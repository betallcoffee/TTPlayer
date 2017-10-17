//
//  TTOpenGLLayer.m
//  TTPlayerExample
//
//  Created by liang on 12/21/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <GLKit/GLKit.h>

#import "TTOpenGLLayer.h"

#pragma mark - shaders

#define STRINGIZE(x) #x
#define STRINGIZE2(x) STRINGIZE(x)
#define SHADER_STRING(text) @ STRINGIZE2(text)

// Attribute index.
enum
{
    ATTRIB_VERTEX,
    ATTRIB_TEXCOORD,
    NUM_ATTRIBUTES
};

NSString *const vertexShaderString = SHADER_STRING
(
 attribute vec4 position;
 attribute vec2 texcoord;
 uniform mat4 modelViewProjectionMatrix;
 varying vec2 v_texcoord;
 
 void main()
 {
     gl_Position = modelViewProjectionMatrix * position;
     v_texcoord = texcoord.xy;
 }
 );

NSString *const rgbFragmentShaderString = SHADER_STRING
(
 varying highp vec2 v_texcoord;
 uniform sampler2D s_texture;
 
 void main()
 {
     gl_FragColor = texture2D(s_texture, v_texcoord);
 }
 );

NSString *const yuvFragmentShaderString = SHADER_STRING
(
 varying highp vec2 v_texcoord;
 uniform sampler2D s_texture_y;
 uniform sampler2D s_texture_u;
 uniform sampler2D s_texture_v;
 
 void main()
 {
     highp float y = texture2D(s_texture_y, v_texcoord).r;
     highp float u = texture2D(s_texture_u, v_texcoord).r - 0.5;
     highp float v = texture2D(s_texture_v, v_texcoord).r - 0.5;
     
     highp float r = y +             1.402 * v;
     highp float g = y - 0.344 * u - 0.714 * v;
     highp float b = y + 1.772 * u;
     
     gl_FragColor = vec4(r,g,b,1.0);
 }
 );

static BOOL validateProgram(GLuint prog)
{
    GLint status;
    
    glValidateProgram(prog);
    
#ifdef DEBUG
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        NSLog(@"Failed to validate program %d", prog);
        return NO;
    }
    
    return YES;
}

static GLuint compileShader(GLenum type, NSString *shaderString)
{
    GLint status;
    const GLchar *sources = (GLchar *)shaderString.UTF8String;
    
    GLuint shader = glCreateShader(type);
    if (shader == 0 || shader == GL_INVALID_ENUM) {
        NSLog( @"Failed to create shader %d", type);
        return 0;
    }
    
    glShaderSource(shader, 1, &sources, NULL);
    glCompileShader(shader);
    
#ifdef DEBUG
    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glDeleteShader(shader);
        NSLog(@"Failed to compile shader:\n");
        return 0;
    }
    
    return shader;
}

static void mat4f_LoadOrtho(float left, float right, float bottom, float top, float near, float far, float* mout)
{
    float r_l = right - left;
    float t_b = top - bottom;
    float f_n = far - near;
    float tx = - (right + left) / (right - left);
    float ty = - (top + bottom) / (top - bottom);
    float tz = - (far + near) / (far - near);
    
    mout[0] = 2.0f / r_l;
    mout[1] = 0.0f;
    mout[2] = 0.0f;
    mout[3] = 0.0f;
    
    mout[4] = 0.0f;
    mout[5] = 2.0f / t_b;
    mout[6] = 0.0f;
    mout[7] = 0.0f;
    
    mout[8] = 0.0f;
    mout[9] = 0.0f;
    mout[10] = -2.0f / f_n;
    mout[11] = 0.0f;
    
    mout[12] = tx;
    mout[13] = ty;
    mout[14] = tz;
    mout[15] = 1.0f;
}


@interface TTOpenGLLayer ()
{
    GLint _uniformSamplers[3];
    GLuint _textures[3];
    
    GLfloat _vertices[8];
}

@property (nonatomic, strong) EAGLContext *glContext;

@property (nonatomic, assign) GLuint framebuffer;
@property (nonatomic, assign) GLuint colorRenderbuffer;
@property (nonatomic, assign) GLuint program;
@property (nonatomic, assign) GLint uniformMatrix;

@property (nonatomic, assign) GLint framebufferWidth;
@property (nonatomic, assign) GLint framebufferHeight;

@end

@implementation TTOpenGLLayer

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _framebuffer = 0;
        _colorRenderbuffer = 0;
        [self setup];
    }
    
    return self;
}

- (void)dealloc {
    [self teardownBuffers];
}

- (void)setup {
    CAEAGLLayer* gllayer = (CAEAGLLayer*)self;
    gllayer.opaque = YES;
    gllayer.drawableProperties = @{ kEAGLDrawablePropertyRetainedBacking : @(YES) };
    
    self.glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    [self setupBuffers];
}

- (void)setupBuffers {
    if (!self.glContext || ![EAGLContext setCurrentContext:self.glContext]) {
        return;
    }
    // set up frame buffer
    if (_framebuffer == 0) {
        glGenFramebuffers(1, &_framebuffer);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    
    // set up color render buffer
    if (_colorRenderbuffer == 0) {
        glGenRenderbuffers(1, &_colorRenderbuffer);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderbuffer);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderbuffer);
    
    [self.glContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_framebufferWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_framebufferHeight);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
}

- (BOOL)loadShaders
{
    BOOL result = NO;
    GLuint vertShader = 0, fragShader = 0;
    
    _program = glCreateProgram();
    
    vertShader = compileShader(GL_VERTEX_SHADER, vertexShaderString);
    if (!vertShader)
        goto exit;
    
    fragShader = compileShader(GL_FRAGMENT_SHADER, yuvFragmentShaderString);
    if (!fragShader)
        goto exit;
    
    glAttachShader(_program, vertShader);
    glAttachShader(_program, fragShader);
    glBindAttribLocation(_program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(_program, ATTRIB_TEXCOORD, "texcoord");
    
    glLinkProgram(_program);
    
    GLint status;
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        NSLog(@"Failed to link program %d", _program);
        goto exit;
    }
    
    result = validateProgram(_program);
    
    _uniformMatrix = glGetUniformLocation(_program, "modelViewProjectionMatrix");
    _uniformSamplers[0] = glGetUniformLocation(_program, "s_texture_y");
    _uniformSamplers[1] = glGetUniformLocation(_program, "s_texture_u");
    _uniformSamplers[2] = glGetUniformLocation(_program, "s_texture_v");
    
exit:
    
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    
    if (result) {
        
        NSLog(@"OK setup GL programm");
        
    } else {
        
        glDeleteProgram(_program);
        _program = 0;
    }
    
    return result;
}

- (void)teardownBuffers {
    if (self.framebuffer) {
        glDeleteFramebuffers(1, &_framebuffer);
        self.framebuffer = 0;
    }
    if (self.colorRenderbuffer) {
        glDeleteRenderbuffers(1, &_colorRenderbuffer);
        self.colorRenderbuffer = 0;
    }
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
}

- (void)drawFrame {
    // bind framebuffer & set viewport
    glBindFramebuffer(GL_FRAMEBUFFER, self.framebuffer);
    glViewport(0, 0, self.framebufferWidth, self.framebufferHeight);
    
    // bind shader program
    
    // set up vertices
    GLfloat vertices[] = {
        -0.5f, -0.5f, -1.0f,
        0.0f, 0.5f, -1.0f,
        0.5f, -0.5f, -1.0f,
    };
    
    // set up colors
    CGFloat colors[] = {
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
    };
    
    // draw triangle
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glEnableVertexAttribArray(GLKVertexAttribColor);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(GLKVertexAttribColor, 4, GL_FLOAT, GL_FALSE, 0, colors);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // present render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderbuffer);
    [self.glContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)updateVertices
{
    const float w  = 1;
    const float h  = 1;
    
    _vertices[0] = - w;
    _vertices[1] = - h;
    _vertices[2] =   w;
    _vertices[3] = - h;
    _vertices[4] = - w;
    _vertices[5] =   h;
    _vertices[6] =   w;
    _vertices[7] =   h;
}

- (void)displayPixels:(const UInt8 **)pixels width:(const NSUInteger)width height:(const NSUInteger)height
{
    [self updateVertices];
    static const GLfloat texCoords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };
    
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glViewport(0, 0, self.framebufferWidth, self.framebufferHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(_program);
    
    if (pixels) {
        [self setPixels:pixels width:width height:height];
    }
    
    if ([self prepareRender]) {
        
        GLfloat modelviewProj[16];
        mat4f_LoadOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, modelviewProj);
        glUniformMatrix4fv(_uniformMatrix, 1, GL_FALSE, modelviewProj);
        
        glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, _vertices);
        glEnableVertexAttribArray(ATTRIB_VERTEX);
        glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, 0, 0, texCoords);
        glEnableVertexAttribArray(ATTRIB_TEXCOORD);
        
#if 0
        if (!validateProgram(_program))
        {
            LoggerVideo(0, @"Failed to validate program");
            return;
        }
#endif
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderbuffer);
    [self.glContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void) setPixels:(const UInt8 **)pixels width:(const NSUInteger)width height:(const NSUInteger)height
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    if (0 == _textures[0])
        glGenTextures(3, _textures);
    
    const NSUInteger widths[3]  = { width, width / 2, width / 2 };
    const NSUInteger heights[3] = { height, height / 2, height / 2 };
    
    for (int i = 0; i < 3; ++i) {
        
        glBindTexture(GL_TEXTURE_2D, _textures[i]);
        
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_LUMINANCE,
                     widths[i],
                     heights[i],
                     0,
                     GL_LUMINANCE,
                     GL_UNSIGNED_BYTE,
                     pixels[i]);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

- (BOOL) prepareRender
{
    if (_textures[0] == 0)
        return NO;
    
    for (int i = 0; i < 3; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, _textures[i]);
        glUniform1i(_uniformSamplers[i], i);
    }
    
    return YES;
}


@end

/*
*函数调用步骤如下:
*
* [初始化]
* glutInit(): 初始化glut库。
* glutInitDisplayMode(): 设置显示模式。
* glutCreateWindow(): 创建一个窗口。
* glewInit(): 初始化glew库。
* glutDisplayFunc(): 设置绘图函数（重绘的时候调用）。
* glutTimerFunc(): 设置定时器。
* InitShaders(): 设置Shader。包含了一系列函数，暂不列出。
* glutMainLoop(): 进入消息循环。
*
* [循环渲染数据]
* glActiveTexture(): 激活纹理单位。
* glBindTexture(): 绑定纹理
* glTexImage2D(): 根据像素数据，生成一个2D纹理。
* glUniform1i():
* glDrawArrays(): 绘制。
* glutSwapBuffers(): 显示。
*
* This software plays YUV raw video data using OpenGL.
* It support read YUV420P raw file and show it on the screen.
* It's use a slightly more complex technologies such as Texture,
* Shaders etc. Suitable for beginner who already has some
* knowledge about OpenGL.
*
* The process is shown as follows:
*
* [Init]
* glutInit(): Init glut library.
* glutInitDisplayMode(): Set display mode.
* glutCreateWindow(): Create a window.
* glewInit(): Init glew library.
* glutDisplayFunc(): Set the display callback.
* glutTimerFunc(): Set timer.
* InitShaders(): Set Shader, Init Texture. It contains some functions about Shader.
* glutMainLoop(): Start message loop.
*
* [Loop to Render data]
* glActiveTexture(): Active a Texture unit
* glBindTexture(): Bind Texture
* glTexImage2D(): Specify pixel data to generate 2D Texture
* glUniform1i():
* glDrawArrays(): draw.
* glutSwapBuffers(): show.
*/
