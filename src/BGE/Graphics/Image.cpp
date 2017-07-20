#include <BGE/Graphics/Image.h>
#include <BGE/GUI/WindowManager.h>

namespace bge
{

Image::~Image()
{
    if(handle() != 0)
        WindowManager::instance().canvas()->unregisterImage(handle());
}

}
