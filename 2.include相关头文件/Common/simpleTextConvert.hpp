#ifndef __SIMPLE__TEXT_CONVERT_HPP__
#define __SIMPLE__TEXT_CONVERT_HPP__
#include <iconv.h> 
#include <opencc/opencc.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <memory.h> 
#include <string>
#include <iostream>
namespace robosay{
	class simpleTextConvert{
public:
	virtual ~simpleTextConvert(){
		if(m_cd!=NULL){
			iconv_close(m_cd);
			m_cd = NULL;
		}
		if(m_od!=NULL){
			opencc_close(m_od);
			m_od = NULL;
		}
	}
	simpleTextConvert(const std::string& destCharset,const std::string& srcCharset){
		m_srcCharset = srcCharset;
		m_destCharset= destCharset;
		m_cd = iconv_open(destCharset.c_str(),srcCharset.c_str()); 
		m_od = opencc_open(OPENCC_DEFAULT_CONFIG_TRAD_TO_SIMP);
	}


	///繁体转化为简体，编码从m_srcCharset转换为m_destCharset，输入是src，输出是dest 
	bool cht2chs(const std::string& src, std::string& dest) 
	{ 
		char utf8[1000+100];
		const char*raw= src.c_str(); 
		memset(utf8,0,sizeof(utf8));
		if(src.empty() || m_od==NULL){
			return false;
		} 
		
		if(src.size()>1000){
			std::cout<<"error:the string is too length"<<std::endl;
			return false;
		}
		
		if(m_srcCharset=="utf8"){
			memcpy(utf8,raw,strlen(raw));
		}
		else{
			if(!m_iconv(m_srcCharset.c_str(), "utf8", raw, strlen(raw), utf8, sizeof(utf8))){    //先转为utf8
				return false;
			}
		}			
		
		char * chsutf8 = opencc_convert_utf8(m_od, utf8, strlen(utf8));    //简繁转换 
		if(chsutf8==NULL  || 0==strlen(chsutf8)){
			return false;
		}

		memset(utf8,0,sizeof(utf8));
		if(m_destCharset=="utf8"){
			strncpy(utf8,chsutf8,sizeof(utf8)-3);//再转到目标编码cpy//再转到目标编码 
		}
		else{
			m_iconv("utf8", m_destCharset.c_str(), chsutf8, strlen(chsutf8), utf8, sizeof(utf8)-3);    //再转到目标编码 
		}
		
		free(chsutf8);    //一定要free不然会内存泄漏 
		dest = std::string(utf8);
		return true;
	} 
	/*
	///简体转化为繁体，编码从src_charset转换为dest_charset，输入是src，输出是dest 
	void chs2cht(const char* src_charset, const char * dest_charset, 
		const char *src, char *dest) 
	{ 
		char utf8[1000], tempout[1000], *raw=(char *)src; 
		m_iconv(src_charset, "utf8", raw, strlen(raw), utf8, sizeof(utf8));    //先转为utf8 
		opencc_t od = opencc_open(OPENCC_DEFAULT_CONFIG_SIMP_TO_TRAD); 
		char * chsutf8 = opencc_convert_utf8(od, utf8, strlen(utf8));    //简繁转换 
		strcpy(tempout, chsutf8); 
		free(chsutf8);    //一定要free不然会内存泄漏 
		opencc_close(od); 
		size_t len = strlen(tempout); 
		m_iconv("utf8", dest_charset, tempout, len, dest, len*2+1);    //再转到目标编码 
	} 
	*/
	protected:
		///使用iconv库将字符串从编码srcCharset转换到destCharset，输入是inbuf，输出是outbuf 
		bool m_iconv(const char *from_charset, const char * to_charset, 
				const char *inbuf, size_t inlen, char *outbuf, size_t outlen) 
		{ 
			memset(outbuf,0,outlen); 
			if (m_cd==NULL) return false; 
			iconv(m_cd, (char **)&inbuf, &inlen, &outbuf, &outlen); 
			return true;
		} 
	private:
		iconv_t  m_cd;
		opencc_t m_od;
		std::string m_srcCharset;
		std::string m_destCharset;
};
}
#endif