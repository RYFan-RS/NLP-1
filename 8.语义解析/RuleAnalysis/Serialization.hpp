#ifndef __SAE_SERIALIZATION_H__
#define __SAE_SERIALIZATION_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "LineIndexManager.h"
#include "AllocationTable.hpp"

namespace sae
{

#define INDEX_CACHE_SIZE 32

class Serialization
{
public:
    Serialization() {
        m_firstTimeFlag = true;
    }
    Serialization(const std::string& fileName) {
        m_fileName = fileName;
        m_firstTimeFlag = true;
    }

    void setFileName(const std::string& fileName) {
        m_fileName = fileName;
    }

    template<class T>
    bool writeModule(T& t, const std::string& moduleName) {
        AllocationRange range;

        if(m_firstTimeFlag) {
            if( !writeAllocationRange(t,range,true) ) {
                return false;
            }
            m_firstTimeFlag = false;
        } else {
            if( !writeAllocationRange(t,range,false) ) {
                return false;
            }
        }

        m_AllocationTable.add(moduleName,range);

        return true;
    }

    template<class T>
    bool readModule(T& t, const std::string& moduleName) const {
        AllocationRange range = m_AllocationTable.find(moduleName);

        if( range == m_AllocationTable.NullVal ) {
            return false;
        }

        if( !readAllocationRange(t,range) ) {
            return false;
        }

        return true;
    }

    bool writeAllocationTable() const {
        AllocationRange range;

        if( !writeAllocationRange(m_AllocationTable,range) ) {
            return false;
        }

        if( !writeAllocationTableAllocationRange(range) ) {
            return false;
        }

        return true;
    }

    bool readAllocationTable() {
        AllocationRange range;

        if( !readAllocationTableAllocationRange(range) ) {
            return false;
        }

        if( !readAllocationRange(m_AllocationTable,range) ) {
            return false;
        }

        return true;
    }
private:
    int getFileSize() const {
        std::fstream pin(m_fileName.c_str(),std::ios::in|std::ios::binary);
        if(!pin) return false;

        pin.seekg(std::ios::beg, std::ios::end);
        int fileSize = pin.tellg();
        pin.close();

        return fileSize;
    }

    template<class T>
    bool writeAllocationRange(T& t, AllocationRange& range, bool clearFlag = false) const {
        if(clearFlag) {
            std::ofstream pclear(m_fileName.c_str(),std::ios::out|std::ios::binary);
            if(!pclear) return false;
            pclear.close();
            range.beg = 0;
        } else {
            range.beg = getFileSize();
        }

        std::ofstream pout(m_fileName.c_str(),std::ios::app|std::ios::binary);
        if(!pout) return false;

        const int BUFFER_SIZE = 20 * 1024 * 1024;
        char *buffer = new char[BUFFER_SIZE];
        if(buffer == NULL) return false;
        memset(buffer,'\0',BUFFER_SIZE);

        mem_buffer mb(buffer, BUFFER_SIZE);
        GIOP_format<mem_buffer> gfmb(mb);
        obj_output<GIOP_format<mem_buffer>, graph_refs> objout(gfmb);
        objout.content(t);

        pout.write(buffer, mb.size());

        delete[] buffer;
        buffer = NULL;
        pout.close();

        range.end = getFileSize();

        return true;
    }

    template<class T>
    bool readAllocationRange(T& t, const AllocationRange& range) const {
        std::fstream pin(m_fileName.c_str(),std::ios::in|std::ios::binary);
        if(!pin) return false;

        pin.seekg(range.beg);

        const int bufferSize = range.end - range.beg;
        char *buffer = new char[ bufferSize ];
        if(buffer == NULL) return false;

        pin.read(buffer, bufferSize);
        pin.close();

        mem_buffer mb(buffer, bufferSize);
        GIOP_format<mem_buffer> gf(mb);
        obj_input< GIOP_format<mem_buffer>, graph_refs > objin(gf);

        objin.content(t);

        delete[] buffer;
        buffer = NULL;

        return true;
    }

    bool writeAllocationTableAllocationRange(const AllocationRange& range) const {
        std::ofstream pout(m_fileName.c_str(),std::ios::app|std::ios::binary);
        if(!pout) return false;

        char* buffer = new char[ INDEX_CACHE_SIZE ];
#ifdef WIN32
        sprintf_s(buffer, INDEX_CACHE_SIZE, "%d", range.end - range.beg);
#else
		sprintf(buffer, "%d", range.end - range.beg);
#endif
        pout.write(buffer,INDEX_CACHE_SIZE);
        pout.close();

        delete[] buffer;
        buffer = NULL;

        return true;
    }

    bool readAllocationTableAllocationRange(AllocationRange& range) const {
        std::fstream pin(m_fileName.c_str(),std::ios::in|std::ios::binary);
        if(!pin) return false;

        pin.seekg(std::ios::beg, std::ios::end);
        const int bufferSize  = pin.tellg();

        pin.seekg(bufferSize - INDEX_CACHE_SIZE);

        char *buffer = new char[ INDEX_CACHE_SIZE ];
        if(buffer == NULL) return false;

        pin.read(buffer, INDEX_CACHE_SIZE);
        pin.close();

        const int allocationTableSize = atoi(buffer);

        delete[] buffer;
        buffer = NULL;

        range.beg = bufferSize - INDEX_CACHE_SIZE - allocationTableSize;
        range.end = bufferSize - INDEX_CACHE_SIZE;

        return true;
    }

private:
    AllocationTable m_AllocationTable;
    std::string m_fileName;
    bool m_firstTimeFlag;
};

}//namespace sae

#endif //__MEAN_SERIALIZATION_H__

