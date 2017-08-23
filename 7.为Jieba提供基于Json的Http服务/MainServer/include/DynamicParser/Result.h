#ifndef __AISMS__DYPSR__RESULT__H__
#define __AISMS__DYPSR__RESULT__H__
#include <vector>
#include <map>
#include "lib4/String.hpp"
#include "lib4/Utility.hpp"
#include "Range.h"

namespace aisms
{
namespace dypsr
{

class Result : public std::vector<std::string>
{
public:
	Result(){}
    Result(const acl::String& s) {
        std::vector<std::string> v = makeCharFromStr(s);
/*        std::string sWord;

        for(size_t i=0; i<v.size(); i++) {
            if(v.at(i).length() > 1 || (v.at(i) != " " && !acl::Utility::isAlphaNumber(v.at(i).at(0)) && v.at(i)!="-")){
                if(!sWord.empty()){
                    this->push_back(sWord);
                    sWord.erase(sWord.begin(),sWord.end());
                }
                this->push_back(v.at(i));
            }else{
                sWord += v.at(i);
            }
        }

        if(!sWord.empty()){
            this->push_back(sWord);
        }*/

		this->swap(v);
    }

    void add(const Range& range);
    std::string getInput() const;
    std::string toStr() const;
	std::vector<std::string> makeCharFromStr(const acl::String& s) const;
	void readWords(const std::vector<std::string>& vWord);
    std::vector<std::string> getWords() const;
	std::map<size_t,Range> getRanges() const;
	void setRanges(const std::map<size_t,Range>& mRange);
private:
    std::map<size_t,Range> m_mRange;
};

}//dypsr
}//namespace aisms

#endif //__AISMS__DYPSR__RESULT__H__
