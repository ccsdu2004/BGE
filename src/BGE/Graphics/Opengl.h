#ifndef BGE_GRAPHICS_OPENGL_H
#define BGE_GRAPHICS_OPENGL_H
#if defined(WIN32)
    #include <windows.h>
    #include <gl/gl.h>
    #include <gl/glu.h>
#endif

#ifndef GL_EXT_texture_object
    #define GL_TEXTURE_PRIORITY_EXT           0x8066
    #define GL_TEXTURE_RESIDENT_EXT           0x8067
    #define GL_TEXTURE_1D_BINDING_EXT         0x8068
    #define GL_TEXTURE_2D_BINDING_EXT         0x8069
    #define GL_TEXTURE_3D_BINDING_EXT         0x806A
#endif

#endif
