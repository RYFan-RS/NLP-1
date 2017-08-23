#ifdef WIN32
#pragma warning(disable: 4786)
#else //WIN32
#include <sys/time.h>
//#define MEAN_TIME_EVALUE
#endif //WIN32
//#define MEAN_DEBUG
//#define DUMP_HIT_RULE
//#define TEST_OUTPUT
#include "AnalyzeEngine.h"
#if defined(MEAN_DEBUG) || defined(DUMP_HIT_RULE)
#include <sstream>
#include "Common/lib4/Debug.hpp"
#include "Common/lib4/Definations.hpp"
#endif //MEAN_DEBUG

using namespace std;

namespace sae
{

bool AnalyzeEngine::fastSerialize(const std::string& fileName)
{
    setFileName(fileName);

    if( !readAllocationTable() ) {
        return false;
    }

    if( !readModule(m_data.getDataManager().getTagManager(),"TagManager") ) {
        return false;
    }

    return true;
}

int AnalyzeEngine::init(const std::string& fileName)
{
#ifdef MEAN_TIME_EVALUE
    struct timeval tBegin,tEnd;
    gettimeofday(&tBegin , NULL);
#endif //MEAN_TIME_EVALUE
    if( !readModule(m_data,"Storage") ) {
        return 1;
    }

	//test
	//std::ofstream ofs("keywordoutput.txt");
	//m_data.getConstDataManager().GetConstKeyWordManager().Output(ofs);

    if(m_data.initAfterSerializing() == false) {
        return 2;
    }

#ifdef MEAN_TIME_EVALUE
    gettimeofday(&tEnd , NULL);
    std::cerr<<"Init time:"<<(tEnd.tv_sec-tBegin.tv_sec)*1000+(tEnd.tv_usec-tBegin.tv_usec)/1000<<" millisecond"<<std::endl;
#endif //MEAN_TIME_EVALUE
#ifdef MEAN_DEBUG
    //m_data.dump();
#endif //MEAN_DEBUG

    return 0;
}

int AnalyzeEngine::loadUsrDic(const std::string& fileName, const std::map<std::string, std::set<std::string> >* pWords)
{

#ifdef MEAN_TIME_EVALUE
    struct timeval tBegin,tEnd;
    gettimeofday(&tBegin , NULL);
#endif //MEAN_TIME_EVALUE

    if(!fileName.empty()){
        bool bRet = m_data.parseUsrDic(fileName);
        if(!bRet){
            return 1;
        }
    }

    if(pWords){
        bool bRet = m_data.addUsrWord(pWords);
        if(!bRet){
            return 2;
        }
    }

    m_data.makeEntries();

#ifdef MEAN_TIME_EVALUE
    gettimeofday(&tEnd , NULL);
    std::cerr<<"Load UsrDic time:"<<(tEnd.tv_sec-tBegin.tv_sec)*1000+(tEnd.tv_usec-tBegin.tv_usec)/1000<<" millisecond"<<std::endl;
#endif //MEAN_TIME_EVALUE
#ifdef MEAN_DEBUG
    //m_data.dump();
#endif //MEAN_DEBUG

    return 0;
}

VMeanResult AnalyzeEngine::analyzePhase(const std::string& input, MeanWords& words, const ExecutePhase& phase) const
{

    VRANGE constantRanges;
    VDequeWord dWords;
    LoopInfo loopInfo;
    int numberOfWords,meanResultSize,i;

    //initialize vPhaseMeanResult with just one phase
    PhaseInfo phaseInfo;
    VMeanResult vPhaseMeanResult;
    int lastLoopBegin = 0;

    //record the phase info
    phaseInfo.m_phaseString = phase.dump2Str(m_data.getConstDataManager());
    vPhaseMeanResult.m_phases.push_back(phaseInfo);

    for( int iLoop=0; iLoop<=phase.loopTime; iLoop++ ) { //begin the loop
#ifdef MEAN_DEBUG
        ssDebug << "===Now searching from rule "<<Utility::int2Str(phase.start)<<" to rule "<<Utility::int2Str(phase.end)<<" for the ";
        ssDebug << Utility::int2Str(iLoop)<<" times.===";
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL0);
        ssDebug << "===";
        for(size_t iDebug=0; iDebug<words.size(); iDebug++) {
            ssDebug << Utility::int2Str(iDebug) << ":";
            ssDebug << words[iDebug].dump2Str(m_data.getConstDataManager());
            ssDebug << " ";
        }
        ssDebug << "===";
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL0);
#endif //MEAN_DEBUG
        numberOfWords = words.size();

        for( int wordIndex=0; wordIndex<numberOfWords; wordIndex++ ) {
#ifdef MEAN_DEBUG
            ssDebug << "Try to find pos " <<Utility::int2Str(wordIndex)<<": ";
            ssDebug << words[wordIndex].dump2Str(m_data.getConstDataManager());
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
#endif //MEAN_DEBUG
            findRulesWithWordAndAnalyze(words,wordIndex,phase.start,phase.end,vPhaseMeanResult);
            findRulesWithBasicWordAndAnalyze(words,wordIndex,phase.start,phase.end,vPhaseMeanResult);
            findRulesWithTagAndAnalyze(words,wordIndex,phase.start,phase.end,vPhaseMeanResult);
        }

        //add the MeanResult data
        meanResultSize = vPhaseMeanResult.size();
        loopInfo.m_words = words;
        if(meanResultSize > lastLoopBegin) {
            loopInfo.m_start = lastLoopBegin;
            loopInfo.m_end = meanResultSize - 1;
            lastLoopBegin = meanResultSize;
            vPhaseMeanResult.m_phases[0].push_back(loopInfo);
        } else {
            break;
        }

