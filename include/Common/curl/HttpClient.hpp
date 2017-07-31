#ifndef __HTTP_CURL_H__
#define __HTTP_CURL_H__
#include <string>
#include <iostream>
#include <curl/curl.h>

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	if(itype == CURLINFO_TEXT)
	{
		//printf("[TEXT]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_IN)
	{
		printf("[HEADER_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_OUT)
	{
		printf("[HEADER_OUT]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_IN)
	{
		printf("[DATA_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_OUT)
	{
		printf("[DATA_OUT]%s\n", pData);
	}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if( NULL == str || NULL == buffer )
	{
		return -1;
	}

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
	
	return nmemb;
}

class CHttpClient
{
public:
	/**
	* @brief HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse) const
	{
		CURLcode res;
		CURL* curl = curl_easy_init();
		if(NULL == curl)
		{
			return CURLE_FAILED_INIT;
		}
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return res;
	}

	/**
	* @brief HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Get(const std::string & strUrl, std::string & strResponse, const int nTmOut = 3) const
	{
		CURLcode res;
		CURL* curl = curl_easy_init();
		if(NULL == curl)
		{
			return CURLE_FAILED_INIT;
		}
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
		/**
		* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
		* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
		*/
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, nTmOut);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return res;
	}
	
	/**
	* @brief HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int GetHeader(const std::string & strUrl, std::string & strResponse) const
	{
		CURLcode res;
		CURL* curl = curl_easy_init();
		if(NULL == curl)
		{
			return CURLE_FAILED_INIT;
		}
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		/**
		* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
		* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
		*/
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return res;
	}
	
	/**
	* @brief HTTPS POST请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL) const
	{
		CURLcode res;
		CURL* curl = curl_easy_init();
		if(NULL == curl)
		{
			return CURLE_FAILED_INIT;
		}
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		if(NULL == pCaPath)
		{
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		}
		else
		{
			//缺省情况就是PEM，所以无需设置，另外支持DER
			//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
			curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
		}
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return res;
	}


	/**
	* @brief HTTPS GET请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL) const
	{
		CURLcode res;
		CURL* curl = curl_easy_init();
		if(NULL == curl)
		{
			return CURLE_FAILED_INIT;
		}
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		if(NULL == pCaPath)
		{
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		}
		else
		{
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
			curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
		}
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return res;
	}
	
	unsigned char ToHex(unsigned char x) const
	{ 
		return  x > 9 ? x + 55 : x + 48; 
	}

	unsigned char FromHex(unsigned char x) const 
	{ 
		unsigned char y;
		if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
		else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
		else if (x >= '0' && x <= '9') y = x - '0';
		else {printf("invalid char");};
		return y;
	}

	std::string UrlEncode(const std::string& str) const
	{
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (isalnum((unsigned char)str[i]) || 
				(str[i] == '-') ||
				(str[i] == '_') || 
				(str[i] == '.') || 
				(str[i] == '~'))
				strTemp += str[i];
			else if (str[i] == ' ')
				strTemp += "+";
			else
			{
				strTemp += '%';
				strTemp += ToHex((unsigned char)str[i] >> 4);
				strTemp += ToHex((unsigned char)str[i] % 16);
			}
		}
		return strTemp;
	}

	std::string UrlDecode(const std::string& str) const
	{
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (str[i] == '+') strTemp += ' ';
			else if (str[i] == '%')
			{
				unsigned char high = FromHex((unsigned char)str[++i]);
				unsigned char low = FromHex((unsigned char)str[++i]);
				strTemp += high*16 + low;
			}
			else strTemp += str[i];
		}
		return strTemp;
	}
};

#endif
