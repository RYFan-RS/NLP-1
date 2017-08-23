#ifndef __WHL_LOG_H_
#define __WHL_LOG_H_

#include "WHL_Define.hpp"
#include <stdarg.h>


#define RET_FALSE               (1)
#define RET_SUCCEED             (0)
#define RET_ERROR_INVALID_PARAM (-1)
#define RET_NO_ENOUGH_MEMORY    (-2)
#define CHECK_FAILED(x) 		(0 > x)

//#define PATH_2_NAME(x) strrchr(x, '\\') + 1
#define PATH_2_NAME(x) x

#define TRY(x, y)         				"\rThread is trying[%s] param[%s]\t\r", x, y
#define ERROR(x,y)        				"FILE: %s LINE: %d\nERROR: [%s] [%s] \n", PATH_2_NAME(__FILE__), __LINE__, x, y
#define WARNING(x)      				"FILE: %s LINE: %d\nWARNING: [%s] \n", PATH_2_NAME(__FILE__), __LINE__, x
#define NULL_POINTER(x) 				"FILE: %s LINE: %d\nERROR: [%s] is NULL pointer\n", PATH_2_NAME(__FILE__), __LINE__, x
#define INVALID_PARAMETER(x,y) 			"FILE: %s LINE: %d\nERROR: %s parameter is invalid, it's[[%s]]\n", PATH_2_NAME(__FILE__), __LINE__, x, y
#define VALID_PARAMETER(x,y) 			"FILE: %s LINE: %d\nERROR: %s parameter is valid, it's[[%s]]\n", PATH_2_NAME(__FILE__), __LINE__, x, y
#define FAILE_CALL(x,y) 				"FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s]\n",PATH_2_NAME(__FILE__), __LINE__, x, y
#define FAILE_CALL_PARAM(x,y,z) 		"FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], parameter is [%s]\n", PATH_2_NAME(__FILE__), __LINE__, x, y, z
#define FAILE_CALL_LEN_PARAM(x,y,l,z) 	"FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], parameter is [%.*s]\n", PATH_2_NAME(__FILE__), __LINE__, x, y, l, z
#define FAILE_CALL_RET(x,y,r) 			"FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], return [%d]\n", PATH_2_NAME(__FILE__), __LINE__, x, y, r
#define FAILE_CALL_ID(x,y,r) 			"FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], ID "CP_PU64"\n", PATH_2_NAME(__FILE__), __LINE__, x, y, r
#define PARAM(x,y,z) 					"FILE: %s LINE: %d\nERROR: in [%s], parameter [%s] is [%s]\n", PATH_2_NAME(__FILE__), __LINE__, x, y, z
#define FAILE_NEW(x) 					"FILE: %s LINE: %d\nERROR: failed to new [%s] object\n", PATH_2_NAME(__FILE__), __LINE__, x
#define FAILE_DUPSTR(x) 				"FILE: %s LINE: %d\nERROR: failed to duplicate [%s] string\n", PATH_2_NAME(__FILE__), __LINE__, x

