#ifndef __SAE_CONFIG_H__
#define __SAE_CONFIG_H__
#if defined(MEAN_DEBUG) || defined(DUMP_HIT_RULE)
#include <sstream>
#include "lib4/Debug.hpp"
static std::stringstream ssDebug;
#endif //MEAN_DEBUG
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32

namespace sae
{

#ifdef __LANG_CHN__
#define WORD_SEPARATOR ""
#elif __LANG_ENG__
#define WORD_SEPARATOR " "
#else
#define WORD_SEPARATOR " "
#endif

#define RULE_INDEX_MAX_NUMBER 100000

#ifdef WIN32
#define ERR_STREAM	"D:\\SAE_DEBUG_LOG.log"
#define MSG_STREAM	"D:\\SAE_DEBUG_LOG.log"
#else
#define ERR_STREAM	"SAE_DEBUG_LOG.log"
#define MSG_STREAM	"SAE_DEBUG_LOG.log"
#endif//WIN32
#define DUMP_SEPARATOR	"  "
#define RULE_INDEX_MAX	10000
#define IndexRuleElementsNumber 3
#define IndexRuleFileLineElementsNumber 2
#define DefineRuleElementsNumber 2
#define TagDefineRuleElementsNumber 4
#define TagDefineFileLineElementsNumber 3
#define ExecuteMethodRuleElementsNumber 3
#define ExecuteMethodFileLineElementsNumber 2
#define WholePartElementsNumber 2
#define KeyElementsNumber 5
#define MeanTagRuleElementsNumber 2

#define TAIL 99999
#define TAIL_RANGE 99000
#define WHOLE_SENTENCE 100000

#define __ENCRYPT_DATA__
}//namespace sae

#endif //__MEAN_CONFIG_H__
