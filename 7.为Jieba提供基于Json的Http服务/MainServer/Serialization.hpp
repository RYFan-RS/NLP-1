#ifndef __SERIALIZATION_H__
#define __SERIALIZATION_H__
#include <iostream>
#include <fstream>
#include "serialize/objio.h"
#include "serialize/giop.h"
#include "serialize/graphio.h"

#define INDEX_CACHE_SIZE 32

class Serialization
{
public:
	template<class T>
	static bool writeData(T& t, const std::string& sBinDataFile){
		std::ofstream pout(sBinDataFile.c_str(),std::ios::out|std::ios::binary);
		if(!pout) return false;

		const int BUFFER_SIZE = 1024 * 1024 * 1024;
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

		return true;
	}
	
	template<class T>
	static bool readData(T& t, const std::string& sBinDataFile){
		std::fstream pin(sBinDataFile.c_str(),std::ios::in|std::ios::binary);
		if(!pin) return false;
		
		pin.seekg(std::ios::beg, std::ios::end);
		const int bufferSize = pin.tellg();
		pin.clear();
		pin.seekg(0, std::ios::beg);
		
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
};

#endif //__SERIALIZATION_H__

