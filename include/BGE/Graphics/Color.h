#ifndef BGE_GRAPHICS_COLOR_H
#define BGE_GRAPHICS_COLOR_H
#include <BGE/System/Serialization.h>
#include <cstdint>

namespace bge
{

class BGE_EXPORT_API Color
{
public :
    Color();
    Color(float red,float green,float blue,float alpha = 1.0f);
public:
    static const Color Black;       ///< Black predefined color
    static const Color White;       ///< White predefined color
    static const Color Red;         ///< Red predefined color
    static const Color Green;       ///< Green predefined color
    static const Color Blue;        ///< Blue predefined color
    static const Color Yellow;      ///< Yellow predefined color
    static const Color Magenta;     ///< Magenta predefined color
    static const Color Cyan;        ///< Cyan predefined color
    static const Color Transparent; ///< Transparent (black) predefined color

    float r()const{return r_;}
    float g()const{return g_;}
    float b()const{return b_;}
    float a()const{return a_;}

    float r_; ///< Red component
    float g_; ///< Green component
    float b_; ///< Blue component
    float a_; ///< Alpha (opacity) component
};

BGE_EXPORT_API bool operator ==(const Color& left, const Color& right);
BGE_EXPORT_API bool operator !=(const Color& left, const Color& right);

void BGE_EXPORT_API serialize(const ObjectNamer<Color>& object,TiXmlNode* node);

template<>
inline Color deserialize<>(const char* name,TiXmlNode* node)
{
    TiXmlNode* element = node->FirstChildElement(name);
    Color color;
    color.r_ = deserialize<float>("r",element);
    color.g_ = deserialize<float>("g",element);
    color.b_ = deserialize<float>("b",element);
    color.a_ = deserialize<float>("a",element);
    return color;
}

}

#endif

