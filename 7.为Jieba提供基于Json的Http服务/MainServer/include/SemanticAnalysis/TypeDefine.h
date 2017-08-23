#ifndef __SAE_TYPE_DEFINE_H__
#define __SAE_TYPE_DEFINE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <map>
#include "objio.h"
#include "giop.h"
#include "graphio.h"
#include "Config.h"

namespace sae
{

typedef struct _LINE {
    int m_lineNum;
    std::string m_line;
} LINE;

class FILE
{
public:
    std::string m_name;
    std::vector<LINE> m_file;
};

enum DIRECTION {
    NO_DIRECTION = 0,
    DIRECTION_BEFORE,
    DIRECTION_AFTER
};

class FormulaString
{
public:
    bool m_isIntersection;
    bool m_isKeyType;
    std::string m_sFormula;
public:
    void dump(std::ostream& ofs = std::cout) const {}
};

typedef struct INT_RANGE {
    int m_start;
    int m_end;
} IntRange;

typedef struct FUNC_PARSE_RESULT {
    int m_name;
    std::string m_params;
} FuncParseResult;

enum DATABASE_TYPE {
    WORD_DATABASE,
    TAG_DATABASE,
    OTHER_DATABASE
};

#ifdef WIN32
#define BRIGHT_GREEN ""
#define BRIGHT_RED ""
#define RESET_COLOR ""
#else
#define BRIGHT_GREEN "\033[1;32m"
#define BRIGHT_RED "\033[1;31m"
#define RESET_COLOR "\033[0m"
#endif

//begin---defination for DefineRule----
const char BASIC_DEFINITION       = 0x01;
const char CONDITIONAL_DEFINITION = 0x02;
const char KEY_DEFINITION         = 0x04;
//end---defination for DefineRule------

#define TheMaxOf(a,b) (a)>(b) ? (a):(b)
#define TheMinOf(a,b) (a)<(b) ? (a):(b)

//begin---defination for Cell and VCell---
const char CELL_TYPE_ERASER = 0x0f;
const char NORMAL_CELL      = 0x10;
const char	REFER_KEY_CELL  = 0x20;
const char BASIC_WORD_CELL = 0x40;
const unsigned char NEGATIVE_SET = 0x80;

const unsigned char RELATION_ERASER = 0xf0;
const char AND_RELATION             = 0x01;
const char CONTINUOUS_RELATION      = 0x02;
const char OR_RELATION              = 0x04;
const char SHUFFLE_RELATION         = 0x08;
//end---defination for Cell and VCell-----

}//namespace sae

#endif//__MEAN_TYPE_DEFINE_H__