        //change MeanWords to deque<MeanWords> in order to do the actions
        changeMeanWordsToDequeWords(words, dWords, numberOfWords);

        //check the size of the result
        for(i=loopInfo.m_start; i<=loopInfo.m_end; i++) {
            vPhaseMeanResult[i].setPointersForDump(m_data.getNormalRuleManagerPointer(),m_data.getConstDataManager());

            //tag the mean tags
            if(vPhaseMeanResult[i].hasTagRule() == true) {
#ifdef MEAN_DEBUG
                ssDebug << "Execute the tag at "<<vPhaseMeanResult[i].getLineIndexInfo()<<".";
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL0);
#endif //MEAN_DEBUG
                if(doTagRule(dWords,vPhaseMeanResult[i]) != 0) {
                    /*error handling?*/
                };
            }

            vPhaseMeanResult[i].calculateRange();	//calculate the range
        }
        constantRanges.clear();

        //update the words
        words.clear();
        for(i=0; i<numberOfWords; i++) {
            for(int j=0; j<(int)dWords[i].size(); j++) {
                words.push_back(dWords[i][j]);
            }
        }
        words.setTags(dWords.getTags());
        words.setHoldWords(dWords.getHoldWords());

        //judge whether can jump out the iteration
        if(iLoop > 0 && loopInfo.m_words == words) {
            break;
        }
    }

    return vPhaseMeanResult;
}

void AnalyzeEngine::findRulesWithWordAndAnalyze(const MeanWords& words, const int& wordIndex, const int& phaseStart, const int& phaseEnd, VMeanResult& vPhaseMeanResult) const
{

    MeanResult meanResult;
    const std::multimap<int,ENTRY>& wordEntries = m_data.getConstEntryManager().getWordEntries();
    std::pair<std::multimap<int,ENTRY>::const_iterator,std::multimap<int,ENTRY>::const_iterator> pairEntryIter;
    std::multimap<int,ENTRY>::const_iterator entryIter;
    ENTRY wordEntryRule;
    int indexOfLastFoundRule = -1;

#ifdef MEAN_DEBUG
    int lastOutRangeRuleID = -1;
#endif //MEAN_DEBUG

    if(words[wordIndex].m_isDigitalWord == true) { //has this word been indexed ?
        pairEntryIter = wordEntries.equal_range(words[wordIndex].getWord());
        if( pairEntryIter.first != pairEntryIter.second ) { //does this word exist in the map of entries ?
#ifdef MEAN_DEBUG
            ssDebug << "Found word `";
            ssDebug << m_data.getConstDataManager().getConstWordManager().find(words[wordIndex].getWord());
            ssDebug << "' in word entry.";
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG
            for(entryIter=pairEntryIter.first; entryIter!=pairEntryIter.second; ++entryIter) {
                wordEntryRule = entryIter->second;
                if(wordEntryRule.m_index!=indexOfLastFoundRule && wordEntryRule.m_indexRuleID>=phaseStart && wordEntryRule.m_indexRuleID<=phaseEnd) { //is this entry in the wanted range ? this rule must has't indexed last time.
                    if(analyzeRuleWithWord(words,wordIndex,m_data.getConstNormalRuleManager().getConstRule(wordEntryRule.m_index),meanResult) == 0) {
                        meanResult.setRuleEntry(wordEntryRule);
                        vPhaseMeanResult.push_back(meanResult);
                        indexOfLastFoundRule = wordEntryRule.m_index;
#ifdef MEAN_DEBUG
                        ssDebug << "Check word `";
                        ssDebug << m_data.getConstDataManager().getConstWordManager().find(words[wordIndex].getWord());
                        ssDebug << "' in word entry successfully finished.";
                        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
#endif //MEAN_DEBUG

                    }
                    meanResult.clear(); //clear the cache result
                }
#ifdef MEAN_DEBUG
                else {
                    if(lastOutRangeRuleID != wordEntryRule.m_indexRuleID) {
                        ssDebug << "Failed for rule ";
                        ssDebug << Utility::int2Str(wordEntryRule.m_indexRuleID);
                        ssDebug << " is out of range ";
                        ssDebug << Utility::int2Str(phaseStart)<<"-"<<Utility::int2Str(phaseEnd);
                        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
                        lastOutRangeRuleID = wordEntryRule.m_indexRuleID;
                    }
                }
#endif //MEAN_DEBUG
            }
        }

#ifdef MEAN_DEBUG
        else {
            ssDebug << "Failed for word `";
            ssDebug << m_data.getConstDataManager().getConstWordManager().find(words[wordIndex].getWord());
            ssDebug << "' is not exsit in word entry map.";
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
        }
#endif //MEAN_DEBUG

    }

#ifdef MEAN_DEBUG
    else {
        ssDebug << "Failed for word `";
        ssDebug << words[wordIndex].m_sWord;
        ssDebug << "' is not a digital word.";
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
    }
#endif //MEAN_DEBUG

}

