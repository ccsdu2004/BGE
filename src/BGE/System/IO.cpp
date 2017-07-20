#include <BGE/System/IO.h>
#include <BGE/System/Err.h>
#include <BGE/System/Sigslot.h>
#include <physfs.h>
#include <list>
#include <cstdio>

using namespace std;

namespace bge
{

IOWriter::IOWriter()
{
}

IOWriter::~IOWriter()
{
}

IOReader::IOReader()
{
}

IOReader::~IOReader()
{
}

struct SignalHolder
{
    Signal1<Deleter*> signal_;
};

class PhysfsWriter : public IOWriter, public SignalHolder
{
public:
    PhysfsWriter(PHYSFS_file* file,const std::string &filename):
        file_(file),
        filename_(filename)
    {
    }

    ~PhysfsWriter()
    {
        if(file_ != NULL)
            PHYSFS_close(file_);
    }
public:
    uint32_t write(const void* buffer,uint32_t size,uint32_t number)
    {
        if(!isValid())
            return false;
        return PHYSFS_write(file_,buffer,size,number);
    }

    int32_t seek(int32_t offset,int32_t mode)
    {
        uint32_t filesize = PHYSFS_fileLength(file_);
        int32_t pos = 0;

        if(mode == SEEK_SET)
            pos = offset;
        else if(mode == SEEK_CUR)
        {
            int32_t current = PHYSFS_tell(file_);
            pos = current + offset;
            if(pos > filesize)
                return 0;
        }
        else if(mode == SEEK_END)
            pos = filesize - offset;

        return PHYSFS_seek(file_,pos) == 0;
    }

    uint32_t tell()const
    {
        return PHYSFS_tell(file_);
    }

    std::string fileName()const
    {
        return filename_;
    }

    bool isValid()const
	{
	    return file_ != NULL;
	}

	void close()
	{
	    if(isValid())
	        PHYSFS_close(file_);
        file_ = 0;
	}

	void flush()
	{
	    if(isValid())
            PHYSFS_flush(file_);
	}

	void deleteLater()
	{
	    signal_.emit(this);
	}
private:
	PHYSFS_file*  file_;
	std::string   filename_;
};

class PhysfsReader: public IOReader, public SignalHolder
{
public:
    PhysfsReader(PHYSFS_file* file,const std::string &filename):
        file_(file),
        fileSize_(0),
        fileName_(filename)
    {
        fileSize_ = PHYSFS_fileLength(file_);
    }

    ~PhysfsReader()
    {
        close();
    }

    uint32_t read(void* buffer,uint32_t size,uint32_t number)
    {
        return PHYSFS_read(file_,buffer,size,number);
    }

    uint32_t size()const
    {
        return fileSize_;
    }

    int32_t seek(int32_t offset,int32_t mode)
    {
        if(!isValid())
            return 0;

        if(offset > fileSize_)
            return 0;

        int32_t pos(0);

        if(mode == SEEK_SET)
            pos = offset;
        else if(mode == SEEK_CUR)
        {
            int32_t current = PHYSFS_tell(file_);
            pos = current+offset;

            if(pos> fileSize_)
                return 0;
        }
        else if(mode == SEEK_END)
        {
            pos = fileSize_ - offset;
        }

        return PHYSFS_seek(file_,pos) != 0;
    }

    uint32_t tell() const
    {
        return PHYSFS_tell(file_);
    }

    void close()
    {
        if(file_ != NULL)
            PHYSFS_close(file_);
        file_ = 0;
    }

    std::string fileName()const
    {
        return fileName_;
    }

    bool isValid()const
	{
	    return file_ != NULL;
	}

	void deleteLater()
	{
	    signal_.emit(this);
	}
private:
	PHYSFS_file* file_;
	int32_t      fileSize_;
	std::string  fileName_;
};

class LocalReader: public IOReader, public SignalHolder
{
public:
    LocalReader(const std::string& filename):
        fileName_(filename)
    {
        file_ = fopen(filename.c_str(),"rb");
    }

    ~LocalReader()
    {
        if(file_)
           fclose(file_);
    }
public:
    uint32_t read(void* buffer,uint32_t size,uint32_t number)
    {
        return fread(buffer,size,number,file_);
    }