#define PRINT_LOG
#ifdef PRINT_LOG
	#define PRINTF printf
	#define LOG_TRY(x, y)         printf("\rThread is trying[%s] param[%s]\t\r", x, y)
	#define LOG_ERROR(x,y)        printf("FILE: %s LINE: %d\nERROR: [%s] [%s] \n", PATH_2_NAME(__FILE__), __LINE__, x, y)
	#define LOG_WARNING(x)      printf("FILE: %s LINE: %d\nWARNING: [%s] \n", PATH_2_NAME(__FILE__), __LINE__, x)
	#define LOG_NULL_POINTER(x) printf("FILE: %s LINE: %d\nERROR: [%s] is NULL pointer\n", PATH_2_NAME(__FILE__), __LINE__, x)
	#define LOG_INVALID_PARAMETER(x,y) printf("FILE: %s LINE: %d\nERROR: %s parameter is invalid, it's[[%s]]\n", PATH_2_NAME(__FILE__), __LINE__, x, y)
	#define LOG_VALID_PARAMETER(x,y) printf("FILE: %s LINE: %d\nERROR: %s parameter is valid, it's[[%s]]\n", PATH_2_NAME(__FILE__), __LINE__, x, y)
	#define LOG_FAILE_CALL(x,y) printf("FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s]\n",PATH_2_NAME(__FILE__), __LINE__, x, y)
	#define LOG_FAILE_CALL_PARAM(x,y,z) printf("FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], parameter is [%s]\n", PATH_2_NAME(__FILE__), __LINE__, x, y, z)
	#define LOG_FAILE_CALL_LEN_PARAM(x,y,l,z) printf("FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], parameter is [%.*s]\n", PATH_2_NAME(__FILE__), __LINE__, x, y, l, z)
	#define LOG_FAILE_CALL_RET(x,y,r) printf("FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], return [%d]\n", PATH_2_NAME(__FILE__), __LINE__, x, y, r)
	#define LOG_FAILE_CALL_ID(x,y,r) printf("FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], ID "CP_PU64"\n", PATH_2_NAME(__FILE__), __LINE__, x, y, r)
	#define LOG_PARAM(x,y,z) printf("FILE: %s LINE: %d\nERROR: in [%s], parameter [%s] is [%s]\n", PATH_2_NAME(__FILE__), __LINE__, x, y, z)
	#define LOG_FAILE_NEW(x) printf("FILE: %s LINE: %d\nERROR: failed to new [%s] object\n", PATH_2_NAME(__FILE__), __LINE__, x)
	#define LOG_FAILE_DUPSTR(x) printf("FILE: %s LINE: %d\nERROR: failed to duplicate [%s] string\n", PATH_2_NAME(__FILE__), __LINE__, x)
#else
	#define PRINTF 
	#define LOG_TRY(x, y)
	#define LOG_ERROR(x,y)
	#define LOG_WARNING(x)
	#define LOG_NULL_POINTER(x)
	#define LOG_INVALID_PARAMETER(x,y)
	#define LOG_VALID_PARAMETER(x,y)
	#define LOG_FAILE_CALL(x,y)
	#define LOG_FAILE_CALL_PARAM(x,y,z)
	#define LOG_FAILE_CALL_LEN_PARAM(x,y,l,z)
	#define LOG_FAILE_CALL_RET(x,y,r) 
	#define LOG_FAILE_CALL_ID(x,y,r) 
	#define LOG_PARAM(x,y,z) 
	#define LOG_FAILE_NEW(x) 
	#define LOG_FAILE_DUPSTR(x) 
#endif

namespace whl{

//每天更换文件的日志
class daily_log
{
public:
    daily_log()
	{
        memset(m_log_dir, 0, sizeof(m_log_dir));
        memset(m_prefix, 0, sizeof(m_prefix));
        m_need_time_stamp = true;
        m_flush_freq = 1;
        m_save_period = 30;
        m_handle = NULL;
        m_save_count = 0;
        m_last_open_day = 0;
        m_last_open_time = 0;
    }

