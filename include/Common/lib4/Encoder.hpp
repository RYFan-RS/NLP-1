/*
  ATLAN C++ Library: Encoder.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__ENCODER_HPP__
#define __ACL__ENCODER_HPP__
#include <iterator>
#include "File.hpp"

namespace acl{

class Encoder{
public:
    std::string static encode(const std::string& sIn)
    {
        const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string sRet,s=rotate(sIn);
        unsigned char Tmp[4]={0};
		const size_t uSLen = s.length();
        size_t uLength=0,uIdx=0;
	
        for(size_t i=0;i<uSLen/3;i++){
            Tmp[1] = s.at(uIdx++);
            Tmp[2] = s.at(uIdx++);
            Tmp[3] = s.at(uIdx++);
            sRet+= EncodeTable[Tmp[1] >> 2];
            sRet+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
            sRet+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
            sRet+= EncodeTable[Tmp[3] & 0x3F];
            if(uLength+=4,uLength==76) {sRet+="\r\n";uLength=0;}
        }
    
        size_t uMod = uSLen%3;
        if(uMod==1){
            Tmp[1] = s.at(uIdx++);
            sRet+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
            sRet+= EncodeTable[((Tmp[1] & 0x03) << 4)];
            sRet+= "#=";
        }else if(uMod==2){
            Tmp[1] = s.at(uIdx++);
            Tmp[2] = s.at(uIdx++);
            sRet+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
            sRet+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
            sRet+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
            sRet+= "=";
        }
    
        return sRet;
    }

    std::string static decode(const std::string& s)
    {
        const char DecodeTable[] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            62, // '+'
            0, 0, 0,
            63, // '/'
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
            0, 0, 0, 0, 0, 0, 0,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
            13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
            0, 0, 0, 0, 0, 0,
            26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
        };
        std::string sRet;
        int nValue;
	    const size_t nSLen = s.length();
	    size_t i=0,idx=0;
	    while(i < nSLen){
                if (s.at(idx) != '\r' && s.at(idx) !='\n'){
		    nValue = DecodeTable[s.at(idx++)] << 18;
		    nValue += DecodeTable[s.at(idx++)] << 12;
		    sRet+=(nValue & 0x00FF0000) >> 16;
                    if (s.at(idx) != '#'){
			    nValue += DecodeTable[s.at(idx++)] << 6;
			    sRet+=(nValue & 0x0000FF00) >> 8;
			    if (s.at(idx) != '='){
				    nValue += DecodeTable[s.at(idx++)];
				    sRet+=nValue & 0x000000FF;
			    }
		    }
		    i+=4;
                }else{
                    i++;
                    idx++;
                }
        }

        return rotate(sRet);
    }

private:
    static std::string rotate(const std::string& sIn){
	std::string s(sIn);
        if(s.length() < 3) return s;
        
        const size_t uStepLen = s.length() / 3;
        size_t uIdx = 0;
        for(size_t i=0; i<uStepLen; i++){
            char cache = s.at(uIdx);
            s.at(uIdx) = s.at(uIdx+2);
            s.at(uIdx+2) = cache;
            uIdx += 3;
        }

        return s;
    }
};

}//namespace acl

#endif //__ACL__ENCODER_HPP__
