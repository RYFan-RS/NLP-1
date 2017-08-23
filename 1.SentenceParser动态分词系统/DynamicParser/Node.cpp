#include "Node.h"

namespace aisms
{
namespace dypsr
{

void Node::addWord(const std::vector<int>& vChar, int pos){
	if(++pos == vChar.size()){
		//递归到最后一个节点,打上一个标记，表面到此处可以单独作为一个分词
		this->m_bEnd = true;
		return;
	}
	
	const int& nChar = vChar[pos];
	std::map<int,Node>::iterator iter = m_nChar_Node.find(nChar);
	if(iter != m_nChar_Node.end()){
		iter->second.addWord(vChar,pos);
	}else{
		Node node;
		node.addWord(vChar,pos);
		m_nChar_Node.insert(std::make_pair(nChar,node));
	}
}

void Node::getWords(std::vector<std::string>& vWord, std::vector<int>& vChar, const std::vector<std::string>& vCharDic) const{
	if(m_bEnd == true){
		//将多个单词组成一个分词结果
		vWord.push_back(getWord(vChar,vCharDic));
	}
	
	std::map<int,Node>::const_iterator iter;
	for(iter=m_nChar_Node.begin(); iter!=m_nChar_Node.end(); ++iter){
		//记录节点索引
		vChar.push_back(iter->first);
		iter->second.getWords(vWord,vChar,vCharDic);
		vChar.pop_back();
	}
}

bool Node::isEnd() const{
	return m_nChar_Node.empty() ? true : false;
}

std::string Node::getWord(const std::vector<int>& vChar, const std::vector<std::string>& vCharDic) const{
	std::string sWord;
	
	for(size_t i=0; i<vChar.size(); i++){
		//取出索引对应的单词
		sWord += vCharDic[vChar.at(i)];
	}
	
	return sWord;
}

}//dypsr
}//namespace aisms
