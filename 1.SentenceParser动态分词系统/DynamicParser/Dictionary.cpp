#include "Dictionary.h"

namespace aisms
{
namespace dypsr
{

bool Dictionary::init(const std::string& filename)
{
    acl::File file;

    if( file.read(filename) == false) {
        PRINT("Can't open dictionary file ["<<filename<<"].")
        return false;
    }
	
    for(size_t i=0; i<file.size(); i++) {
        if(file[i].empty()) continue;
		
        std::vector<int> vChar = makeCharFromStr(file.at(i));
		const int& nChar = vChar[0];
		//找对应的数字索引，比查String节约内存并且速度更快
		std::map<int,Node>::iterator iter = m_nChar_Node.find(nChar);
		if(iter != m_nChar_Node.end()){
			iter->second.addWord(vChar,0);
		}else{
			Node node;
			//递归添加子节点
			node.addWord(vChar,0);
			//记录当前索引与节点
			m_nChar_Node.insert(std::make_pair(nChar,node));
		}
    }
	
	/*std::vector<std::string> vWord = getWords();
	for(size_t i=0; i<vWord.size(); i++){
		P(vWord[i])
	}*/

    return true;
}

bool Dictionary::initWithEncryption(const std::string& filename)
{
	acl::FileEncoder fileEncoder;
    std::vector<std::string> file;

    if(fileEncoder.decodeFile(filename,file) == false){
		PRINT("Can't decode dictionary file ["<<filename<<"].");
		return false;
	}

    for(size_t i=0; i<file.size(); i++) {
        if(file[i].empty()) continue;
		
        std::vector<int> vChar = makeCharFromStr(file.at(i));
		const int& nChar = vChar[0];
		std::map<int,Node>::iterator iter = m_nChar_Node.find(nChar);
		if(iter != m_nChar_Node.end()){
			iter->second.addWord(vChar,0);
		}else{
			Node node;
			node.addWord(vChar,0);
			m_nChar_Node.insert(std::make_pair(nChar,node));
		}
    }

    return true;
}

bool Dictionary::init(const std::vector<std::string>& vWord)
{
    for(size_t i=0; i<vWord.size(); i++) {
        if(vWord[i].empty()) continue;

        std::vector<int> vChar = makeCharFromStr(vWord.at(i));
		const int& nChar = vChar[0];
		std::map<int,Node>::iterator iter = m_nChar_Node.find(nChar);
		if(iter != m_nChar_Node.end()){
			iter->second.addWord(vChar,0);
		}else{
			Node node;
			node.addWord(vChar,0);
			m_nChar_Node.insert(std::make_pair(nChar,node));
		}
    }

    return true;
}

Range Dictionary::find(const Result& result, size_t pos) const
{
	Range range;
	//用于记录在树的分支中曾出现的分词位置(最后取最长的一个分词)
	std::vector<int> vIndex;
	const std::string& sChar = result[pos];
	std::map<std::string,int>::const_iterator iter1 = m_sChar_nIdx.find(sChar);
	if(iter1 == m_sChar_nIdx.end()){
		return range;
	}
	
	//将当前的pos放入vIndex
	vIndex.push_back(pos);
	
	std::map<int,Node>::const_iterator iter = m_nChar_Node.find(iter1->second);
	if(iter == m_nChar_Node.end()){
		return range;
	}else{
		range.first = pos;
		range.second = iter->second.find(result,pos,vIndex,m_sChar_nIdx);
	}
	
    return range;
}

std::vector<int> Dictionary::makeCharFromStr(const acl::String& s)
{
    std::vector<std::string> vChar, v = s.toCharactor();
	std::map<std::string,int>::iterator iter;
	std::vector<int> vRet;
    std::string sChar;

    for(size_t i=0; i<v.size(); i++) {
        if(v.at(i).length() > 1 || (!acl::Utility::isAlphaNumber(v.at(i).at(0)) && v.at(i)!="-")){
            if(!sChar.empty()){
                vChar.push_back(sChar);
                sChar.erase(sChar.begin(),sChar.end());
            }
            vChar.push_back(v.at(i));
        }else{
			//英文将字符组装起来
            sChar += v.at(i);
        }
    }

    if(!sChar.empty()){
        vChar.push_back(sChar);
    }
	
	for(size_t i=0; i<vChar.size(); i++){
		iter = m_sChar_nIdx.find(vChar.at(i));
		if(iter == m_sChar_nIdx.end()){
			//一个单词对应一个数字(依次累加)
			m_sChar_nIdx.insert(std::make_pair(vChar.at(i),m_nLastIdx));
			//记录所有单词
			m_vChar.push_back(vChar.at(i));
			//记录所有索引
			vRet.push_back(m_nLastIdx++);
		}else{
			//如果m_sChar_nIdx已经有了,返回已经保存好的数字索引
			vRet.push_back(iter->second);
		}
	}
	
    return vRet;
}

std::vector<std::string> Dictionary::getWords() const{
	std::vector<std::string> vWord;
	std::vector<int> vChar;
	std::map<int,Node>::const_iterator iter;
	
	for(iter=m_nChar_Node.begin(); iter!=m_nChar_Node.end(); ++iter){
		vChar.push_back(iter->first);
		//m_vChar记录的是词汇表里的所有单词(非分词)
		iter->second.getWords(vWord,vChar,m_vChar);
		vChar.pop_back();
	}
	
	return vWord;
}

}//dypsr
}//namespace aisms
