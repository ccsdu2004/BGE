#ifndef BGE_FONT_TYPE_FACE_H
#define BGE_FONT_TYPE_FACE_H
#include <BGE/System/Math.h>
#include <BGE/System/String.h>
#include <string>
 
namespace bge
{
    namespace Font
    {
        struct Data;
        struct CacheEntry;
    }
}
struct FT_GlyphRec_;
struct FT_BitmapGlyphRec_;

namespace bge
{

namespace Font
{

class TypeFace
{
public:
    explicit TypeFace(const std::string& aFileName, size_t aPointSize = 12, size_t aResolution = 100);
    virtual ~TypeFace();
public:
    //! Returns the point size of the font
    size_t pointSize() const;

    //! Sets the point size of the font
    void setPointSize(size_t aPointSize);

    //! Returns the resolution of the font
    size_t resolution() const;

    //! Sets the resolution of the font
    void setResolution(size_t aResolution);

    //! Renders the passed in string at the passed in position
    void render(const String& aString, const Vector2f& aPosition);

    //! \brief Returns the ascent of this type face in pixels.
    //! \note This is a positive value representing the distance from the baseline to the
    //! highest point used to place an outline point.
    float ascent() const;

    //! \brief Returns the descent of this type face in pixels.
    //! \note This is a \e positive value representing the distance from the baseline to the lowest
    //! point used to place an outline point..
    float descent() const;

    //! Returns the height of a line rendered in this type face
    float lineHeight() const;

    //! \brief Returns the total width of the passed in text in pixels, assuming the text is
    //! rendered on one line
    float width(const String& aString);

    //! \brief Returns the index of the character at the position determined by the passed in
    //! offset from the beginning of the string
    size_t hitCharacterIndex(const String& aString, float anOffset);

    //! Returns the largest theoretically possible glyph size for this face
    Vector2i maximumGlyphSize() const;
private:
    //! \brief Renders the glyph with the passed in index
    //! \returns How far to advance to the position of the next character
    Vector2f renderGlyph(size_t aGlyphIndex, const Vector2f& aPosition);

    //! Adds a new texture to the array of cache textures
    void addNewTexture(const Vector2i& aGlyphSize);

    //! Puts a new entry into the glyph cache
    void addCacheEntry(FT_GlyphRec_* aGlyph, FT_BitmapGlyphRec_* aBitmapGlyph, size_t aGlyphIndex, const Vector2i& aGlyphSize);

    //! Checks that a cache texture is available, adding a new one if necessary
    void ensureTextureIsAvailable();

    //! Returns the cache entry for the glyph with the passed in index
    CacheEntry& cachedGlyph(size_t aGlyphIndex);

    //! Returns the kerning vector for the passed in pair of glyphs
    Vector2f kerning(size_t leftGlyphIndex, size_t rightGlyphIndex) const;

    //! Renders the glyphs of the passed in string
    void renderGlyphs(const String& aString);

    //! binds the passed in cache entry's texture
    void bindTexture(const CacheEntry& aCacheEntry) const;

    void cacheGlyph(size_t aGlyphIndex);  //!< Caches the glyph with the passed in index

    void flushCache(); //! Flushes the string and character cache

    Data* data_;  //!< All member data pertaining to the font is encapsulated an instance of \c Data
};

}

}

#endif
