#ifndef __WHL_TRAVERSE_DIR_H_
#define __WHL_TRAVERSE_DIR_H_

//#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#include "WHL_Define.hpp"

namespace whl{

#define MAX_PATH_LEN (256)

class dir_browser
{
protected:
	char m_base_dir[MAX_PATH_LEN];
	
public:
	dir_browser()
		: m_base_dir(){

	}

	virtual ~dir_browser(){
		
	}
	
	// _dir must be absolute directory
	bool set_init_dir(const std::string& _dir){
		if(_dir.empty() || _dir.length() >= MAX_PATH_LEN){
			return false;
		}
		strcpy(m_base_dir, _dir.c_str());
		
		int ret = chdir(m_base_dir);
		if(ret){
			return false;
		}
		
		int len = strlen(m_base_dir);
		if(1 < len){
			if('\\' != m_base_dir[len - 1]){
				m_base_dir[len++] = '\\';
			}
		}
	
		return true;
	}
	
	// 开始遍历m_base_dir下由file_spec指定类型的文件
	// file_spec 可以使用通配符，如："D:\\*.txt"
	// 返回false，表示遍历终止
	bool browse(const char* file_spec){
		process_dir(m_base_dir, NULL);
		return browse_dir(m_base_dir, file_spec);
	}
	
protected:
	// 遍历目录dir下由filespec指定的文件
	// 对于子目录,采用迭代的方法
	// 返回false,表示遍历终止
	bool browse_dir(const char* dir, const char* file_spec){
		chdir(dir);
		
		char cur_path[MAX_PATH_LEN];
		
		// 查找dir中符合要求的文件
		long file_handle = 0L;
		/*finddata_t file_info;
		file_handle = findfirst(file_spec, &file_info);
		if(file_handle != -1){
			for(;;){
				// 不是文件夹
				if(!(file_handle.attrib & _A_SUBDIR)){
					snprintf(cur_path, MAX_PATH_LEN, "%s/%s", dir, fileinfo.name);
					if(!process_file(cur_path)){
						_findclose(file_handle);
						return false;
					}
				}
				
				int ret = _findnext(file_handle, &file_info);
				if(!ret){
					break;
				}
			}
			_findclose(file_handle);
		}
		
		//查找dir中的子目录
		_chdir(dir);
		memset(&file_info, 0, sizeof(_finddata_t));
		file_handle = _findfirst("*.*", &file_info);
		if(file_handle != -1){
			for(;;){
				// 是文件夹
				if((file_handle.attrib & _A_SUBDIR)){
					snprintf(cur_path, MAX_PATH_LEN, "%s/%s/", dir, fileinfo.name);
					if(!process_dir(cur_path, dir)){
						_findclose(file_handle);
						return false;
					}
					if(!browse_dir(cur_path, file_spec)){
						_findclose(file_handle);
						return false;
					}
				}
				
				int ret = _findnext(file_handle, &file_info);
				if(!ret){
					break;
				}
			}
			_findclose(file_handle);
		}
		*/
		
		return true;
	}

	//函数browse_dir每找到一个文件,就调用process_file
	//并把文件名作为参数传递过去
	//如果返回false,表示中止遍历文件
	//用户可以覆写该函数,加入自己的处理代码
	virtual bool process_file(const char* file_name){;}

	//函数browse_dir每进入一个目录,就调用process_dir
	//并把正在处理的目录名及上一级目录名作为参数传递过去
	//如果正在处理的是初始目录,则parent_dir=NULL
	//用户可以覆写该函数,加入自己的处理代码
	//比如用户可以在这里统计子目录的个数
	virtual void process_dir(const char* cur_dir, const char* parent_dir){;}
};


// 递归遍历文件夹下所有的文件
/*class deep_list_file : public dir_browser{
public:
	deep_list_file()
		: dir_browser()
		, m_files(){

	}
	
	virtual ~deep_list_file(){}
	
	virtual bool process_file(const char* file_name){
		m_files.push_back(file_name);
	}
	
	const std::vector<std::string>& get_files() const{
		return m_files;
	}
	
private:
	std::vector<std::string> m_files;
};*/

class deep_list_file {

public:
	deep_list_file()
		: m_files(){

	}
	
	~deep_list_file(){}
	
	bool set_init_dir(const std::string& _dir){
		strcpy(m_base_dir, _dir.c_str());
		return true;
	}
	
	bool browse(const char* file_spec){
		m_files.clear();
		return browse_dir(m_base_dir, file_spec);
	}
	
	
	const std::vector<std::string>& get_files() const{
		return m_files;
	}

private:
	bool browse_dir(const char* dir, const char* file_spec){
		DIR *pdir = NULL;
		dirent *pdirent = NULL;
		std::string file("");
		
		if((pdir = ::opendir(dir)) != NULL){
			while((pdirent = readdir(pdir)) != NULL){
				file = pdirent->d_name;
				if(file!="." && file!=".."){
					std::string path = std::string(dir) + "/" + file;
					if(is_dir(path)){
						browse_dir(path.c_str(), file_spec);
					}else if(is_spec_file(path, file_spec)){
						m_files.push_back(path);
					}else {
						;
					}
				}
			}
			::closedir(pdir);
		}
	
		return true;
	}

	bool is_dir(const std::string& file){
		struct stat statbuf;		
		if(::stat(file.c_str(), &statbuf)== 0){
			if(S_ISDIR(statbuf.st_mode) != 0)
				return true;
		}
		return false;
	}
	
	bool is_spec_file(const std::string& path, const char* spec_file){
		if(!strcmp(spec_file, "*.xml")){
			if(path.length() > 4){
				if(path.substr(path.length() - 4) == ".xml"){
					return true;
				}
			}
		}else if(!strcmp(spec_file, "*.*")){
			return true;
		}else{
			;
		}
		return false;
	}
	
private:
	char 						m_base_dir[MAX_PATH_LEN];
	std::vector<std::string> 	m_files;
};

}

#endif