    //FUNC  初始化
    //PARAM log_dir：日志目录
    //      prefix：日志前缀
    //      need_time_stamp：是否需要时间戳
    //      FlushFreq：刷新频率，若为3，表示每写三次就强制刷新文件
    //      SavePeriod：保存周期，若为7，表示只保存最近7日内的日志文件
    //RET   0：初始化成功
    //      其他：初始化失败，返回错误码
    int init(const char* log_dir, const char* prefix, bool need_time_stamp = true, size_t flush_freq = 1, size_t save_period = 30)
    {
		if(!log_dir || access(log_dir, 0))
        {//日志目录不存在
            return -1;
        }

        strcpy(m_log_dir, log_dir);
        strcpy(m_prefix, prefix);
        m_need_time_stamp = need_time_stamp;
        m_flush_freq = flush_freq;
        m_save_period = save_period;

        return 0;
    }
    //FUNC  记录日志的printf版本
    //PARAM show_console：是否显示在控制台上
    void print(bool show_console, const char* format, ...)
    {
        char file_name[FILENAME_MAX] = "";
        char time_stamp[64] = "";
        if (m_need_time_stamp)
        {
            whl::get_date_time_str(time_stamp, 64);
        }
        m_lock.lock();

        time_t now = time(NULL);
		struct tm local;
		localtime_r(&now, &local);
        if (NULL == m_handle)
        {
            sprintf(file_name, "%s/%s_%04d%02d%02d.txt", 
                m_log_dir, m_prefix,
                local.tm_year+1900, local.tm_mon+1, local.tm_mday);
            do
            {
                m_handle = fopen(file_name, "a");
            }
            while (NULL == m_handle);
            m_last_open_time = time(NULL); 
            m_last_open_day = local.tm_mday;
        }
        assert(m_handle != NULL);


        if (m_need_time_stamp)
        {
            fprintf(m_handle, "[%s]", time_stamp);
        }
		va_list ap;
        va_start(ap, format);
        vfprintf(m_handle, format, ap);
        va_end(ap);

        ++m_save_count;
        try_flush_data();

        if (show_console == true)
        {
            if (m_need_time_stamp)
            {
                ::printf("[%s]", time_stamp);
            }
            va_start(ap, format);
            vprintf(format, ap);
            va_end(ap);
        }
		m_lock.unlock();
    }
    //FUNC  强制刷新日志文件
    void flush()
    {
        if (m_handle != NULL)
        {
            fflush(m_handle);
        }
    }
	
protected:
    //FUNC  尝试刷新数据
    void try_flush_data()
    {
        time_t now = time(NULL);	
        if (m_last_open_time+10*60 < now || m_save_count >= m_flush_freq)
        {//打开文件超过十分钟 || 保存次数已经超过刷新周期
            
            fflush(m_handle);      //刷新
            m_save_count = 0;    //保存次数清零
            
            //判断是否需要重新打开一个文件
			struct tm local;
			localtime_r(&now, &local);
            if (m_last_open_time+10*60 < now || local.tm_mday != m_last_open_day)
            {//打开文件超过十分钟 || 已经跨天
                
                fclose(m_handle);
                m_handle = NULL;
                                
                char file_name[FILENAME_MAX] = "";
                sprintf(file_name, "%s/%s_%04d%02d%02d.txt", 
                    m_log_dir, m_prefix,
                    local.tm_year+1900, local.tm_mon+1, local.tm_mday);				
                do 
                {
                    m_handle = fopen(file_name, "a");
                }
                while (m_handle == NULL);
                m_last_open_time = now;
                m_last_open_day = local.tm_mday;

                clean_old_log();  //每打开一个新文件，就清理一次旧日志
            }
            assert(m_handle != NULL);
        }
    }
	
    //FUNC  清理旧的日志文件
    void clean_old_log(){
        char file_name[FILENAME_MAX] = "";
        time_t now = time(NULL);
        now -= m_save_period*86400;
        for (size_t i = 0; i < m_save_period; ++i, now -= 86400){
			struct tm local;
			localtime_r(&now, &local);
            sprintf(file_name, "%s/%s_%04d%02d%02d.txt",
                m_log_dir, m_prefix,
                local.tm_year+1900, local.tm_mon+1, local.tm_mday);
            remove(file_name);
        }
    }
	
private:
    char m_log_dir[FILENAME_MAX];  		//日志目录
    char m_prefix[FILENAME_MAX];  		//日志前缀
    bool m_need_time_stamp;          	//是否需要时间戳
    size_t m_flush_freq;             	//刷新频率
    size_t m_save_period;            	//保存周期
    whl::mutex_lock m_lock;         	//写文件的锁
    FILE* m_handle;                    	//文件追加指针
    size_t m_save_count;             	//保存次数
    time_t m_last_open_day;          	//上次打开文件的时间，以天为单位
    time_t m_last_open_time;         	//上次打开文件的时间，以秒为单位
};


//尝试记录日志
class try_log
{
public:
    try_log()
    {
        memset(m_log_dir, 0, sizeof(m_log_dir));
        memset(m_prefix, 0, sizeof(m_prefix));
        m_need_time_stamp = true;
        m_max_size = 100<<20;
    }
	