    uint32_t size()const
    {
        if(isValid() == false)
           return 0;

        uint32_t pos = ftell(file_);
        fseek(file_,0,SEEK_END);
        uint32_t size = ftell(file_);
        fseek(file_,0,SEEK_SET);
        return size;
    }

    int32_t seek(int32_t offset,int32_t mode)
    {
        return fseek(file_,offset,mode);
    }

    uint32_t tell()const
    {
       return ftell(file_);
    }

    string fileName()const
    {
        return fileName_;
    }

    bool isValid()const
    {
        return file_ != NULL;
    }

    void close()
    {
        if(file_)
            fclose(file_);
        file_ = 0;
    }

	void deleteLater()
	{
	    signal_.emit(this);
	}
private:
    FILE* file_;
    string fileName_;
};

class LocalWriter : public IOWriter, public SignalHolder
{
public:
    LocalWriter(const string& filename):
        fileName_(filename)
    {
        file_ = fopen(filename.c_str(),"wb");
    }

    ~LocalWriter()
    {
        if(file_)
            fclose(file_);
    }
public:
    uint32_t write(const void* buffer,uint32_t size,uint32_t number)
    {
        return fwrite(buffer,size,number,file_);
    }

    int32_t  seek(int32_t position,int32_t mode)
    {
        return fseek(file_,position,mode);
    }

    uint32_t tell()const
    {
        return ftell(file_);
    }

    string fileName()const
    {
        return fileName_;
    }

    bool isValid()const
    {
        return file_ != 0;
    }

    void flush()
    {
        if(isValid())
            fflush(file_);
    }

    void close()
    {
        if(isValid())
            fclose(file_);
        file_ = 0;
    }

	void deleteLater()
	{
	    signal_.emit(this);
	}
private:
    FILE* file_;
	string fileName_;
};

class FileSystemImpl : public FileSystem, public SlotHolder
{
public:
    ~FileSystemImpl()
    {
    }
public:
    void initial(const char* arg)
    {
        PHYSFS_init(arg);
    }

    bool setResourcePackage(const string& package)
    {
        bool flag = PHYSFS_addToSearchPath(package.c_str(),1);
        if(!flag)
            err() << "set resource package :" << package <<" failed"<<std::endl;
        return flag;
    }

    string workDir() const
    {
        return PHYSFS_getBaseDir();
    }

    string userDir() const
    {
         return PHYSFS_getUserDir();
    }

    IOReader* readFile(const string& filename)
    {
        if(filename.size() > 0 && filename[0] == ':')
        {
            string file = filename.substr(1);
            PHYSFS_File* ptr = PHYSFS_openRead(file.c_str());
            if(!ptr)
            {
                err() << "bad read file name:" << filename << "\n";
                return 0;
            }

            PhysfsReader* reader = new PhysfsReader(ptr,file);
            if(reader)
                reader->signal_.connect(this,&FileSystemImpl::recycle);
            return reader;
        }

        LocalReader* reader = new LocalReader(filename);
        if(reader)
            reader->signal_.connect(this,&FileSystemImpl::recycle);
        return reader;
    }

    IOWriter* writeFile(const string& filename)
    {
        if(filename.size() > 0 && filename[0] == ':')
        {
            string file = filename.substr(1);
            PHYSFS_File* ptr = PHYSFS_openWrite(file.c_str());
            if(!ptr)
            {
                err() << "bad write file name:" << filename << "\n";
                return 0;
            }

            PhysfsWriter* writer = new PhysfsWriter(ptr,file);
            if(writer)
                writer->signal_.connect(this,&FileSystemImpl::recycle);
            return writer;
        }

        LocalWriter* writer = new LocalWriter(filename);
        if(writer)
            writer->signal_.connect(this,&FileSystemImpl::recycle);
        return writer;
    }

    void recycle(Deleter* deleter)
    {
        data_.remove(deleter);
        delete deleter;
		deleter = 0;
    }

    void deleteLater()
    {
        auto itr = data_.begin();
        while(itr != data_.end())
        {
            (*itr)->deleteLater();
			(*itr) = 0;
            itr ++;
        }

        PHYSFS_deinit();
    }
private:
    std::list<Deleter*> data_;
};

FileSystem& FileSystem::instance()
{
    static FileSystemImpl impl;
    return impl;
}

}
