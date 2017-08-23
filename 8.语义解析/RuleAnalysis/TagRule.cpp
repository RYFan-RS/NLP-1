#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "CommonParser.h"
#include "TagRule.h"

namespace sae
{

bool TagRule::parse(const std::string& rule, DataManager* pDataManager, ErrorData& errData)
{
    std::string sRule = rule.substr(1,rule.length()-2);
    if( CommonParser::checkElementsNumber(sRule,MeanTagRuleElementsNumber) == false ) {
        ERROR_MESSAGE( "The elements number of tag rule should be "+Utility::int2Str(MeanTagRuleElementsNumber)+"." )
    }

    std::vector<std::string> v =  Utility::divide(sRule,"\t");
    if( Utility::hasEmptyCell(v) > -1 ) {
        ERROR_MESSAGE( "There is empty element in tag rule `"+sRule+"'." )
    }

    //divide the tags
    std::vector<std::string> vTagSets = Utility::divide(v[1],";");
    if( Utility::hasEmptyCell(vTagSets) > -1 ) {
        ERROR_MESSAGE( "There is empty tag cell in tag string `"+v[1]+"'." )
    }

    for(size_t i=0; i<vTagSets.size(); i++) {
        TagRuleCell cell;
        int ret;

        //read tag cell
        std::vector<std::string> vTag = CommonParser::divideKeyAndContent(vTagSets[i]);
        ret = Utility::hasEmptyCell(vTag);
        if( ret > -1 ) {
            if(ret == 0) {
                ERROR_MESSAGE( "The key range define string in tag string `"+vTagSets[i]+"' is empty." )
            } else {
                ERROR_MESSAGE( "The tag string in tag string `"+vTagSets[i]+"' is empty." )
            }
        }
        if( vTag.size() != 2) {
            ERROR_MESSAGE( "The tag rule string `"+vTagSets[i]+"' should only have two parts." )
        }
        if(vTag[1].find("/")!=std::string::npos) {
            ERROR_MESSAGE( "The @TAG tag string `"+vTag[1]+"' cannot contains OR separators (/)." )
        }

        //read key range
        ERROR_TRACE( cell.m_keyRange.parse(vTag[0],errData) )

        //read tags
        ERROR_TRACE( cell.m_tagLimit.parse(vTag[1],pDataManager,errData) )

        //record the cell
        m_tagSets.push_back(cell);
    }

    return true;
}

bool TagRule::preProcess(const std::string& rule,std::set<std::string>& vTags, ErrorData& errData) const
{
    std::string sRule = rule.substr(1,rule.length()-2);
    if( CommonParser::checkElementsNumber(sRule,MeanTagRuleElementsNumber) == false ) {
        ERROR_MESSAGE( "The elements number of tag rule should be "+Utility::int2Str(MeanTagRuleElementsNumber)+"." )
    }

    std::vector<std::string> v =  Utility::divide(sRule,"\t");
    if( Utility::hasEmptyCell(v) > -1 ) {
        ERROR_MESSAGE( "There is empty element in tag rule `"+sRule+"'." )
    }

    //divide the tags
    std::vector<std::string> vTagSets = Utility::divide(v[1],";");
    if( Utility::hasEmptyCell(vTagSets) > -1 ) {
        ERROR_MESSAGE( "There is empty tag cell in tag string `"+v[1]+"'." )
    }

    for(size_t i=0; i<vTagSets.size(); i++) {
        //TagRuleCell cell;
        int ret;

        //read tag cell
        std::vector<std::string> vTag = CommonParser::divideKeyAndContent(vTagSets[i]);
        ret = Utility::hasEmptyCell(vTag);
        if( ret > -1 ) {
            if(ret == 0) {
                ERROR_MESSAGE( "The key range define string in tag string `"+vTagSets[i]+"' is empty." )
            } else {
                ERROR_MESSAGE( "The tag string in tag string `"+vTagSets[i]+"' is empty." )
            }
        }
        if( vTag.size() != 2) {
            ERROR_MESSAGE( "The tag rule string `"+vTagSets[i]+"' should only have two parts." )
        }
        if(vTag[1].find("/")!=std::string::npos) {
            ERROR_MESSAGE( "The @TAG tag string `"+vTag[1]+"' cannot contains OR separators (/)." )
        }

        //read key range
        //ERROR_TRACE( cell.m_keyRange.parse(vTag[0],errData) )

        //read tags
		if(vTag[1].at(0) =='!')
		{
			vTags.insert(vTag[1].substr(1));			
		}
		else
		{
			vTags.insert(vTag[1]);				
		}

        //ERROR_TRACE( cell.m_tagLimit.parse(vTag[1],pDataManager,errData) )

        //record the cell
        //m_tagSets.push_back(cell);
    }

    return true;
}


void TagRuleCell::dump(const DataManager& dataManager, std::ostream& ofs) const
{
    m_keyRange.dump(ofs);
    ofs<<":";
    m_tagLimit.dump(dataManager,ofs);
}

void TagRule::dump(const DataManager& dataManager, std::ostream& ofs) const
{
    ofs<<"(@TAG";
    ofs<<DUMP_SEPARATOR;
    for(size_t i=0; i<m_tagSets.size(); i++) {
        m_tagSets[i].dump(dataManager,ofs);
        if( i != m_tagSets.size()-1 ) {
            ofs<<";";
        }
    }
    ofs<<")";
}

bool TagRule::isKeyOutOfRange(const std::vector<int>& lengthMatrix, ErrorData& errData) const
{
    for(size_t i=0; i<m_tagSets.size(); i++) {
        if( CommonParser::isOutOfRange(m_tagSets[i].m_keyRange.m_startKeyNum,lengthMatrix.size()-1) ) {
            ERROR_MESSAGE( "The start key number of tag "+Utility::int2Str(i)+" is out of the key index range." )
        }

        if(m_tagSets[i].m_keyRange.m_isRange == true) {
            if( CommonParser::isOutOfRange(m_tagSets[i].m_keyRange.m_endKeyNum,lengthMatrix.size()-1) ) {
                ERROR_MESSAGE( "The end key number of tag "+Utility::int2Str(i)+" is out of the key index range." )
            }
        }
    }

    return true;
}

bool TagRule::checkKeyRangesConflictWithOmissilbeKeys(const std::set<int>& setOmissibleKey)
{
    for(size_t i=0; i<m_tagSets.size(); i++) {
        if(m_tagSets[i].getKeyRange().ifIncludeOmissibleKeyIntSet(setOmissibleKey) == true) {
            return false;
        }
    }

    return true;
}

}//namespace sae
