#ifndef __AISMS__DYPSR__NODE__H__
#define __AISMS__DYPSR__NODE__H__
#include "Result.h"

namespace aisms
{
namespace dypsr
{

class Node
{
public:
	bool m_bEnd; //记录到当前节点是否可以作为一个分词
	std::map<int,Node> m_nChar_Node;//记录当前节点的下级分支
	
public:
	Node():m_bEnd(false){}
	void addWord(const std::vector<int>& vChar, int pos);
	int find(const Result& result, int pos, std::vector<int>& vIndex, const std::map<std::string,int>& mCharIdx) const{
		//如果已经到达分支的底端，那么这肯定是一个分词单元
		if(m_nChar_Node.empty()){
			return pos;
		}
		
		//如果中途出现了满足条件的,需要记录下来
		if(this->m_bEnd == true){
			vIndex.push_back(pos);
		}
		
		//说明到达本次搜索内容的底端
		if( ++pos == result.size()) {
			//返回最后一个，也就是最长路径
			return vIndex.back();
		}
			
		const std::string& sChar = result[pos];
		std::map<std::string,int>::const_iterator iter1 = mCharIdx.find(sChar);
		if(iter1 == mCharIdx.end()){
			//没有出现过这个词，返回当前最长的位置
			return vIndex.back();
		}
		//mCharIdx是针对所有的单词的，m_nChar_Node是针对本节点的
		std::map<int,Node>::const_iterator iter = m_nChar_Node.find(iter1->second);
		if(iter != m_nChar_Node.end()){
			//去子节点下继续递归找
			return iter->second.find(result,pos,vIndex,mCharIdx);
		}else{
			return vIndex.back();
		}
	}
	void getWords(std::vector<std::string>& vWord, std::vector<int>& vChar, const std::vector<std::string>& vCharDic) const;
	bool isEnd() const;
	
private:
	std::string getWord(const std::vector<int>& vChar, const std::vector<std::string>& vCharDic) const;
};

}//dypsr
}//namespace aisms

#endif //__AISMS__DYPSR__NODE__H__
