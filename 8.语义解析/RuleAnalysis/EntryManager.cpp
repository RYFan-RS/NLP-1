#include "EntryManager.h"

namespace sae
{

bool EntryManager::makeEntries()
{

    size_t i,j,k;

    clearEntries();

    std::vector<NormalRule>& cacheRules = mp_normalRuleManager->getRules();

    for(i=0; i<cacheRules.size(); i++) {
        ENTRY entry;
        bool do_once = false;
        entry.m_index = i;
        entry.m_indexRuleID = cacheRules[i].getIdx();
		entry.m_fileLineIdx = cacheRules[i].getLineIndexID();
        KeyCell keyCell = cacheRules[i].getKeyPart().getKeys()[0];
        const SearchMode searchMode = keyCell.getSearchMode();
        SVector<int> entryCodes;
        SVector<bool> negativeTags;

        //word entry map
        if(searchMode.hasWordSearchType()) {
            const WordLimit& wordLimit = keyCell.getConstWordLimit();
            //forward entry map
            for(j=0; j<wordLimit.size(); j++) {
                entryCodes = wordLimit[j]->getEntryCodes();
                negativeTags = wordLimit[j]->getNegativeTags();
                for(k=0; k<entryCodes.size(); k++) {
                    if(!negativeTags[k]) {
                        m_wordEntries.insert(std::make_pair(entryCodes[k],entry));
                        do_once = true;
                    }
                }
            }
        }

        //tag entry map
        if(searchMode.hasTagSearchType() && do_once == false) {
            const TagLimit& tagLimit = keyCell.getTagLimit();

            //forward entry map
            for(j=0; j<tagLimit.size(); j++) {
                entryCodes = tagLimit[j]->getEntryCodes();
                negativeTags = tagLimit[j]->getNegativeTags();
                for(k=0; k<entryCodes.size(); k++) {
                    if(!negativeTags[k]) {
                        m_tagEntries.insert(std::make_pair(entryCodes[k],entry));
                        do_once = true;
                    }
                }
            }
        }

        // tag rule map, this tag is the tags in the FLAG rule, which is the result of the normal rule
        if(cacheRules[i].hasTagRule()) {
            const TagLimit& tagLimit = cacheRules[i].getTagRule().getTagSets()[0].getTagLimit();

            for(j=0; j<tagLimit.size(); j++) {
                entryCodes = tagLimit[j]->getEntryCodes();
                negativeTags = tagLimit[j]->getNegativeTags();
                for(k=0; k<entryCodes.size(); k++) {
                    if(!negativeTags[k]) {
                        m_tagRuleIdxMap.insert(std::make_pair(entryCodes[k],entry));
                    }
                }
            }
        }
    }

    return true;
}

void EntryManager::clearEntries()
{
    m_wordEntries.clear();
    m_tagEntries.clear();
}

void EntryManager::dump(std::ostream& ofs) const
{
    ofs<<"Word Entry: "<<std::endl;
    m_wordEntries.dump(mp_dataManager->getWordManager(),mp_dataManager->getLineIndexManager(),ofs);
    ofs<<"Tag Entry: "<<std::endl;
    m_tagEntries.dump(mp_dataManager->getTagManager(),mp_dataManager->getLineIndexManager(),ofs);
}

}//namespace sae
