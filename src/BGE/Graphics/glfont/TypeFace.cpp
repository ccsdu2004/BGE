#include <cstddef>
#include <vector>
#include <map>
#include <cstring>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <BGE/System/Math.h>
#include <BGE/System/Err.h>
#include <BGE/System/IO.h>
#include <BGE/Graphics/OpenGL.h>
#include <BGE/Graphics/CanvasImpl.h>
#include <BGE/GUI/WindowManager.h>
#include <algorithm>
#include "TypeFace.h"

namespace bge
{

unsigned long read(FT_Stream rec,unsigned long offset,unsigned char* buffer,unsigned long count)
{
    IOReader* reader = (IOReader*)rec->descriptor.pointer;
    if(!reader)
        return 0;
    reader->seek(offset,SEEK_SET);
    return static_cast<unsigned long>(reader->read(reinterpret_cast<char*>(buffer),sizeof(unsigned char),count));
}

void close(FT_Stream rec)
{
}

FT_Face getFaceByFileName(const std::string& filename,bool& ok)
{
    FT_Face face;
    ok = false;

    IOReader* reader = FileSystem::instance().readFile(filename);
    if(!reader)
    {
        return face;
    }

    FT_StreamRec* rec = new FT_StreamRec;
    std::memset(rec,0,sizeof(*rec));
    rec->base = NULL;
    rec->size = reader->size();
    rec->pos = 0;
    rec->descriptor.pointer = reader;
    rec->read = read;
    rec->close = close;

    FT_Open_Args args;
    args.flags  = FT_OPEN_STREAM;
    args.stream = rec;
    args.driver = 0;

    CanvasImpl* canvas = (CanvasImpl*)WindowManager::instance().canvas();
    if(FT_Open_Face(canvas->ftLibrary(),&args,0,&face) != 0)
    {
        err() << "failed to load font from stream (failed to create the font face)" << std::endl;
        delete rec;
        return face;
    }

    if(FT_Select_Charmap(face,FT_ENCODING_UNICODE) != 0)
    {
        err() << "failed to load font from stream (failed to set the Unicode character set)" << std::endl;
        FT_Done_Face(face);
        delete rec;
        return face;
    }
    ok = true;
    return face;
}

namespace Font
{

struct CacheEntry
{
    size_t        textureIndex_;   //!< Index into the TypeFace's texture array
    Vector2f topLeftUV_;      //!< Texcoords for the top left corner of the glyph
    Vector2f bottomRightUV_;  //!< Texcoords for the bottom right corner of the glyph
    Vector2i    renderSize_;     //!< The size of the glyph in pixels on the render target
    Vector2f advance_;        //!< The character advance for the cached glyph
    Vector2f bitmapPosition_; //!< The offset from the pen to the top left corner of the bitmap
};

struct TextureInfo
{
    typedef GLuint Handle; //!< typedef for a texture handle for an OpenGL texture
    Handle     handle_;    //!< The OpenGL texture handle
    Vector2i size_;      //!< The size of the texture in texels
};

typedef std::vector<TextureInfo>      Textures;       //!< An array of textures
typedef std::map<FT_UInt, CacheEntry> CharacterCache; //!< The cache maps glyphs to cache entries

struct Data
{
    Data(size_t aPointSize, size_t aResolution) :
        pointSize_(aPointSize), resolution_(aResolution) {}

    size_t         pointSize_;      //!< The size of the type face in points
    size_t         resolution_;     //!< The resolution of the type face in dpi
    FT_Face        face_;           //!< A handle to the FreeType face object
    CharacterCache characterCache_; //!< The glyph cache
    Vector2i offset_;         //!< The top left corner of the next glyph to be cached
    Textures       textures_;       //!< The texture sizes and handles obtained from OpenGL
    bool           hasKerning_;     //!< Set to true if the type face supports kerning
};

const size_t glyphPadding = 1;     //!< Glyphs have padding around them to prevent filtering issues
const float OneOver64 = 0.015625f; //!< A frequently used constant

size_t nextHighestPowerOf2(size_t aValue)
{
    size_t v = aValue - 1;
    for(int i = 16; i > 0; i >>= 1) v |= (v >> i);
    return (v + 1);
}

size_t maximumTextureSize()
{
    GLint maxSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    if(maxSize > 512) maxSize = 512;
    return static_cast<size_t>(maxSize);
}

size_t cacheTextureWidth(size_t aGlyphWidth, size_t numGlyphsToCache)
{
    size_t width = nextHighestPowerOf2( ( (aGlyphWidth + glyphPadding) * numGlyphsToCache) + glyphPadding );
    if(width > maximumTextureSize()) width = maximumTextureSize();
    return width;
}

Vector2i cacheTextureSize(const Vector2i& aGlyphSize, size_t numGlyphsToCache)
{
    size_t textureWidth = cacheTextureWidth(aGlyphSize.width(), numGlyphsToCache);

    const size_t numGlyphsInRow = (textureWidth - (glyphPadding * 2)) / aGlyphSize.width();
    const size_t requiredHeight = ((numGlyphsToCache / numGlyphsInRow) + 1) * aGlyphSize.height();
    size_t textureHeight = nextHighestPowerOf2(requiredHeight);
    if(textureHeight > maximumTextureSize()) textureHeight = maximumTextureSize();

    return Vector2i(textureWidth, textureHeight);
}

void setPixelStorage()
{
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

    // Enforce a standard packing mode
    glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_FALSE );
    glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE );
    glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
    glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
}