void AnalyzeEngine::findRulesWithBasicWordAndAnalyze(const MeanWords& words, const int& wordIndex, const int& phaseStart, const int& phaseEnd, VMeanResult& vPhaseMeanResult) const
{

    MeanResult meanResult;
    const std::multimap<int,ENTRY>& wordEntries = m_data.getConstEntryManager().getWordEntries();
    std::pair<std::multimap<int,ENTRY>::const_iterator,std::multimap<int,ENTRY>::const_iterator> pairEntryIter;
    std::multimap<int,ENTRY>::const_iterator entryIter;
    ENTRY wordEntryRule;
    int indexOfLastFoundRule = -1;

#ifdef MEAN_DEBUG
    int lastOutRangeRuleID = -1;
#endif //MEAN_DEBUG

    if(words[wordIndex].getBasicWord() > -1) { //if the basic form of the word is indexed
        pairEntryIter = wordEntries.equal_range(words[wordIndex].getBasicWord());
        if( pairEntryIter.first != pairEntryIter.second ) { //does this word exist in the map of entries ?
#ifdef MEAN_DEBUG
            ssDebug << "Found basic word `";
            ssDebug << m_data.getConstDataManager().getConstWordManager().find(words[wordIndex].getBasicWord());
            ssDebug << "' in word entry.";
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG
            for(entryIter=pairEntryIter.first; entryIter!=pairEntryIter.second; ++entryIter) {
                wordEntryRule = entryIter->second;
                if(wordEntryRule.m_index!=indexOfLastFoundRule && wordEntryRule.m_indexRuleID>=phaseStart && wordEntryRule.m_indexRuleID<=phaseEnd) { //is this entry in the wanted range ? this rule must has't indexed last time.
                    if(analyzeRuleWithWord(words,wordIndex,m_data.getConstNormalRuleManager().getConstRule(wordEntryRule.m_index),meanResult) == 0) {
                        meanResult.setRuleEntry(wordEntryRule);
                        vPhaseMeanResult.push_back(meanResult);
                        indexOfLastFoundRule = wordEntryRule.m_index;
#ifdef MEAN_DEBUG
                        ssDebug << "Check basic word `";
                        ssDebug << m_data.getConstDataManager().getConstWordManager().find(words[wordIndex].getBasicWord());
                        ssDebug << "' in word entry successfully finished.";
                        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
#endif //MEAN_DEBUG

                    }
                    meanResult.clear(); //clear the cache result
                }
#ifdef MEAN_DEBUG
                else {
                    if(lastOutRangeRuleID != wordEntryRule.m_indexRuleID) {
                        ssDebug << "Failed for rule ";
                        ssDebug << Utility::int2Str(wordEntryRule.m_indexRuleID);
                        ssDebug << " is out of range ";
                        ssDebug << Utility::int2Str(phaseStart)<<"-"<<Utility::int2Str(phaseEnd);
                        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
                        lastOutRangeRuleID = wordEntryRule.m_indexRuleID;
                    }
                }
#endif //MEAN_DEBUG
            }
        }

#ifdef MEAN_DEBUG
        else {
            ssDebug << "Failed for word `";
            ssDebug << m_data.getConstDataManager().getConstWordManager().find(words[wordIndex].getBasicWord());
            ssDebug << "' is not exsit in word entry map.";
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
        }
#endif //MEAN_DEBUG
    }

#ifdef MEAN_DEBUG
    else {
        ssDebug << "Failed for word `";
        ssDebug << words[wordIndex].getBasicWord();
        ssDebug << "' is not a digital word.";
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
    }
#endif //MEAN_DEBUG
}

void AnalyzeEngine::findRulesWithTagAndAnalyze(const MeanWords& words, int& wordIndex, const int& phaseStart, const int& phaseEnd, VMeanResult& vPhaseMeanResult) const
{
    MeanResult meanResult;
    const std::multimap<int,ENTRY>& tagEntries = m_data.getConstEntryManager().getTagEntries();
    std::pair<std::multimap<int,ENTRY>::const_iterator,std::multimap<int,ENTRY>::const_iterator> pairEntryIter;
    std::multimap<int,ENTRY>::const_iterator entryIter;
    ENTRY tagEntryRule;
    int indexOfLastFoundRule = -1;

#ifdef MEAN_DEBUG
    int lastOutRangeRuleID = -1;
#endif //MEAN_DEBUG

    std::vector<int> tags = words[wordIndex].getTags();
    if(!tags.empty()) {
        for(int i=0; i<tags.size(); i++) {
            pairEntryIter = tagEntries.equal_range(tags[i]);

            if( pairEntryIter.first != pairEntryIter.second ) { //is this word exist int the map of entries ?
#ifdef MEAN_DEBUG
                ssDebug << "Find tag `";
                ssDebug << m_data.getConstDataManager().getConstTagManager().find(tags[i]);
                ssDebug << "' in tag entry.";
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG
                for(entryIter=pairEntryIter.first; entryIter!=pairEntryIter.second; ++entryIter) { //is this entry in the wanted range ?
                    tagEntryRule = entryIter->second;
                    if(tagEntryRule.m_index!=indexOfLastFoundRule && tagEntryRule.m_indexRuleID>=phaseStart && tagEntryRule.m_indexRuleID<=phaseEnd) { //is this entry in the wanted range ? this rule must has't indexed last time.

                        if(analyzeRuleWithTag(words,wordIndex,m_data.getConstNormalRuleManager().getConstRule(tagEntryRule.m_index),meanResult) == 0) {
                            meanResult.setRuleEntry(tagEntryRule);
                            vPhaseMeanResult.push_back(meanResult);
                            indexOfLastFoundRule = tagEntryRule.m_index;
#ifdef MEAN_DEBUG
                            ssDebug << "Check tag `";
                            ssDebug << m_data.getConstDataManager().getConstTagManager().find(tags[i]);
                            ssDebug << "' in tag entry successfully finished.";
                            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
#endif //MEAN_DEBUG
                        }
                        meanResult.clear();//clear the cache result
                    }
#ifdef MEAN_DEBUG
                    else {
                        if(lastOutRangeRuleID != tagEntryRule.m_indexRuleID) {
                            ssDebug << "Failed for rule ";
                            ssDebug << Utility::int2Str(tagEntryRule.m_indexRuleID);
                            ssDebug << " is out of range ";
                            ssDebug << Utility::int2Str(phaseStart)<<"-"<<Utility::int2Str(phaseEnd);
                            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
                            lastOutRangeRuleID = tagEntryRule.m_indexRuleID;
                        }
                    }
#endif //MEAN_DEBUG
                }
            }
#ifdef MEAN_DEBUG
            else {
                ssDebug << "Failed for tag `";
                ssDebug << m_data.getConstDataManager().getConstTagManager().find(tags[i]);
                ssDebug << "' is not exsit in tag entry map.";
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL1);
            }
#endif //MEAN_DEBUG
        }

    }
}

