/*
  ATLAN C++ Library: SuperNode.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__SUPER_NODE_HPP__
#define __ACL__SUPER_NODE_HPP__
#include <vector>
#include <iostream>

namespace acl{

template<class Content>
class SuperNode{
public:
	SuperNode(){
		pnt = NULL;
	}
	
	~SuperNode(){
		clear();
	}
	
	const std::vector<SuperNode*>& getChd() const{
		return chd;
	}
	
	void setChd(const std::vector<SuperNode*>& nodes){
		chd = nodes;
	}
	
	void addChd(SuperNode* node){
		chd.push_back(node);
	}
	
	const SuperNode* getPnt() const{
		return pnt;
	}
	
	void setPnt(SuperNode* node){
		pnt = node;
	}
	
	void setContent(const Content& content){
		this->content = content;
	}
	
	void clear(){
		for(size_t i=0; i<chd.size(); i++){
			if(!chd[i]->getChd().empty()){
				chd[i]->clear();
			}
			delete chd[i];
			chd[i] = NULL;
		}
		
		pnt = NULL;
		chd.clear();
	}
	
	void dump(std::ostream& ofs = std::cout) const{
		ofs<<"Parent  :"<<(pnt == NULL ? "NULL" : pnt->content)<<std::endl;
		ofs<<"Content :"<<content<<std::endl;
		ofs<<"Children:"<<std::endl;
		for(size_t i=0; i<chd.size(); i++){
			ofs<<"\t"<<chd[i]->content<<std::endl;
		}
	}
	
	void dumpTree(std::ostream& ofs = std::cout, int depth = 0) const{
		ofs<<generateDepthHead(depth)<<content<<std::endl;
		for(size_t i=0; i<chd.size(); i++){
			chd[i]->dumpTree(ofs,++depth);
		}
	}
	
private:
	std::string generateDepthHead(const int& depth) const{
		if(depth == 0) return "";
		
		std::string head;
		
		for(size_t i=0; i<depth-1; i++){
			head += "| ";
		}
		head += "+-";
		
		return head;
	}
	
private:
	SuperNode* pnt;
	Content content;
	std::vector<SuperNode*> chd;
};

}//namespace acl

#endif //__ACL__SUPER_NODE_HPP__
