#ifndef BGE_SYSTEM_ENUM_H
#define BGE_SYSTEM_ENUM_H
#include <BGE/System/String.h>

namespace bge
{

template<typename EnumType,EnumType start,EnumType end,typename Names>
class Enum
{
public:
    Enum(EnumType value = start) : value_(value) {}
public:
    operator EnumType () const { return value_; }
public:
    bool operator== (EnumType value) const { return value == value_; }
    bool operator!= (EnumType value) const { return value != value_; }
public:
    EnumType valueFromName(const std::string& name)
    {
        int index = int(start);
        while(std::string(name) != Names::name(index-start))
        {
            ++index;
            if(index > int(end))
                return value_;
        }
        return static_cast<EnumType>(index);
    }

    EnumType value_;
};

#define Enum2(Name, e1, e2) \
    enum E##Name { e1 = 0x00, e2 }; \
    class NamesE##Name \
    { \
    public: \
        static const std::string name(const E##Name& value) \
        { \
            int offset = (int)value - e1;\
            if(offset > 1)\
                offset = 0;\
            static const std::string names[] = { #e1, #e2 }; \
            return names[offset]; \
        } \
        static E##Name fromString(const std::string& str)\
        {\
            if(name(e1) == str)\
                return e1;\
            return e2;\
        }\
    }; \
    typedef Enum<E##Name,e1,e2,NamesE##Name> Name;
#define Enum3(Name,e1,e2,e3) \
    enum E##Name { e1 = 0x00,e2,e3 }; \
    class NamesE##Name \
    { \
    public: \
        static const std::string name(const E##Name& value) \
        { \
            int offset = (int)value - e1;\
            if(offset > 2)\
                offset = 0;\
            static const std::string names[] = { #e1,#e2,#e3 }; \
            return names[offset]; \
        } \
        static E##Name fromString(const std::string& str)\
        {\
            if(name(e1) == str)\
                return e1;\
            if(name(e2) == str)\
                return e2;\
            return e3;\
        }\
    }; \
    typedef Enum<E##Name,e1,e3,NamesE##Name> Name;
#define Enum4(Name,e1,e2,e3,e4) \
    enum E##Name { e1 = 0x00,e2,e3,e4 }; \
    class NamesE##Name \
    { \
    public: \
        static const std::string name(const E##Name& value) \
        { \
            int offset = (int)value - e1;\
            if(offset > 3)\
                offset = 0;\
            static const std::string names[] = { #e1,#e2,#e3,#e4 }; \
            return names[offset]; \
        } \
        static E##Name fromString(const std::string& str)\
        {\
            if(name(e1) == str)\
                return e1;\
            else if(name(e2) == str)\
                return e2;\
            else if(name(e3) == str)\
                return e3;\
            return e4;\
        }\
    };\
    typedef Enum<E##Name,e1,e4,NamesE##Name> Name;
#define Enum5(Name,e1,e2,e3,e4,e5) \
    enum E##Name { e1 = 0x00,e2,e3,e4,e5}; \
    class NamesE##Name \
    { \
    public: \
        static const std::string name(const E##Name& value) \
        { \
            int offset = (int)value - e1;\
            if(offset > 4)\
                offset = 0;\
            static const std::string names[] = { #e1,#e2,#e3,#e4,#e5 }; \
            return names[offset]; \
        } \
        static E##Name fromString(const std::string& str)\
        {\
            if(name(e1) == str)\
                return e1;\
            else if(name(e2) == str)\
                return e2;\
            else if(name(e3) == str)\
                return e3;\
            else if(name(e4) == str)\
                return e4;\
            return e5;\
        }\
    }; \
    typedef Enum<E##Name,e1,e5,NamesE##Name> Name;
#define Enum6(Name,e1,e2,e3,e4,e5,e6) \
    enum E##Name { e1 = 0x00,e2,e3,e4,e5,e6}; \
    class NamesE##Name \
    { \
    public: \
        static const std::string name(const E##Name& value) \
        { \
            int offset = (int)value - e1;\
            if(offset > 5)\
                offset = 0;\
            static const std::string names[] = { #e1,#e2,#e3,#e4,#e5,#e6 }; \
            return names[offset]; \
        } \
        static E##Name fromString(const std::string& str)\
        {\
            if(name(e1) == str)\
                return e1;\
            else if(name(e2) == str)\
                return e2;\
            else if(name(e3) == str)\
                return e3;\
            else if(name(e4) == str)\
                return e4;\
            else if(name(e5) == str)\
                return e5;\
            return e6;\
        }\
    }; \
    typedef Enum<E##Name,e1,e6,NamesE##Name> Name;
#define Enum9(Name,e1,e2,e3,e4,e5,e6,e7,e8,e9) \
    enum E##Name { e1 = 0x00,e2,e3,e4,e5,e6,e7,e8,e9}; \
    class NamesE##Name \
    { \
    public: \
        static const std::string name(const E##Name& value) \
        { \
            int offset = (int)value - e1;\
            if(offset > 8)\
                offset = 0;\
            static const std::string names[] = { #e1,#e2,#e3,#e4,#e5,#e6,#e7,#e8,#e9 }; \
            return names[offset]; \
        } \
        static E##Name fromString(const std::string& str)\
        {\
            if(name(e1) == str)\
                return e1;\
            else if(name(e2) == str)\
                return e2;\
            else if(name(e3) == str)\
                return e3;\
            else if(name(e4) == str)\
                return e4;\
            else if(name(e5) == str)\
                return e5;\
            else if(name(e6) == str)\
                return e6;\
            else if(name(e7) == str)\
                return e7;\
            else if(name(e8) == str)\
                return e8;\
            return e9;\
        }\
    }; \
    typedef Enum<E##Name,e1,e9,NamesE##Name> Name;

Enum3(RectangleStyle,RectangleStyle_normal,RectangleStyle_dropped,RectangleStyle_raised)
Enum2(Mapping,Mapping_tiled,Mapping_stretched)
Enum9(FrameSection,FrameSection_right,FrameSection_topRight,FrameSection_top,FrameSection_topLeft,FrameSection_left,FrameSection_bottomLeft,FrameSection_bottom,FrameSection_bottomRight,FrameSection_title)
Enum2(Orientation,Orientation_horizontal,Orientation_vertical)
Enum6(TickPlacement,TickPlacement_none,TickPlacement_left,TickPlacement_below,TickPlacement_right,TickPlacement_above,TickPlacement_both)
Enum3(Horizontal,Horizontal_left,Horizontal_hCenter,Horizontal_right)
Enum3(Vertical,Vertical_top,Vertical_vCenter,Vertical_bottom)

template <>
inline std::string toString<Orientation>(const Orientation& o)
{
    if(o == Orientation_horizontal)
        return "horizontal";
    return "vertical";
}

template <>
inline std::string toString<Mapping>(const Mapping& map)
{
    if(map == Mapping_tiled)
        return "tiled";
    return "stretched";
}

template <>
inline std::string toString<Horizontal>(const Horizontal& horizontal)
{
    if(horizontal == Horizontal_left)
        return "left";
    else if(horizontal == Horizontal_hCenter)
        return "hCenter";
    return "right";
}

template <>
inline std::string toString<Vertical>(const Vertical& vertical)
{
    if(vertical == Vertical_top)
        return "top";
    else if(vertical == Vertical_vCenter)
        return "vCenter";
    return "bottom";
}

template <>
inline std::string toString<RectangleStyle>(const RectangleStyle& style)
{
    if(style == RectangleStyle_normal)
        return "normal";
    else if(style == RectangleStyle_dropped)
        return "dropped";
    return "raised";
}

template <>
inline std::string toString<TickPlacement>(const TickPlacement& placement)
{
    std::string str;
    switch(placement)
    {
    case TickPlacement_none:
        str = "none";
        break;
    case TickPlacement_left:
        str = "left";
        break;
    case TickPlacement_below:
        str = "below";
        break;
    case TickPlacement_right:
        str = "right";
        break;
    case TickPlacement_above:
        str = "above";
        break;
    default:
        str = "both";
    }
    return str;
}

template <>
inline std::string toString<FrameSection>(const FrameSection& section)
{
    if(section == FrameSection_right)
        return "right";
    else if(section == FrameSection_topRight)
        return "topRight";
    else if(section == FrameSection_top)
        return "top";
    else if(section == FrameSection_topLeft)
        return "topLeft";
    else if(section == FrameSection_left)
        return "left";
    else if(section == FrameSection_bottomLeft)
        return "bottomLeft";
    else if(section == FrameSection_bottom)
        return "bottom";
    else if(section == FrameSection_bottomRight)
        return "bottomRight";
    return "title";
}

template <>
inline Orientation fromString<Orientation>(const std::string& str)
{
    Orientation o;
    if(str == "horizontal")
        o = Orientation_horizontal;
    else
        o = Orientation_vertical;
    return o;
};

template <>
inline Mapping fromString<Mapping>(const std::string& str)
{
    Mapping map;
    if(str == "tiled")
        return Mapping_tiled;
    return Mapping_stretched;
};

template <>
inline Horizontal fromString<Horizontal>(const std::string& str)
{
    if(str == "left")
        return Horizontal_left;
    else if(str == "hCenter")
        return Horizontal_hCenter;
    return Horizontal_right;
}

template <>
inline Vertical fromString<Vertical>(const std::string& str)
{
    if(str == "top")
        return Vertical_top;
    else if(str == "vCenter")
        return Vertical_vCenter;
    return Vertical_bottom;
}

template <>
inline TickPlacement fromString<TickPlacement>(const std::string& str)
{
    TickPlacement tick;
    if(str == "none")
        tick = TickPlacement_none;
    else if(str == "left")
        tick = TickPlacement_left;
    else if(str == "below")
        tick = TickPlacement_below;
    else if(str == "right")
        tick = TickPlacement_right;
    else if(str == "above")
        tick = TickPlacement_above;
    else
        tick = TickPlacement_both;
    return tick;
};

template <>
inline RectangleStyle fromString<RectangleStyle>(const std::string& str)
{
    RectangleStyle style;
    if(str == "normal")
        style = RectangleStyle_normal;
    else if(str == "dropped")
        style = RectangleStyle_dropped;
    else
        style = RectangleStyle_raised;
    return style;
};

template <>
inline FrameSection fromString<FrameSection>(const std::string& str)
{
    if(str == "right")
        return FrameSection_right;
    else if(str == "topRight")
        return FrameSection_topRight;
    else if(str == "top")
        return FrameSection_top;
    else if(str == "topLeft")
        return FrameSection_topLeft;
    else if(str == "left")
        return FrameSection_left;
    else if(str == "bottomLeft")
        return FrameSection_bottomLeft;
    else if(str == "bottom")
        return FrameSection_bottom;
    else if(str == "bottomRight")
        return FrameSection_bottomRight;
    return FrameSection_title;
}

}

#endif // BGE_SYSTEM_ENUM_H