int AnalyzeEngine::analyzeRuleWithWord(const MeanWords& words, const int& wordIndex, const NormalRule& rule, MeanResult& result) const
{
    const std::vector<KeyCell>& keys = rule.getConstKeyPart().getConstKeys();
    VRANGE vRange;
    RANGE range;
    int returnOfCheckingEntryPosition = -1;

#ifdef MEAN_DEBUG
    ssDebug << "Enter rule at "<<rule.getLineIndexInfo()<<" in word entry.";
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#if defined(DUMP_HIT_RULE) && defined(MEAN_DEBUG)
    rule.dump(ssDebug);
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL0);
#endif //defined(DUMP_HIT_RULE) && defined(MEAN_DEBUG)
#endif //MEAN_DEBUG

    //if the entry key has been defined positon, check it
    if(keys[0].getPosition().isSearchAnywhere() == false) {
        returnOfCheckingEntryPosition = checkEntryPosition(words,wordIndex,keys[0].getPosition());
        if(returnOfCheckingEntryPosition < 0) {
#ifdef MEAN_DEBUG
            ssDebug << "Failed for checking the position limitation of entry KEY.";
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG

            return 13;
        }
    }

    //check the entry key
    if(checkEntryKey(keys[0],wordIndex,words,range,WORD_ENTRY) == false) {

#ifdef MEAN_DEBUG
        ssDebug << "Failed for checking the content of entry KEY.";
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG

        return 14;
    }

    //judge if the returnOfCheckingEntryPosition > 0, at this time this is a backward search from tail
    if(returnOfCheckingEntryPosition > 0) {
        if(checkEntryPositionAtTail(keys[0].getPosition(),range,words.size()-1) == false) {
#ifdef MEAN_DEBUG
            ssDebug << "Failed for checking the position limitation of entry KEY.";
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG
            return 1;
        }
    }

    //check the script limit of entry key
    if(keys[0].getScriptLimit().match(words,m_data.getConstDataManager(),range) == false) return 3;

    //record the entry key range
    result.m_keyRanges.push_back(range);

#ifdef MEAN_DEBUG
    ssDebug << "Check the entry KEY returns range "<<Utility::int2Str(range.start);
    ssDebug << "--"<<Utility::int2Str(range.end);
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG

    //check from KEY1 to the end
    if(checkNormalKeys(words,keys,wordIndex,vRange,range,result) == false) return 4;

#ifdef MEAN_DEBUG
    ssDebug << "Check rule "<<Utility::int2Str(rule.getIdx())<<" successfully finished.";
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG

    return 0;
}

int AnalyzeEngine::analyzeRuleWithTag(const MeanWords& words, int& wordIndex, const NormalRule& rule, MeanResult& result) const
{
    const std::vector<KeyCell>& keys = rule.getConstKeyPart().getConstKeys();
    VRANGE vRange;
    RANGE range;
    int returnOfCheckingEntryPosition = -1;

#ifdef MEAN_DEBUG
    ssDebug << "Enter rule at "<<rule.getLineIndexInfo()<<" in POS entry.";
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#if defined(DUMP_HIT_RULE) && defined(MEAN_DEBUG)
    rule.dump(ssDebug);
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL0);
#endif //defined(DUMP_HIT_RULE) && defined(MEAN_DEBUG)
#endif //MEAN_DEBUG

    //if the entry key has been defined positon, check it
    if(keys[0].getPosition().isSearchAnywhere() == false) {
        returnOfCheckingEntryPosition = checkEntryPosition(words,wordIndex,keys[0].getPosition());
        if(returnOfCheckingEntryPosition < 0) {
#ifdef MEAN_DEBUG
            ssDebug << "Failed for checking the position limitation of entry KEY.";
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG
            return 13;
        }
    }

    //check the entry key
    if(checkEntryKey(keys[0],wordIndex,words,range,WORD_ENTRY) == false) {
#ifdef MEAN_DEBUG
        ssDebug << "Failed for checking the content of entry KEY.";
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG
        return 14;
    }

    //judge if the returnOfCheckingEntryPosition > 0, at this time this is a backward search from tail
    if(returnOfCheckingEntryPosition > 0) {
        if(checkEntryPositionAtTail(keys[0].getPosition(),range,words.size()-1) == false) {
#ifdef MEAN_DEBUG
            ssDebug << "Failed for checking the position limitation of entry KEY.";
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG
            return 1;
        }
    }

    //check the script limit of entry key
    if(keys[0].getScriptLimit().match(words,m_data.getConstDataManager(),range) == false) return 3;

    //record the entry key range
    result.m_keyRanges.push_back(range);
    /*if(start != end){
    	pos = end;
    }*/

#ifdef MEAN_DEBUG
    ssDebug << "Check the entry KEY returns range "<<Utility::int2Str(range.start);
    ssDebug << "--"<<Utility::int2Str(range.end);
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG

    //check from KEY1 to the end
    if(checkNormalKeys(words,keys,wordIndex,vRange,range,result) == false) return 4;

#ifdef MEAN_DEBUG
    ssDebug << "Check rule "<<Utility::int2Str(rule.getIdx())<<" successfully finished.";
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL2);
#endif //MEAN_DEBUG

    return 0;
}

