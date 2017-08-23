#ifndef __JSON_HPP__
#define __JSON_HPP__
#include "reader.h"
#include "writer.h"
#include "elements.h"
#include "../lib4/Definations.hpp"

class Json{
public:
	static std::string toString(const json::UnknownElement& element){
		std::stringstream stream;
		
		json::Writer::Write(element, stream);
		std::string s = stream.str();
		
		if(!s.empty() && s[0] =='"'){
			s = s.substr(1,s.length()-2);
		}
		
		std::string sRet;
		for(size_t i=0; i<s.length(); i++){
			if(s[i]!='\r' && s[i]!='\n' && s[i]!='\t'){
				sRet += s[i];
			}
		}
		
		return sRet;
	}
	
	static bool read(const std::string& s, json::Object& obj){
		try{
			if(s.empty()){
				return false;
			}
			std::stringstream stream(s);
			json::Reader::Read(obj, stream);
		} catch (...) {
			PRINT("json read string ["<<s<<"] meets error!")
			return false;
		}
		
		return true;
	}

	static std::string get(const std::string& s, json::Object& obj){
		return Json::toString(obj[s]);
	}
	
	static std::vector<std::string> getArray(json::Object& oRoot, const std::string& s){
		const json::Array& array = oRoot[s];
		std::vector<std::string> vRet;
		json::Array::const_iterator iter(array.Begin());
		
		for (; iter != array.End(); ++iter){
			vRet.push_back(Json::toString(*iter));
		}
		
		return vRet;
	}
};

#endif //__JSON_HPP__
