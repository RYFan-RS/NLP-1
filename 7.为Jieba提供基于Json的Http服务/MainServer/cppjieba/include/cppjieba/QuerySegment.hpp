#ifndef CPPJIEBA_QUERYSEGMENT_H
#define CPPJIEBA_QUERYSEGMENT_H

#include <algorithm>
#include <set>
#include <cassert>
#include "limonp/Logging.hpp"
#include "DictTrie.hpp"
#include "SegmentBase.hpp"
#include "FullSegment.hpp"
#include "MixSegment.hpp"
#include "Unicode.hpp"
#include "DictTrie.hpp"

namespace cppjieba {
class QuerySegment: public SegmentBase {
 public:
  QuerySegment(const string& dict, const string& model, const string& userDict = "")
    : mixSeg_(dict, model, userDict),
      trie_(mixSeg_.GetDictTrie()) {
  }
  QuerySegment(const DictTrie* dictTrie, const HMMModel* model)
    : mixSeg_(dictTrie, model), trie_(dictTrie) {
  }
  ~QuerySegment() {
  }

  void Cut(const string& sentence, vector<string>& words) const {
    Cut(sentence, words, true);
  }
  void Cut(const string& sentence, vector<string>& words, bool hmm) const {
    vector<Word> tmp;
    Cut(sentence, tmp, hmm);
    GetStringsFromWords(tmp, words);
  }
  void Cut(const string& sentence, vector<Word>& words, bool hmm = true) const {
    PreFilter pre_filter(symbols_, sentence);
    PreFilter::Range range;
    vector<WordRange> wrs;
    wrs.reserve(sentence.size()/2);
    while (pre_filter.HasNext()) {
      range = pre_filter.Next();
      Cut(range.begin, range.end, wrs, hmm);
    }
    words.clear();
    words.reserve(wrs.size());
    GetWordsFromWordRanges(sentence, wrs, words);
  }
  void Cut(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res, bool hmm) const {
    //use mix Cut first
    vector<WordRange> mixRes;
    mixSeg_.Cut(begin, end, mixRes, hmm);

    vector<WordRange> fullRes;
    for (vector<WordRange>::const_iterator mixResItr = mixRes.begin(); mixResItr != mixRes.end(); mixResItr++) {
      if (mixResItr->Length() > 2) {
        for (size_t i = 0; i + 1 < mixResItr->Length(); i++) {
          WordRange wr(mixResItr->left + i, mixResItr->left + i + 1);
          if (trie_->Find(wr.left, wr.right + 1) != NULL) {
            res.push_back(wr);
          }
        }
      }
      if (mixResItr->Length() > 3) {
        for (size_t i = 0; i + 2 < mixResItr->Length(); i++) {
          WordRange wr(mixResItr->left + i, mixResItr->left + i + 2);
          if (trie_->Find(wr.left, wr.right + 1) != NULL) {
            res.push_back(wr);
          }
        }
      }
      res.push_back(*mixResItr);
    }
  }

  void Cut(const string& sentence, vector<string>& words, map<std::string,vector<std::string> >&subSentence,bool hmm) const {
    vector<Word> tmp;
	subSentence.clear();
    CutWithSubSentence(sentence, tmp, subSentence,hmm);
    GetStringsFromWords(tmp, words);
  }  
 private:
  void CutWithSubSentence(const string& sentence, vector<Word>& words, map<std::string,vector<std::string> >&subSentence, bool hmm = true) const {
    PreFilter pre_filter(symbols_, sentence);
    PreFilter::Range range;
    vector<WordRange> wrs;
    wrs.reserve(sentence.size()/2);
    while (pre_filter.HasNext()) {
      range = pre_filter.Next();
      CutEx(sentence,range.begin, range.end, wrs,subSentence, hmm);
    }
    words.clear();
    words.reserve(wrs.size());
    GetWordsFromWordRanges(sentence, wrs, words);
  }	
  