void AnalyzeEngine::changeMeanWordsToDequeWords(const MeanWords& words, VDequeWord& dWords, const int& wordSize) const
{
    dWords.clear();
    dWords.resize(wordSize);
    for(size_t i=0; i<wordSize; i++) {
        dWords[i].push_back(words[i]);
    }
    dWords.setTags(words.getTags());
    dWords.setHoldWords(words.getHoldWords());
}

bool AnalyzeEngine::checkEntryKey(const KeyCell& key, const int& wordIndex, const MeanWords& words, RANGE& range, const ENTRY_TYPE& entryType) const
{
    VRANGE vRange;
    const std::vector<SearchModeCell>& vSearchModeCell = key.getSearchMode().getSearchModeCells();
    bool executedFlag = false;

#ifdef MEAN_DEBUG
    ssDebug << "Try to deal with entry KEY.";
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG
    range.start = wordIndex;
    range.end = words.size()-1;

#ifdef MEAN_DEBUG
    ssDebug << "Position calculate result is ";
    ssDebug << Utility::int2Str(range.start) << "--" << Utility::int2Str(range.end);
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG

    for(size_t i=0; i<vSearchModeCell.size(); i++) {
        vRange.clear();
        vRange.push_back(range);
        switch(vSearchModeCell[i].m_searchType) {
        case WORD_TYPE:
            if(vSearchModeCell[i].m_limitMode == true) {
                if(key.getConstWordLimit().checkLimitWord(words,vRange) != 0) {
                    return false;
                } else {
                    range = vRange[0];
                }
                executedFlag = true;
            } else if(vSearchModeCell[i].m_phraseMode == true) {
                if(key.getConstWordLimit().checkPhraseWord(words,vRange,range) != 0) {
                    return false;
                }
                executedFlag = true;
                //newPos = range.end;
            } else {
                if(key.getConstWordLimit().checkWord(words,vRange,range) != 0) {
                    return false;
                }
                executedFlag = true;
            }
            break;
        case TAG_TYPE:
            if(vSearchModeCell[i].m_limitMode == true) {
                if(key.getTagLimit().checkLimitTag(words,vRange) != 0) {
                    return false;
                } else {
                    range = vRange[0];
                }
                executedFlag = true;
            } else if(vSearchModeCell[i].m_phraseMode == true) {
                if(key.getTagLimit().checkPhraseTag(words,vRange,range) != 0) {
                    return false;
                }
                executedFlag = true;
                //newPos = range.end;
            } else {
                if(key.getTagLimit().checkTag(words,vRange,range) != 0) {
                    return false;
                }
                executedFlag = true;
            }
            break;
        default:
#ifdef MEAN_DEBUG
            ssDebug << "Meet an unknow SearchType.";
            acl::debug::ERRMSG(ERR_STREAM,ssDebug,acl::debug::LEVEL0);
#endif //MEAN_DEBUG
            return false;
        }
        if(range.start != wordIndex) {
#ifdef MEAN_DEBUG
            ssDebug << "The start position changed to "<<Utility::int2Str(range.start)<<" as";
            ssDebug << " the result of searching process.";
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG
            return false;
        }
    }

    //if nothing has been done, the postion need change from a range back to a point
    if(executedFlag == false) {
        range.end = range.start;
    }

    //the wordIndex maybe changed as the phrase search process
    //wordIndex = newPos;

    return true;
}