	//FUNC  初始化
    //PARAM log_dir: 日志路径
	//		prefix：日志前缀
    //      need_time_stamp：是否需要时间戳
    //      max_size：日志文件的最大大小，若日志文件超过此大小，则会重命名当前文件，并使用一个新文件
    //RET   0：初始化成功
    //      其他：初始化失败
    int init(const char *log_dir, const char *prefix, bool need_time_stamp = true, size_t max_size = 100<<20)
    {
        if (log_dir == NULL || prefix == NULL)
        {
            return -1;
        }

        strcpy(m_log_dir, log_dir);
        return init(prefix, need_time_stamp, max_size);
    }
	
	void print(bool show_console, const char *format, ...)
    {
		if(!m_log_dir || access(m_log_dir, 0))
        {//日志目录不存在
            return;
        }

        char time_stamp[64] = "";
        if (m_need_time_stamp)
        {
            whl::get_date_time_str(time_stamp, 64);
        }

        char file_name[FILENAME_MAX] = "";
        sprintf(file_name, "%s/%s.txt", m_log_dir, m_prefix);

		m_lock.lock();
        //记日志
        FILE *handle = fopen(file_name, "a");
        if (NULL != handle)
        {
            if (m_need_time_stamp)
            {
                fprintf(handle, "[%s]", time_stamp);
            }
			va_list ap;
			va_start(ap, format);
			vfprintf(handle, format, ap);
			va_end(ap);
		
            fclose(handle);
        }

        //屏幕输出
        if (show_console == true)
        {
            if (m_need_time_stamp)
            {
                printf("[%s]", time_stamp);
            }
			va_list ap;
			va_start(ap, format);
            vprintf(format, ap);
            va_end(ap);
        }

        //日志文件足够大，就重命名
        size_t _size = 0;
        int iRet = whl::file_size(file_name, _size);
        if (iRet == 0 && _size >= m_max_size)
        {
            char dest_file[FILENAME_MAX] = "";
            int now = time(NULL);
            sprintf(dest_file, "%s.%d", file_name, now);
            rename(file_name, dest_file);
        }
		m_lock.unlock();
    }

private:
    //FUNC  初始化
    //PARAM prefix：日志前缀
    //      need_time_stamp：是否需要时间戳
    //      max_size：日志文件的最大大小，若日志文件超过此大小，则会重命名当前文件，并使用一个新文件
    //RET   0：初始化成功
    //      其他：初始化失败
    int init(const char *prefix, bool need_time_stamp = true, size_t max_size = 100<<20)
    {
        if (prefix == NULL)
        {
            return -1;
        }

        strcpy(m_prefix, prefix);
        m_need_time_stamp = need_time_stamp;
        m_max_size = m_max_size;

        return 0;
    }
	/*void printf_kernel(const char *log_dir, bool show_console, const char *format, va_list ap)
    {
		if(!log_dir || access(log_dir, 0))
        {//日志目录不存在
            return;
        }

        char time_stamp[64] = "";
        if (m_need_time_stamp)
        {
            whl::get_date_time_str(time_stamp, 64);
        }

        char file_name[FILENAME_MAX] = "";
        sprintf(file_name, "%s/%s.txt", log_dir, m_prefix);

		m_lock.lock();

        //记日志
        FILE *handle = fopen(file_name, "a");
        if (NULL != handle)
        {
            if (m_need_time_stamp)
            {
                fprintf(handle, "[%s]", time_stamp);
            }
            vfprintf(handle, format, ap);

            fclose(handle);
        }

        //屏幕输出
        if (show_console == true)
        {
            if (m_need_time_stamp)
            {
                printf("[%s]", time_stamp);
            }
            vprintf(format, ap);
        }

        //日志文件足够大，就重命名
        size_t _size = 0;
        int iRet = whl::file_size(file_name, _size);
        if (iRet == 0 && _size >= m_max_size)
        {
            char dest_file[FILENAME_MAX] = "";
            int now = time(NULL);
            sprintf(dest_file, "%s.%d", file_name, now);
            rename(file_name, dest_file);
        }
		m_lock.unlock();
    }*/

private:
    char m_log_dir[FILENAME_MAX];  		//日志目录
    char m_prefix[FILENAME_MAX];  		//日志前缀
    bool m_need_time_stamp;          	//是否需要时间戳
    size_t m_max_size;            		//日志文件的最大长度
    whl::mutex_lock m_lock;         	//日志锁
};

}

#endif
