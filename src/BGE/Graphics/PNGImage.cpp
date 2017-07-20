#include <BGE/System/IO.h>
#include <BGE/System/Err.h>
#include <BGE/Graphics/PNGImage.h>
#include <png.h>
#include <cstdio>
#include <map>
#include <string.h>

namespace bge
{

namespace
{

//! The number of bytes used to check whether a file is actually a PNG file
const unsigned int numBytesForPNGCheck = 4;

bool isValidPNGFile(FILE *fp)
{
    png_byte buf[numBytesForPNGCheck];

    for(size_t i = 0; i < numBytesForPNGCheck; i++)
    {
        if(!(buf[i] = fgetc(fp)))
            return false; // ### RETURN ###
    }
    return (!png_sig_cmp(buf, 0, numBytesForPNGCheck));
}

std::map<std::string,PNGImage*> imageMap;

} // anonymous namespace

Image* Image::create(const std::string& fileName)
{
    auto imageIterator = imageMap.find(fileName);
    if(imageIterator == imageMap.end())
    {
        imageMap[fileName] = new PNGImage(fileName);
    }

    imageIterator = imageMap.find(fileName);
    //ASSERT(imageIterator != imageMap.end());

    return imageIterator->second;
}

#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

/*void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    void* read_io_ptr = png_get_io_ptr(png_ptr) ;
    fread( (unsigned char*) data, length, 1, (FILE*) read_io_ptr ) ;
}*/

void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    void* read_io_ptr = png_get_io_ptr(png_ptr) ;
    bge::IOReader* reader = (IOReader*)read_io_ptr;
    if(reader)
        reader->read(data,length,1);
    //fread( (unsigned char*) data, length, 1, (FILE*) read_io_ptr ) ;
}

PNGImage::PNGImage(const std::string& fileName):
    Image(fileName),
    data_(0)
{
    IOReader* reader = FileSystem::instance().readFile(fileName);
    if(reader == 0)
        return;

    /*if(!isValidPNGFile(fp))
    {
        fclose(fp);
    }*/

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if(png_ptr == 0)
    {
        reader->close();
        reader->deleteLater();
		reader = NULL;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == 0)
    {
        png_destroy_read_struct(&png_ptr, png_infopp(0), png_infopp(0));
        reader->close();
        reader->deleteLater();
		reader = NULL;
    }

    if(setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, png_infopp(0), png_infopp(0));
        reader->close();
        reader->deleteLater();
        reader = NULL;
    }

    png_set_read_fn( png_ptr, reader,(png_rw_ptr)user_read_data ) ;

    //png_init_io(png_ptr,reader);
    //png_set_read_fn(png_ptr,reader,);
    png_set_sig_bytes(png_ptr,0);//numBytesForPNGCheck);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);
    const unsigned int width = png_get_image_width(png_ptr, info_ptr);
    const unsigned int height = png_get_image_height(png_ptr, info_ptr);
    const unsigned int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    if(bit_depth != 8)
    {
        reader->close();
        reader->deleteLater();
        png_destroy_info_struct(png_ptr, png_infopp(&info_ptr));
        png_destroy_read_struct(&png_ptr, png_infopp(0), png_infopp(0));
		err() << fileName << " the bit depth must be 8\n";
    }
    const png_byte colorType = png_get_color_type(png_ptr, info_ptr);
    if((colorType != PNG_COLOR_TYPE_RGB) && (colorType != PNG_COLOR_TYPE_RGB_ALPHA))
    {
        reader->close();
        reader->deleteLater();
		reader = NULL;
        png_destroy_info_struct(png_ptr, png_infopp(&info_ptr));
        png_destroy_read_struct(&png_ptr, png_infopp(0), png_infopp(0));
		err() << fileName << " only GRB and RGBA are supported.\n";
    }

    const int bytesPerPixel = (colorType == PNG_COLOR_TYPE_RGB) ? 3 : 4;
    const int stride = bytesPerPixel * width;

    unsigned char* data = new unsigned char[stride * height];
    png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
    for(unsigned int i = 0; i < height; ++i)
    {
        const unsigned int row = height - i - 1;
        memcpy(data + (row * stride), row_pointers[i], stride);
    }

    size_.x_ = width;
    size_.y_ = height;
    numberOfBitsPerPixel_ = bytesPerPixel * 8;
    data_ = data;

    //fclose(fp);
    reader->close();
    reader->deleteLater();
    png_destroy_info_struct(png_ptr, png_infopp(&info_ptr));
    png_destroy_read_struct(&png_ptr, png_infopp(0), png_infopp(0));
}

PNGImage::~PNGImage()
{

}

void PNGImage::deleteLater()
{
    imageMap.erase(imageMap.find(fileName()));
    if(data_)
        delete[] data_;
}

}