bool AnalyzeEngine::checkNormalKeys(const MeanWords& words, const std::vector<KeyCell>& keys, const int& wordIndex, VRANGE& vRange, RANGE& range, MeanResult& result) const
{
    size_t i,size = keys.size(),j;
    bool notMatchedFlag;
    VCell dynamicVCell;
#ifdef MEAN_DEBUG
    stringstream ssDebug;
#endif //MEAN_DEBUG

    //check the key form key1
    for(i=1; i<size; i++) {
#ifdef MEAN_DEBUG
        ssDebug << "Try to deal with KEY" << i;
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
        ssDebug.str("");
#endif //MEAN_DEBUG

        //clear data
        vRange.clear();
        range.start = -1;
        range.end = -1;
        notMatchedFlag = false;

        //get the word ranges to be checked from the key's Position
        if(getWordRangesFromPosition(words,wordIndex,keys[i].getPosition(),result,vRange) != 0) {
            if(keys[i].isOmissibleKey() == false) {
                return false;
            } else {
                //record the weak deny key range
                range.start = -1;
                range.end = -1;
#ifdef MEAN_DEBUG
                ssDebug << "For this is a weak deny KEY, the return result changes to -1--1.";
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
                ssDebug.str("");
#endif //MEAN_DEBUG
                result.addKeyRange(range);
#ifdef MEAN_DEBUG
                ssDebug << "Check weak deny KEY"<<i<<" successfully finished.";
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
                ssDebug.str("");
#endif //MEAN_DEBUG
                continue;
            }
        }

        //check the need checked positions
        const std::vector<SearchModeCell>& vSearchModeCell = keys[i].getSearchMode().getSearchModeCells();
        for(j=0; j<vSearchModeCell.size(); j++) {
            vRange.setContinuousSearchFlag(vSearchModeCell[j].continuousTag);
            switch(vSearchModeCell[j].m_searchType) {
            case WORD_TYPE:
                keys[i].getConstWordLimit().makeReferKeyInstant( words,result.m_keyRanges,m_data.getConstDataManager(),dynamicVCell );
                if(vSearchModeCell[j].m_limitMode == true) {
#ifdef MEAN_DEBUG
					ssDebug << "LW Search {"<<words.dump2Str()<<"} use Key {";
					keys[i].dump(m_data.getConstDataManager(),ssDebug);
					ssDebug <<"}";
					acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
					ssDebug.str("");
#endif //MEAN_DEBUG
                    if(keys[i].getConstWordLimit().checkLimitWord(words,vRange,dynamicVCell) != 0) {
                        dynamicVCell.freeCellMemories();
                        return false;
                    } else {
                        range = vRange[0];
                    }
                } else if(vSearchModeCell[j].m_phraseMode == true) {
#ifdef MEAN_DEBUG
					ssDebug << "PW Search {"<<words.dump2Str()<<"} use Key {";
					keys[i].dump(m_data.getConstDataManager(),ssDebug);
					ssDebug <<"}";
					acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
					ssDebug.str("");
#endif //MEAN_DEBUG
                    if(keys[i].getConstWordLimit().checkPhraseWord(words,vRange,range,dynamicVCell) != 0) {
                        dynamicVCell.freeCellMemories();
                        return false;
                    }
                } else {
#ifdef MEAN_DEBUG
//					ssDebug << "W Search {"<<words.dump2Str()<<"} use Key {";
//					keys[i].dump(m_data.getConstDataManager(),ssDebug);
//					ssDebug <<"}";
//					acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
//					ssDebug.str("");
#endif //MEAN_DEBUG
                    if(keys[i].getConstWordLimit().checkWord(words,vRange,range,dynamicVCell) != 0) {
                        dynamicVCell.freeCellMemories();
                        return false;
                    }
                }
                dynamicVCell.freeCellMemories();
#ifdef MEAN_DEBUG
                ssDebug << "Use search mode '";
                vSearchModeCell[j].dump(ssDebug);
                ssDebug << "' to check hit range "<<range;
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
                ssDebug.str("");
#endif //MEAN_DEBUG
                break;
            case TAG_TYPE:
                if(vSearchModeCell[j].m_limitMode == true) {
                    if(keys[i].getTagLimit().checkLimitTag(words,vRange) != 0) {
                        return false;
                    } else {
                        range = vRange[0];
                    }
                } else if(vSearchModeCell[j].m_phraseMode == true) {
                    if(keys[i].getTagLimit().checkPhraseTag(words,vRange,range) != 0) {
                        return false;
                    }
                } else {
                    if(keys[i].getTagLimit().checkTag(words,vRange,range) != 0) {
                        return false;
                    }
                }
                break;
#ifdef MEAN_DEBUG
                ssDebug << "Use search mode '";
                vSearchModeCell[j].dump(ssDebug);
                ssDebug << "' to check hit range "<<range;
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
                ssDebug.str("");
#endif //MEAN_DEBUG
            default:
#ifdef MEAN_DEBUG
                ssDebug << "Meet an unknow SearchType.";
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL0);
                ssDebug.str("");
#endif //MEAN_DEBUG
                return false;
            }
            vRange.clear();
            vRange.push_back(range);
        }
#ifdef MEAN_DEBUG
        ssDebug<<"Check KEY"<<i<<" hit range "<<range;
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
        ssDebug.str("");
#endif //MEAN_DEBUG

        //check the script limit of normal keys
        if(keys[i].getScriptLimit().match(words,m_data.getConstDataManager(),range) == false) {
            return false;
        }

        //record the key range
        result.addKeyRange(range);
#ifdef MEAN_DEBUG
        ssDebug<<"Check KEY"<<i<<" returns range "<<range;
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
        ssDebug.str("");
#endif //MEAN_DEBUG
    }

    return true;
}

int AnalyzeEngine::checkEntryPosition(const MeanWords& words, const int& wordIndex, const Position& position) const
{
    const std::vector<PosCell>& vPos = position.getSearchPoses();
    int i,iPos;
    const int size = vPos.size();
    const int tail = words.size() - 1;

    for(i=0; i<size; i++) {
        //convert the virtual tail to the real tail
        if(vPos[i].m_num == TAIL) {
            iPos = TheMaxOf(tail - vPos[i].m_offset, 0);
        } else {
            iPos = vPos[i].m_num;
        }

        if(vPos[i].m_tDirection == DIRECTION_BEFORE) {
            if(vPos[i].m_num == TAIL) {
                return 1;
            }
            if(wordIndex >= iPos) {
                return -1;
            }
        } else if(vPos[i].m_tDirection == DIRECTION_AFTER) {
            if(wordIndex <= iPos) {
                return -2;
            }
        } else {
            if(vPos[i].m_num == TAIL) {
                return 2;
            }
            if(wordIndex != iPos) {
                return -3;
            }
        }
    }

    return 0;
}

bool AnalyzeEngine::checkEntryPositionAtTail(const Position& position, const RANGE& range, const int& tail) const
{
    const std::vector<PosCell>& vPos = position.getSearchPoses();

    for(size_t i=0; i<vPos.size(); i++) {
        if(vPos[i].m_num == TAIL) {
            if(vPos[i].m_offset == 0 && vPos[i].m_tDirection == DIRECTION_BEFORE) { //T or T-N
                if(range.end == tail) {
                    return false;
                }
            } else { //T-
                const int iPos = TheMaxOf(tail - vPos[i].m_offset, 0);
                if(range.end != iPos) {
                    return false;
                }
            }
        }
    }

    return true;
}