void restorePixelStorage()
{
    glPopClientAttrib();
}

TextureInfo createCacheTexture(const Vector2i& aGlyphSize, size_t numGlyphsToCache)
{
    setPixelStorage();

    TextureInfo info;
    glGenTextures(1, &info.handle_);

    glBindTexture(GL_TEXTURE_2D, info.handle_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    info.size_ = cacheTextureSize(aGlyphSize, numGlyphsToCache);

    unsigned char* textureData = new unsigned char[info.size_.size()];
    memset(textureData, 0, info.size_.size());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, info.size_.width(), info.size_.height(), 0, GL_ALPHA,
                 GL_UNSIGNED_BYTE, textureData);

    restorePixelStorage();
    delete[] textureData;

    return info;
}

void blitGlyphToTexture(FT_Bitmap* aBitmap,const TextureInfo& aTextureInfo,const Vector2i& anOffset, const Vector2i& aGlyphSize)
{
    setPixelStorage();

    const int pitch = aBitmap->pitch;
    unsigned char* data = new unsigned char[aGlyphSize.size()];

    for(size_t y = 0; y < aGlyphSize.height(); ++y)
    {
        memcpy(data + (y * aGlyphSize.width()), aBitmap->buffer + (y * pitch), aGlyphSize.width());
    }

    glBindTexture(GL_TEXTURE_2D, aTextureInfo.handle_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, anOffset.x(), anOffset.y(),
        aGlyphSize.width(), aGlyphSize.height(), GL_ALPHA, GL_UNSIGNED_BYTE, data);

    delete[] data;

    restorePixelStorage();
}

struct DestroyTexture
{
    //! The function that does the destroying
    void operator() (const TextureInfo& info)
    {
        glDeleteTextures(1, &info.handle_);
    }
};

TypeFace::TypeFace(const std::string& filename, size_t aPointSize, size_t aResolution) :
    data_(new Data(aPointSize, aResolution))
{
    //ASSERT(FT_New_Face(FreeType::library(), aFaceName.c_str(), 0, &data_->face_) == 0);

    //ASSERT(FT_IS_SCALABLE(data_->face_));
    //ASSERT(FT_IS_SFNT(data_->face_));
    bool valid_ = false;
    data_->face_ = getFaceByFileName(filename,valid_);
    if(!valid_)
        return;

    if( !data_->face_->charmap) FT_Set_Charmap(data_->face_, data_->face_->charmaps[0]);

    //ASSERT(
    FT_Set_Char_Size(data_->face_, 0L, static_cast<FT_F26Dot6>(aPointSize << 6), aResolution, aResolution);
    // == 0);

    data_->hasKerning_ = (FT_HAS_KERNING(data_->face_) != 0);
}

TypeFace::~TypeFace()
{
    flushCache();
    FT_Done_Face(data_->face_);
    delete data_;
}

size_t TypeFace::pointSize() const
{
    return data_->pointSize_;
}

void TypeFace::setPointSize(size_t aPointSize)
{
    if(data_->pointSize_ != aPointSize)
    {
        data_->pointSize_ = aPointSize;
        const FT_F26Dot6 sz = static_cast<FT_F26Dot6>(aPointSize << 6);
        //ASSERT(
        FT_Set_Char_Size(data_->face_, sz, sz, data_->resolution_, data_->resolution_);
        // == 0);
        flushCache();
    }
}

size_t TypeFace::resolution() const
{
    return data_->resolution_;
}

void TypeFace::setResolution(size_t aResolution)
{
    if(data_->resolution_ != aResolution)
    {
        data_->resolution_ = aResolution;
        flushCache();
    }
}

float TypeFace::ascent() const
{
    return float(data_->face_->size->metrics.y_ppem) - descent();
}

float TypeFace::descent() const
{
    return static_cast<float>(-data_->face_->size->metrics.descender) * OneOver64;
}

float TypeFace::lineHeight() const
{
    return (ascent() + descent());
}

void TypeFace::flushCache()
{
    data_->characterCache_.clear();
    std::for_each(data_->textures_.begin(), data_->textures_.end(), DestroyTexture());
    data_->textures_.clear();
}

