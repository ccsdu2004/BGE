#include <BGE/System/Err.h>
#include <streambuf>
#include <cstdio>
#include <iostream>

namespace priv
{

class DefaultErrStreamBuf : public std::streambuf
{
public :
    DefaultErrStreamBuf()
    {
        static const int size = 64;
        char* buffer = new char[size];
        setp(buffer, buffer + size);
    }

    ~DefaultErrStreamBuf()
    {
        sync();
        delete[] pbase();
    }
private :
    virtual int overflow(int character)
    {
        if((character != EOF) && (pptr() != epptr()))
        {
            // Valid character
            return sputc(static_cast<char>(character));
        }
        else if (character != EOF)
        {
            // Not enough space in the buffer: synchronize output and try again
            sync();
            return overflow(character);
        }
        else
        {
            // Invalid character: synchronize output
            return sync();
        }
    }

    virtual int sync()
    {
        // Check if there is something into the write buffer
        if (pbase() != pptr())
        {
            // Print the contents of the write buffer into the standard error output
            std::size_t size = static_cast<int>(pptr() - pbase());
            fwrite(pbase(), 1, size, stderr);

            // Reset the pointer position to the beginning of the write buffer
            setp(pbase(), epptr());
        }

        return 0;
    }
};

}

namespace bge
{

std::ostream& err()
{
    //static priv::DefaultErrStreamBuf buffer;
    //static std::ostream stream(&buffer);
    return std::cout;//stream;
}

}