int AnalyzeEngine::getWordRangesFromPosition(const MeanWords& words,const int& wordIndex, const Position& position, const MeanResult& result, VRANGE& vRange) const
{
    if(position.isSearchAnywhere() == true) return 0;
    const std::vector<PosCell>& vPos = position.getSearchPoses();

    int i;
    int start,end,num;
    int index=0, highNum=-1, lowNum=-1;
    const int tail = words.size()-1;
    bool allFault = true, skip=false, bLastAbandon=false;

    //init for searching
    RANGE range;
    range.start = 0;
    range.end = tail;
    vRange.push_back(range);
    for(i=0; i<vPos.size(); i++) {
        //get last range
        if(vPos[i].m_isIntersection == true) { //intersection
            start= vRange[index].start;
            end  = vRange[index].end;
        } else {
            start = 0;
            end = tail;
        }
        //caculate
        if(vPos[i].m_isKeyType == true) { //intersection + key type
            if(result.m_keyRanges[vPos[i].m_num].isNull()) { //when the refer key is an omissbile key and points to an impossible position
                start = -1;
                end = -1;
            }
            if(vPos[i].m_tDirection == DIRECTION_BEFORE) {
                if(vPos[i].m_whichPosOfKey == -1) {
                    if(vPos[i].m_offset != 0) {
                        start = result.m_keyRanges[vPos[i].m_num].start - vPos[i].m_offset;
                        end = start;
                    } else {
                        end = result.m_keyRanges[vPos[i].m_num].start - 1;
                    }
                } else {
                    end = result.m_keyRanges[vPos[i].m_num].start + vPos[i].m_whichPosOfKey;
                    if(end > result.m_keyRanges[vPos[i].m_num].end) {
                        end = result.m_keyRanges[vPos[i].m_num].end;
                    }
                }
            } else if(vPos[i].m_tDirection == DIRECTION_AFTER) {
                if(vPos[i].m_whichPosOfKey == -1) {
                    if(vPos[i].m_offset != 0) {
                        end = result.m_keyRanges[vPos[i].m_num].end + vPos[i].m_offset;
                        start = end;
                    } else {
                        start = result.m_keyRanges[vPos[i].m_num].end + 1;
                    }
                } else {
                    start = result.m_keyRanges[vPos[i].m_num].start + vPos[i].m_whichPosOfKey;
                    if(start > result.m_keyRanges[vPos[i].m_num].end) {
                        start = result.m_keyRanges[vPos[i].m_num].end;
                    }
                }
            } else {
                if(vPos[i].m_whichPosOfKey == -1) {
                    start = result.m_keyRanges[vPos[i].m_num].start;
                    end = result.m_keyRanges[vPos[i].m_num].end;
                } else {
                    start = result.m_keyRanges[vPos[i].m_num].start + vPos[i].m_whichPosOfKey;
                    if(start > result.m_keyRanges[vPos[i].m_num].end) {
                        start = result.m_keyRanges[vPos[i].m_num].end;
                    }
                    end = start;
                }
            }
        } else { //intersection + number type
            if(vPos[i].m_hDirection == DIRECTION_BEFORE) {
                if(vPos[i].m_num != -1) {
                    end = wordIndex - vPos[i].m_num;
                    if(vPos[i].m_tDirection == DIRECTION_BEFORE) {
                        end--;
                    } else if(vPos[i].m_tDirection == DIRECTION_AFTER) {
                        start = end + 1;
                        end = vRange[index].end;
                    } else {
                        start = end;
                    }
                } else {
                    end = wordIndex - 1;
                }
            } else if(vPos[i].m_hDirection == DIRECTION_AFTER) {
                if(vPos[i].m_num != -1) {
                    start = wordIndex + vPos[i].m_num;
                    if(vPos[i].m_tDirection == DIRECTION_AFTER) {
                        start++;
                    } else if(vPos[i].m_tDirection == DIRECTION_BEFORE) {
                        end = start - 1;
                        start = vRange[index].start;
                    } else {
                        end = start;
                    }
                } else {
                    start = wordIndex + 1;
                }
            } else { // no hDirection
                num = vPos[i].instantiateTail(tail);
                if(num != -1) {
                    if(vPos[i].m_tDirection == DIRECTION_BEFORE) {
                        end = num - 1;
                    } else if(vPos[i].m_tDirection == DIRECTION_AFTER) {
                        start = num + 1;
                    } else { // no tDirection
                        if(lowNum != -1 && num == lowNum - 1) { // check if explicit position numbers are consecutive
                            vRange[index-1].start = num;
                            lowNum = num;
                            skip = true;
                        } else if(highNum != -1 && num == highNum + 1) { // check if explicit position numbers are consecutive
                            vRange[index-1].end = num;
                            highNum = num;
                            skip = true;
                        } else {
                            start = num;
                            end = start;
                        }
                        if(highNum==-1) highNum = num;
                        if(lowNum==-1) lowNum = num;
                    }
                } else {
                    //impossible
                }
            }
        }
		
        //judge the if the range is illegal
        if(end<0 || start>tail || start>end || (end==0 && start<0)) {
			bLastAbandon = true;
#ifdef MEAN_DEBUG
            ssDebug << "Get position error, abandon this result";
#endif //MEAN_DEBUG
            if(i == vPos.size() -1 && allFault == true) {
#ifdef MEAN_DEBUG
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG
                return 1;
            } else if( i != 0 && vPos[i].m_isIntersection == true) {
#ifdef MEAN_DEBUG
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG
                return 1;
            } else if( position.allIntersection() == true ) {
#ifdef MEAN_DEBUG
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG
                return 1;
			} else {
#ifdef MEAN_DEBUG
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG
                continue;
            }
        } else {
            if(vPos[i].m_isIntersection == false || i==0) {
                allFault = false;
            }
        }
		
        //set the range
        if(vPos[i].m_isIntersection == true) { //intersection
            if(intersection(start,end,vRange[index].start,vRange[index].end) == true) {
                vRange[index].start = start;
                vRange[index].end = end;
                vRange[index].setBackwardSearchFlag( vPos[i].isBackwardSearch() );
            } else {
#ifdef MEAN_DEBUG
                ssDebug << "Get the intersection position error, meets an impossible result.";
                acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG
                return 2;
            }
        } else {
            if(i == 0 || bLastAbandon == true) {
                vRange[index].start = start;
                vRange[index].end = end;
                vRange[index].setBackwardSearchFlag( vPos[i].isBackwardSearch() );
            } else {
                if(skip) {
                    skip = false;
                    continue;
                }
                RANGE range;
                range.start = max(start,0);
                range.end = min(end,tail);
                range.setBackwardSearchFlag( vPos[i].isBackwardSearch() );
                vRange.push_back(range);
            }
            index++;
        }
		bLastAbandon = false;
    }

#ifdef MEAN_DEBUG
    ssDebug << "Position calculate result is ";
    for(int iDebug=0; iDebug<(int)vRange.size(); iDebug++) {
        ssDebug << " " << Utility::int2Str(vRange[iDebug].start) << "--" << Utility::int2Str(vRange[iDebug].end);
    }
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
#endif //MEAN_DEBUG

    return 0;
}

