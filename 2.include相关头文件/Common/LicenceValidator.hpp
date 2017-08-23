#ifndef __RBS__LICENCE_VALIDATOR_HPP__
#define __RBS__LICENCE_VALIDATOR_HPP__
#include<stdio.h>
#include<stdlib.h>
#include "Common/lib4/Popen.hpp"
#include "Common/lib4/File.hpp"
#include "Common/lib4/MD5.hpp"

class LicenceValidator{
public:
	bool validLicence(std::string fileName = std::string()){
		std::string sCpuKeyInfo = "";
		std::string sEthernetKeyInfo = "";
		std::string sOSKeyInfo = "";
		std::string sRandKeyInfo = "";
		
		acl::Line sCpuInfo;
		acl::Popen::execute("cat /proc/cpuinfo",sCpuInfo);
		std::vector<std::string> vCpuInfo = sCpuInfo.separate("\n");
		sCpuKeyInfo = getCpuKeyInfo(vCpuInfo);
		
		acl::Line sEthernetInfo;
		acl::Popen::execute("dmesg",sEthernetInfo);
		std::vector<std::string> vEthernetinfo = sEthernetInfo.separate("\n");
		sEthernetKeyInfo = getEthernetKeyInfo(vEthernetinfo);
		
		acl::Line sOSInfo;
		acl::Popen::execute("cat /etc/issue",sOSInfo);
		std::vector<std::string> vOSInfo = sOSInfo.separate("\n");
		sOSKeyInfo = getOSKeyInfo(vOSInfo);
		
		acl::File file;
		if(fileName.empty()){
			fileName = "rbs_licence.dat";
		}
		if(file.read(fileName) == false) {
			std::cout<<"Can't read file "<<fileName<<" error!"<<std::endl;
			return false;
		}
		acl::String sLicence = file[0];
		if(sLicence.length() != 100){
			std::cout<<"Invalid lincence file 'rbs_licence.dat'!"<<std::endl;
			return false;
		}
		sRandKeyInfo = sLicence.substr(0,2) + sLicence.substr(sLicence.length()-2);
		
#ifdef _DEBUG
		std::cout<<"sCpuKeyInfo:"<<sCpuKeyInfo<<std::endl;
		std::cout<<"sEthernetKeyInfo:"<<sEthernetKeyInfo<<std::endl;
		std::cout<<"sOSKeyInfo:"<<sOSKeyInfo<<std::endl;
		std::cout<<"sRandKeyInfo:"<<sRandKeyInfo<<std::endl;
#endif //_DEBUG
		
		sCpuKeyInfo += sRandKeyInfo;
		sEthernetKeyInfo += sRandKeyInfo;
		sOSKeyInfo += sRandKeyInfo;
		
		acl::MD5 md5;
		acl::String sCpuCipherInfo(md5.MD5String(sCpuKeyInfo.c_str()));
		acl::String sEthernetCipherInfo(md5.MD5String(sEthernetKeyInfo.c_str()));
		acl::String sOSCipherInfo(md5.MD5String(sOSKeyInfo.c_str()));
		
		acl::String sCipherInfo = sRandKeyInfo.substr(0,2) + sCpuCipherInfo + sEthernetCipherInfo + sOSCipherInfo + sRandKeyInfo.substr(2);
		
		return sCipherInfo == sLicence ? true : false;
	}
	
private:
	std::string genRandomInfo(){
		srand((int)time(0));
		int randNum = rand()%10000;
		acl::String sRandNum(randNum);
		return sRandNum;
	}

	std::string getOSKeyInfo(const std::vector<std::string>& vFile){
		std::string sKeyInfo = "";
		
		for(int i=0; i<vFile.size(); i++){
			const acl::Line& line = vFile[i];
			int pos = line.find(" ");
			if(pos > 0){
				sKeyInfo = line.substr(0,pos);
			}
		}
		
		return sKeyInfo;
	}

	std::string tryToGetEthInfo(const std::vector<std::string>& vFile, const std::string& sIdx){
		std::string sKeyInfo = "";
		
		for(int i=0; i<vFile.size(); i++){
			const acl::Line& line = vFile[i];
			int pos = line.find("eth"+sIdx+": (PCI:");
			if(pos >= 0){
				int pos1 = line.rfind(" ");
				if(pos1 > 0){
					sKeyInfo = line.substr(pos1+1);
				}
				break;
			}
		}
		
		return sKeyInfo;
	}

	std::string getEthernetKeyInfo(const std::vector<std::string>& vFile){
		std::string sEthKeyInfo0 = tryToGetEthInfo(vFile,"0");exit;
		std::string sEthKeyInfo1 = tryToGetEthInfo(vFile,"1");
		std::string sEthKeyInfo2 = tryToGetEthInfo(vFile,"2");
		std::string sEthKeyInfo3 = tryToGetEthInfo(vFile,"3");
		
		std::set<std::string> seKey;
		if(!sEthKeyInfo0.empty()){
			seKey.insert(sEthKeyInfo0);
		}
		if(!sEthKeyInfo1.empty()){
			seKey.insert(sEthKeyInfo1);
		}
		if(!sEthKeyInfo2.empty()){
			seKey.insert(sEthKeyInfo2);
		}
		if(!sEthKeyInfo3.empty()){
			seKey.insert(sEthKeyInfo3);
		}
		
		std::string sKeyInfo = "";
		
		for(std::set<std::string>::const_iterator iter=seKey.begin(); iter!=seKey.end(); ++iter){
			sKeyInfo += *iter;
		}
		
		return sKeyInfo;
	}

	std::string getCpuKeyInfo(const std::vector<std::string>& vFile){
		std::string sKeyInfo = "";
		
		for(int i=0; i<vFile.size(); i++){
			const acl::Line& line = vFile[i];
			int pos = line.find("model name	: ");
			if(pos >= 0){
				sKeyInfo += line.substr(pos+13);
			}
		}
		
		return sKeyInfo;
	}
};

#endif //__RBS__LICENCE_VALIDATOR_HPP__