void TypeFace::addNewTexture(const Vector2i& aGlyphSize)
{
    const size_t numUncachedGlyphs = data_->face_->num_glyphs - data_->characterCache_.size();
    data_->textures_.push_back(createCacheTexture(aGlyphSize, numUncachedGlyphs));
    data_->offset_.x_ = glyphPadding;
    data_->offset_.y_ = glyphPadding;
}

void TypeFace::render(const String& aString, const Vector2f& aPosition)
{
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_HINT_BIT | GL_LINE_BIT | GL_PIXEL_MODE_BIT);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(aPosition.x(), aPosition.y(), 0.0f);

    renderGlyphs(aString);
    glPopMatrix();
    glPopAttrib();
}

float TypeFace::width(const String& aString)
{
    auto str = aString.get();
    auto it = str.begin();
    float ret = 0;
    size_t leftChar = 0;
    while(it != str.end())
    {
        size_t rightChar = FT_Get_Char_Index(data_->face_, *it);
        ret += kerning(leftChar, rightChar).x();
        ret += cachedGlyph(rightChar).advance_.x();
        leftChar = rightChar;
        ++it;
    }
    return ret;
}

size_t TypeFace::hitCharacterIndex(const String& aString, float anOffset)
{
    bool found = false;

    size_t characterIndex = 0;
    if(anOffset >= 0.0f)
    {
        auto str = aString.get();
        auto it = str.begin();
        float oldOffset = 0;
        size_t leftChar = 0;
        while(it != str.end())
        {
            float newOffset = oldOffset;
            size_t rightChar = FT_Get_Char_Index(data_->face_, *it);
            newOffset += kerning(leftChar, rightChar).x();
            newOffset += cachedGlyph(rightChar).advance_.x();

            if( (newOffset >= anOffset) && (oldOffset <= anOffset) )
            {
                found = true;
                break;
            }

            leftChar = rightChar;
            oldOffset = newOffset;
            ++characterIndex;
            ++it;
        }
    }

    if(!found)
        characterIndex = std::basic_string<uint32_t>::npos;

    return characterIndex;
}

void TypeFace::addCacheEntry(FT_GlyphRec_* aGlyph, FT_BitmapGlyphRec_* aBitmapGlyph, size_t aGlyphIndex, const Vector2i& aGlyphSize)
{
    const TextureInfo& textureInfo = *(data_->textures_.rbegin());

    const Vector2i bottomRight = data_->offset_ + Vector2i(aGlyphSize);

    const float widthF  = static_cast<float>(textureInfo.size_.width());
    const float heightF = static_cast<float>(textureInfo.size_.height());

    CacheEntry entry;
    entry.topLeftUV_.setX(static_cast<float>(data_->offset_.x()) / widthF);
    entry.topLeftUV_.setY(static_cast<float>(data_->offset_.y()) / heightF);
    entry.bottomRightUV_.setX(static_cast<float>(bottomRight.x()) / widthF);
    entry.bottomRightUV_.setY(static_cast<float>(bottomRight.y()) / heightF);
    entry.textureIndex_ = data_->textures_.size() - 1;
    entry.renderSize_ = aGlyphSize;
    entry.advance_ = Vector2f(float(aGlyph->advance.x >> 16), float(aGlyph->advance.y >> 16));
    entry.bitmapPosition_ = Vector2f(float(aBitmapGlyph->left), float(-aBitmapGlyph->top));

    data_->characterCache_[aGlyphIndex] = entry;
}

Vector2i TypeFace::maximumGlyphSize() const
{
    FT_BBox& bounds = data_->face_->bbox;
    float maxGlyphWidthF = static_cast<float>(bounds.xMax - bounds.xMin);
    maxGlyphWidthF *= ( (float)data_->face_->size->metrics.x_ppem / (float)data_->face_->units_per_EM);

    float maxGlyphHeightF = static_cast<float>(bounds.yMax - bounds.yMin);
    maxGlyphHeightF *= ( (float)data_->face_->size->metrics.y_ppem / (float)data_->face_->units_per_EM);

    return Vector2i(static_cast<size_t>(maxGlyphWidthF), static_cast<size_t>(maxGlyphHeightF));
}

void TypeFace::ensureTextureIsAvailable()
{
    const Vector2i maxGlyphSize = maximumGlyphSize();

    if(data_->textures_.size() == 0)
    {
        addNewTexture(maxGlyphSize);
    }
    else
    {
        const TextureInfo& textureInfo = *(data_->textures_.rbegin());
        if(data_->offset_.x() > int(textureInfo.size_.width()) - int(maxGlyphSize.width()) - int(glyphPadding))
        {
            data_->offset_.x_ = glyphPadding;
            data_->offset_.y_ =  data_->offset_.y() + maxGlyphSize.height();

            if(data_->offset_.y() > int(textureInfo.size_.height()) - int(maxGlyphSize.height()) - int(glyphPadding))
            {
                addNewTexture(maxGlyphSize);
            }
        }
    }
}