int AnalyzeEngine::doTagRule(VDequeWord& dWords, MeanResult& result) const
{
    const std::vector<TagRuleCell>& tagRuleTagSets = result.getTagRule().getTagSets();
    int i,size=tagRuleTagSets.size(),j;
    size_t k;
    SVector<int> dWordTags;

    result.m_tagsPerWord.resize(dWords.size());
    result.m_deletedTagsPerWord.resize(dWords.size());

    for(i=0; i<size; i++) {
        std::vector<std::pair<int,int> > dWordsIndex;

        if(tagRuleTagSets[i].m_tagLimit.size() != 1) continue;
        const Cell* tagRuleTagSet = tagRuleTagSets[i].m_tagLimit[0];

        if(tagRuleTagSets[i].m_keyRange.m_startKeyNum.m_keyNum > -1) {
            if( tagRuleTagSets[i].m_keyRange.isWholeSentence() ) {
                dWordTags = dWords.getTags();
                applyTagsToWord(dWordTags,tagRuleTagSet);
                dWords.setTags(dWordTags);
                result.setSentenceTags(dWordTags);
            } else {
                RANGE wordRange = result.getKeyRanges().getWordRangeFromKeyRange(tagRuleTagSets[i].m_keyRange);
                if(wordRange == RANGE::Error) {
                    continue;
                }

                for(j=wordRange.start; j<=wordRange.end; j++) {
                    for(k=0; k<dWords[j].size(); k++) {
                        dWordTags = dWords[j][k].getTags();
                        applyTagsToWord(dWordTags,tagRuleTagSet,result.m_deletedTagsPerWord[j]);
                        dWords[j][k].setTags(dWordTags);
                        applyTagsToWord(result.m_tagsPerWord[j],tagRuleTagSet);
                    }
                }
            }
        }
    }

    return 0;
}

bool AnalyzeEngine::applyTagsToWord(std::vector<int>& wordTags, const Cell* tagRuleTagSet) const
{
    size_t i,j;
    const SVector<int>& tagRuleTags = tagRuleTagSet->getContent();

    for(i=0; i < tagRuleTags.size(); i++) {

        if( tagRuleTagSet->getNegativeTags()[i] ) {
            std::vector<int> newTags;
            for(j=0; j<wordTags.size(); j++) {
                if( tagRuleTags[i] != wordTags[j]) {
                    newTags.push_back(wordTags[j]);
                }
            }
            wordTags.swap(newTags);
        } else {
            wordTags.push_back( tagRuleTags[i] );
        }

    }

    return true;
}

bool AnalyzeEngine::applyTagsToWord(std::vector<int>& wordTags, const Cell* tagRuleTagSet, std::vector<int>& deletedTags) const
{
    size_t i,j;
    const SVector<int>& tagRuleTags = tagRuleTagSet->getContent();

    for(i=0; i < tagRuleTags.size(); i++) {

        if( tagRuleTagSet->getNegativeTags()[i] ) {
            std::vector<int> newTags;
            for(j=0; j<wordTags.size(); j++) {
                if( tagRuleTags[i] != wordTags[j]) {
                    newTags.push_back(wordTags[j]);
                } else {
                    deletedTags.push_back(wordTags[j]);
                }
            }
            wordTags.swap(newTags);
        } else {
            wordTags.push_back( tagRuleTags[i] );
        }

    }

    return true;
}

void AnalyzeEngine::generateParamMatrix(std::vector<std::vector<std::string> >& vvIn) const
{
    size_t i,j,k;
    size_t size = vvIn.size();
    std::vector<std::vector<std::string> > vvOut,vvCache;

    size = vvIn[0].size();
    for(i=0; i<size; i++) {
        std::vector<std::string> v;
        v.push_back(vvIn[0][i]);
        vvOut.push_back(v);
    }
    size = vvIn.size();
    for(i=1; i<size; i++) {
        size_t sizeCache = vvOut.size();
        vvCache = vvOut;
        for(j=0; j<sizeCache; j++) {
            vvOut[j].push_back(vvIn[i][0]);
        }
        size_t sizeI = vvIn[i].size();
        for(j=1; j<sizeI; j++) {
            for(k=0; k<sizeCache; k++) {
                vvCache[k].push_back(vvIn[i][j]);
                vvOut.push_back(vvCache[k]);
                vvCache[k].pop_back();
            }
        }
    }

    vvIn.swap(vvOut);
}

bool AnalyzeEngine::intersection(int& a1,int& a2, const int& b1, const int& b2) const
{
    if(a2 < b1 || a1 > b2) {
        return false;
    } else if(a1<b1 && a2>=b1 && a2<=b2) {
        a1 = b1;
    } else if(a1>=b1 && a1<=b2 && a2>b2) {
        a2 = b2;
    } else if(a1<b1 && a2>b2) {
        a1 = b1;
        a2 = b2;
    } else {
    }
    return true;
}


}//namespace sae
