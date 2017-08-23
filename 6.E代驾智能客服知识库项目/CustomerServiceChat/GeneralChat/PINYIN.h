#ifndef __ROBOSAY_GENERAL_PINYIN_H__
#define __ROBOSAY_GENERAL_PINYIN_H__

namespace robosay{
namespace general{

class PINYIN{
public:
	std::vector<std::string> Hanzi2Pinyin(const std::vector<std::string>& vWord, std::string format="default") const{
		std::vector<std::string> vWordPinyin;        // 全拼
		std::vector<std::string> vWordFirstLetter;   // 首字母
		std::vector<std::string> vWordOnset;         // 声母
		
		std::map<std::string, std::string>::const_iterator iter;
		
		for(size_t i=0; i<vWord.size(); i++){
			iter = m_hanzi_pinyin.find(vWord[i]);
			if(iter != m_hanzi_pinyin.end()){
				// 全拼
				vWordPinyin.push_back(iter->second);
				// 首字母
				vWordFirstLetter.push_back((iter->second).substr(0,1));
				// 声母
				std::string tmpOnset = (iter->second).substr(0,2);
				std::vector<std::string>::const_iterator it = m_chineseOnset.begin();
				for(; it!=m_chineseOnset.end(); it++){
					if((*it).find(tmpOnset) != -1){
						break;
					}
				}
				if(it != m_chineseOnset.end()){
					vWordOnset.push_back(tmpOnset);
				}else{
					vWordOnset.push_back((iter->second).substr(0,1));
				}
			}else{
				//非汉字字符则被保留
				vWordPinyin.push_back(vWord[i]);
				vWordFirstLetter.push_back(vWord[i]);
				vWordOnset.push_back(vWord[i]);
			}
		}
		
		if(format == "default"){           // 全拼输出
			return vWordPinyin;
		}
		if(format == "firstLetter"){       // 首字母组合输出
			return vWordFirstLetter;
		}
		if(format == "onset"){             // 声母组合输出
			return vWordOnset;
		}
	}
	

public:
	std::map<std::string, std::string> m_hanzi_pinyin; // 汉字、拼音对应字典
	std::vector<std::string> m_chineseOnset;           // 中文汉字声母集合

};

}//namespace general
}//namespace robosay

#endif //__ROBOSAY_GENERAL_TFIDF_H__