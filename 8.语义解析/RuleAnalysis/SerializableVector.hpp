#ifndef __SAE__SERIALIZABLE_VECTOR_H__
#define __SAE__SERIALIZABLE_VECTOR_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <fstream>
#include <vector>
#include "objio.h"
#include "giop.h"
#include "graphio.h"

namespace sae
{

// -> SVector
template <class T>
class SerializableVector : public std::vector<T>
{
public:
    SerializableVector() {};
    SerializableVector(const T& t) {
        this->push_back(t);
    }
    SerializableVector(const std::vector<int>& v) {
        for(size_t i=0; i<v.size(); i++) {
            this->push_back(v[i]);
        }
    }
    virtual ~SerializableVector() {}

    void operator = (const SerializableVector<T>& v) {
        this->clear();
        for(size_t i=0; i<v.size(); i++) {
            this->push_back(v[i]);
        }
    }

    bool operator == (const SerializableVector<T>& s) const {
        if(this->size()!=s.size()) return false;
        for(size_t i=0; i<this->size(); i++) {
            if((*this)[i] != s[i]) {
                return false;
            }
        }
        return true;
    }

    void operator += (const SerializableVector<T>& v) {
        for(size_t i=0; i<v.size(); i++) {
            this->push_back(v[i]);
        }
    }

    bool find(const T& t) const {
        for(size_t i=0; i<this->size(); i++) {
            if((*this)[i] == t) {
                return true;
            }
        }
        return false;
    }

    bool find(const SerializableVector<T>& v) const {
        for(size_t i=0; i<v.size(); i++) {
            if(this->find(v[i]) == false) {
                return false;
            }
        }
        return true;
    }

    void remove(const T& t) {
        for(typename std::vector<T>::iterator i = this->begin(); i != this->end(); ++i) {
            if(*i == t) {
                this->erase(i);
                --i;
            }
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        size_t i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            T t;
            stream.content(t);
            this->push_back(t);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        size_t i,size = this->size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.content((T)(*this)[i]);
        }
        format.output_end_array();
    }
};

#define SVector SerializableVector

}//namespace sae


#endif //__SAE__SERIALIZABLE_VECTOR_H__