void TypeFace::cacheGlyph(size_t aGlyphIndex)
{
    ensureTextureIsAvailable();

    const FT_Error loadError = FT_Load_Glyph(data_->face_, aGlyphIndex, FT_LOAD_NO_HINTING);
    //ASSERT(loadError == 0);
    if(loadError == 0)
    {
        FT_Glyph glyph;
        const FT_Error getError = FT_Get_Glyph( data_->face_->glyph, &glyph);
        if(getError == 0)
        {
            const FT_Error bitmapError = FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, 0, 1);
            if( (bitmapError == 0) && (glyph->format == FT_GLYPH_FORMAT_BITMAP) )
            {
                FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
                FT_Bitmap* bitmap = &bitmapGlyph->bitmap;

                const Vector2i glyphSize(bitmap->width, bitmap->rows);

                if(glyphSize.size() > 0)
                    blitGlyphToTexture(bitmap, *(data_->textures_.rbegin()), data_->offset_, glyphSize);

                addCacheEntry(glyph, bitmapGlyph, aGlyphIndex, glyphSize);
                data_->offset_.setX(data_->offset_.x() + glyphSize.width() + glyphPadding);
            }
        }
        FT_Done_Glyph(glyph);
    }
}

CacheEntry& TypeFace::cachedGlyph(size_t aGlyphIndex)
{
    CharacterCache::iterator cacheIterator = data_->characterCache_.find(aGlyphIndex);
    if(cacheIterator == data_->characterCache_.end())
    {
        cacheGlyph(aGlyphIndex);
        cacheIterator = data_->characterCache_.find(aGlyphIndex);
        //ASSERT(cacheIterator != data_->characterCache_.end());
    }
    return cacheIterator->second;
}

Vector2f TypeFace::renderGlyph(size_t aGlyphIndex, const Vector2f& aPosition)
{
    CacheEntry& entry = cachedGlyph(aGlyphIndex);

    bindTexture(entry);

    const Vector2f position = aPosition + entry.bitmapPosition_;

    const Vector2f topLeftUV     = entry.topLeftUV_;
    const Vector2f bottomRightUV = entry.bottomRightUV_;
    const float glyphWidth  = static_cast<float>(entry.renderSize_.width());
    const float glyphHeight = static_cast<float>(entry.renderSize_.height());

    glBegin(GL_QUADS);
    {
        glTexCoord2f( topLeftUV.x(), topLeftUV.y() );
        glVertex2f( position.x(), position.y() );

        glTexCoord2f( bottomRightUV.x(), topLeftUV.y() );
        glVertex2f( position.x() + glyphWidth, position.y() );

        glTexCoord2f( bottomRightUV.x(), bottomRightUV.y() );
        glVertex2f( position.x() + glyphWidth, position.y() + glyphHeight );

        glTexCoord2f( topLeftUV.x(), bottomRightUV.y() );
        glVertex2f( position.x(), position.y() + glyphHeight );
    }
    glEnd();

    return entry.advance_;
}

Vector2f TypeFace::kerning(size_t leftGlyphIndex, size_t rightGlyphIndex) const
{
    Vector2f ret;
    if(data_->hasKerning_ && (leftGlyphIndex > 0) && (rightGlyphIndex > 0))
    {
        FT_Vector kerningVector;
        FT_Error ftError = FT_Get_Kerning(data_->face_, leftGlyphIndex, rightGlyphIndex, FT_KERNING_DEFAULT, &kerningVector);
        if(!ftError)
        {
            ret = Vector2f(static_cast<float>(kerningVector.x),
                                static_cast<float>(kerningVector.y)) * OneOver64;
        }
    }
    return ret;
}

void TypeFace::bindTexture(const CacheEntry& aCacheEntry) const
{
    //ASSERT(aCacheEntry.textureIndex_ < data_->textures_.size());
    const TextureInfo& info = data_->textures_[aCacheEntry.textureIndex_];

    GLint currentTextureID = -1;
    glGetIntegerv(GL_TEXTURE_2D_BINDING_EXT, &currentTextureID);
    if(static_cast<GLuint>(currentTextureID) != info.handle_)
    {
        glBindTexture(GL_TEXTURE_2D, info.handle_);
    }
}

void TypeFace::renderGlyphs(const String& aString)
{
    Vector2f pos(0.0f, 0.0f);
    size_t leftChar = 0;
    auto str = aString.get();
    auto it = str.begin();
    while(it != str.end())
    {
        size_t rightChar = FT_Get_Char_Index(data_->face_, *it);
        pos += kerning(leftChar, rightChar);
        pos += renderGlyph(rightChar, pos);
        leftChar = rightChar;
        ++it;
    }
}

}

}