  void CutEx(const string& sentence, RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res,
  			  map<std::string,vector<std::string> >&subSentence, bool hmm) const {
    //use mix Cut first
    vector<WordRange> mixRes;
    mixSeg_.Cut(begin, end, mixRes, hmm);

    vector<WordRange> fullRes;
    for (vector<WordRange>::const_iterator mixResItr = mixRes.begin(); mixResItr != mixRes.end(); mixResItr++) {
		if (mixResItr->Length() > 2){
			vector<WordRange> resultWordRange;
			vector< vector<WordRange> >finalResult;
			if(!WordRange(mixResItr->left,mixResItr->right).IsAllAscii()){  //Ascii不需要切分
				std::string key = GetStringFromRunes(sentence,mixResItr->left,mixResItr->right);
				map<std::string,vector<std::string> >::const_iterator iter = subSentence.find(key);
				if(iter==subSentence.end()){
					parse(mixResItr->left,mixResItr->right+1,resultWordRange,CutNoWord,finalResult);
					vector< vector<WordRange> >::const_iterator cIter = finalResult.begin();
					vector<std::string> subVec;
					for(;cIter!=finalResult.end();cIter++){
						vector<WordRange>::const_iterator wordIter = cIter->begin();
						std::string subString;
						for(;wordIter!=cIter->end();wordIter++){
							subString+= GetStringFromRunes(sentence,wordIter->left,wordIter->right);
							if(wordIter+1!=cIter->end()){
								subString+=WORD_SPACE_SYSBOL;
							}
						}
						if(!subString.empty()&& (subString!=key)){
							subVec.push_back(subString);
						}
					}
					if(subVec.size()!=0){
						subSentence.insert(make_pair(key,subVec));
					}
				}
			}
		}
      res.push_back(*mixResItr);

    }
  }

  bool parse(RuneStrArray::const_iterator begin,RuneStrArray::const_iterator end,vector<WordRange>& resultVec,CutWordType previous,vector< vector<WordRange> >&finalResult) const{
	  if(begin>=end){
		  if(finalResult.size()==MaxCutSentenceNumber)
			  return false;
		  if(resultVec.size()!=0)
			  finalResult.push_back(resultVec);
		  else
			  return false;
			  
		  return true;
	  }

	  if(begin+1==end){
		  if(Cut1Word==previous){
			  return true; //不允许2个连续1个字的分词。TRUE结束这次分词
		  }
		  resultVec.push_back(WordRange(begin,begin));
		  if(!parse(begin+1,end,resultVec,Cut1Word,finalResult))
			  return false;
		  resultVec.pop_back();
		  
		  return true;
	  }

	  /*3个字*/
	  if((begin+2)<end && (trie_->Find(begin, begin+2 +1) != NULL)){
		  resultVec.push_back(WordRange(begin,begin+2));
		  if(!parse(begin+3,end,resultVec,Cut3Word,finalResult))
			  return false;
		  resultVec.pop_back();		  
	  }
	  
	  /*2个字*/
	  if(trie_->Find(begin, begin+1 +1) != NULL){
		  resultVec.push_back(WordRange(begin,begin+1));
		  if(!parse(begin+2,end,resultVec,Cut2Word,finalResult))
			  return false;
		  resultVec.pop_back();		  
	  }

	  if(Cut1Word!=previous){
		  resultVec.push_back(WordRange(begin,begin));
		  if(!parse(begin+1,end,resultVec,Cut1Word,finalResult))
			  return false;
		  resultVec.pop_back();	  
	  }

	  return true;
  }

  bool IsAllAscii(const Unicode& s) const {
   for(size_t i = 0; i < s.size(); i++) {
     if (s[i] >= 0x80) {
       return false;
     }
   }
   return true;
  }
  MixSegment mixSeg_;
  const DictTrie* trie_;
}; // QuerySegment

} // namespace cppjieba

#endif
