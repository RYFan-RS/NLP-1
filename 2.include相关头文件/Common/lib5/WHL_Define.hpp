#ifndef __WHL_DEFINE_H_
#define __WHL_DEFINE_H_

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h> 
#include <string>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#include "CommonDef.h"

// 64 bit string operation
#define FORMAT_64 "%ld"
#define FORMAT_u64 "%lu"

#define strtoval_64(x) strtol(x,NULL,10)
#define strtoval_u64(x) strtoul(x,NULL,10)
namespace whl{

// lock
class mutex_lock{
public:
	mutex_lock(){
		pthread_mutex_init(&m_lock, NULL);
	}
	
	~mutex_lock(){
		pthread_mutex_destroy(&m_lock);
	}
	
	inline void lock(){
		pthread_mutex_lock(&m_lock);
	}
	
	inline void unlock(){
		pthread_mutex_unlock(&m_lock);
	}
	
private:
	pthread_mutex_t m_lock;
};

class mutex_rwlock{
public:
	mutex_rwlock(){
		pthread_rwlock_init(&m_lock, NULL);
	}
	
	~mutex_rwlock(){
		pthread_rwlock_destroy(&m_lock);
	}
	
	inline void lock_rd(){
		pthread_rwlock_rdlock(&m_lock);
	}
	
	inline void lock_wr(){
		pthread_rwlock_wrlock(&m_lock);
	}
	
private:
	pthread_rwlock_t m_lock;
};

// thread
static int create_thread(void* (*func)(void*), void* param){
	pthread_t tid;
	if(pthread_create(&tid, 0, func, param)){
		return -1;
	}
	return 0;
}

static int get_threadid(){
	return pthread_self();
}

static int get_last_error(){
	return errno;
}

static void _sleep(const size_t milliseconds){
	usleep(milliseconds * 1000);
}

// string
static int str_replace(char* pStr, const char cSrc, const char cDst){
	if(!pStr){
		return -1;
	}
	for(;*pStr;){
		if(*pStr < 0){
			++pStr;
		}else if(*pStr == cSrc){
			*pStr = cDst;
		}else{
			
		}
		++pStr;
	}
	return 0;
}

static int get_date_str(char* pBuf, const size_t uBufLen, time_t tTime = 0, const char* pSeparate = "/"){
	if(!pBuf || uBufLen < 11 || !pSeparate){
		return -1;
	}
	
	if(tTime == 0){
		tTime = time(NULL);
	}
	
	struct tm local;
	localtime_r(&tTime, &local);
	
	return snprintf(pBuf, uBufLen, "%04d%s%02d%s%02d", local.tm_year+1900, pSeparate, local.tm_mon+1, pSeparate, local.tm_mday);
}

static int get_time_str(char* pBuf, const size_t uBufLen, time_t tTime = 0, const char* pSeparate = ":"){
	if(!pBuf || uBufLen < 9){
		return -1;
	}
	
	if(tTime == 0){
		tTime = time(NULL);
	}
	
	struct tm local;
	localtime_r(&tTime, &local);
	
	return snprintf(pBuf, uBufLen, "%02d%s%02d%s%02d", local.tm_hour, pSeparate, local.tm_min, pSeparate, local.tm_sec);
}

static int get_date_time_str(char* pStr, const size_t uBufLen, time_t tTime = 0){
	if (pStr == NULL || 50 > uBufLen){
        return -1;
    }
    if (tTime == 0){
        tTime = time(NULL);
    }
	char* pPos = pStr;
    int nLen = get_date_str(pStr, uBufLen,tTime);
	pPos += nLen;
    
	*pPos++ = '_';
    
	nLen = get_time_str(pPos, uBufLen-nLen-1,tTime);
	//pPos += nLen;
	
	return 0;
}

#ifndef FILENAME_MAX
#define FILENAME_MAX (256)
#endif 
static int mkdirs(const char* pPath){
	if(!pPath || (strlen(pPath) >= FILENAME_MAX)){
		return -1;
	}
	char arPath[FILENAME_MAX];
	strcpy(arPath, pPath);
	str_replace(arPath, '\\', '/');
	
	//"//*"->"/"
	char* pWrite = arPath;
	bool bContinus = false;
	for(pPath = arPath; *pPath;){
		if(*pPath == '/'){
			if(!bContinus){
				*pWrite++ = *pPath++;
				bContinus = true;
			}else{
				++pPath;
			}
		}else{
			bContinus = false;
			*pWrite++ = *pPath++;
		}
	}
	*pWrite = '\0';
	
	pPath = arPath;
	for(;;){
		char* pEnd = (char*)strchr(pPath + 1, '/');
		if(!pEnd){
			mkdir(arPath, S_IRWXO|S_IRWXU);
			return 0;
		}
		*pEnd = '\0';
		mkdir(arPath, S_IRWXO|S_IRWXU);
		*pEnd = '/';
		pPath = pEnd + 1;
	}
	
	return 0;
}

static int file_size(const char* file_name, size_t& _size){
    if (file_name == NULL)
    {
        return -1;
    } 
    
	struct stat file_info;
    int iRet = stat(file_name, &file_info);
    if (iRet != -1)
    {
        _size = file_info.st_size;
        return 0;
    }
	
    return -10;
}

static int copy_file(const char *pSrcFile, const char *pDestFile)
{
    if (pSrcFile == NULL || pDestFile == NULL)
    {
        return -1;
    }

	if (access(pSrcFile, 0) != 0)
    {
        return -1;
    }
	
    size_t uFileSize = 0u;
    int iRet = file_size(pSrcFile, uFileSize);
    if (iRet != 0)
    {//源大小获取失败
        return -20;
    }

    FILE *fpRB = fopen(pSrcFile, "rb");
    if (fpRB == NULL)
    {//源打不开
        return -30;
    }

    FILE *fpWB = fopen(pDestFile, "wb");
    if (fpWB == NULL)
    {//目标打不开
        fclose(fpRB);
        return -40;
    }

    iRet = 0;
    size_t uCopySize = 0;
    char szTempBuf[1<<10] = "";
    while (uCopySize < uFileSize)
    {
        int iCopyLen = fread(szTempBuf, 1, 1<<10, fpRB);
        if (iCopyLen <= 0)
        {//read fail
            iRet = -100;
            break;
        }

        if (fwrite(szTempBuf, iCopyLen, 1, fpWB) != 1)
        {
            iRet = -200;
            break;
        }

        uCopySize += iCopyLen;
    }

    fclose(fpRB);
    fclose(fpWB);

    return iRet;
}

}
#endif
