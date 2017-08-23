#ifndef __SAE__MESSAGE_H__
#define __SAE__MESSAGE_H__
#include <iostream>
#include <sstream>
#include <string>
#define __LANG_CHN

namespace sae
{

class Message{
public:
	static std::string msg001(const std::string& filename){
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "无法打开文件'"<< filename <<"'。";
#else
		ss << "Can't open file `" << filename <<"'.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg002(const std::string& line,
		const std::string& filename, int lineNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "文件'"<< filename <<"'第"<<lineNum<<"行:'"<<line<<"'含有非UTF8字符。";
#else
		ss << "Line contains non-UTF character(s): '" << line <<"' in " << filename << ":" << lineNum << " .";
#endif //__LANG_CHN
		
		return ss.str();
	} 
	
	static std::string msg003(const std::string& line,
		const std::string& filename, int lineNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "无法从文件'"<< filename <<"'第"<<lineNum<<"行:'"<<line<<"'获取引用的文件名。";
#else
		ss << "Can't read the file name from an include line `" << line <<"' at " << filename << ":" << lineNum << " .";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg004(const std::string& filename)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "生成文件'"<< filename <<"'的版本字符串出错。";
#else
		ss << "Generate the version number of file `" << filename <<"' error.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg005()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "类TagManager序列化失败。";
#else
		ss << "TagManager serialization failed.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg006()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "类Storage序列化失败。";
#else
		ss << "Storage Serialization failed.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg007()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "类AllocationTable序列化失败。";
#else
		ss << "AllocationTable Serialization failed.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg008(const std::string& type)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "未知的行类型'"<< type <<"'。";
#else
		ss << "Meet an unknown line type `" << type <<"'. ";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg009()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "行首和行尾的括号不匹配。";
#else
		ss << "The head and tail brackets are not matched.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg010()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "行内的括号不配对";
#else
		ss << "The brackets are not matched.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg011()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "无法识别该行的类型";
#else
		ss << "Can't recognize the type of the line.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg012()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "合法的标签序号应该仅为自然数。";
#else
		ss << "The tag number should be a positive number.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg013(const std::string& tag)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "标签`" << tag <<"'已经被定义过，请更改标签名称。";
#else
		ss << "The tag `"<< tag <<"' has been used.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg014(const std::string& sTagNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "标签序号`" << sTagNum <<"'已经被定义过，请更改标签序号。";
#else
		ss << "The tag number `"<< sTagNum <<"' has been used.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg015()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "切分规则行时出错。";
#else
		ss << "Received an empty result when getting the simple rule string.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg016()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "规则行的元素超过了3对。";
#else
		ss << "The return result of getting the simple rule string is over 3.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg017()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "规则的名称不应为空。";
#else
		ss << "The name of the rule shouldn't be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg018(const std::string& key)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "条件`"<<key<<"'的定义缺失。";
#else
		ss << "Key definition `" << key << "' must precede its use.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg019(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "在读取条件`"<<keyNum<<"'时发生了未知错误。";
#else
		ss << "An unknown error occured when getting key `" << keyNum << "' from string.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg020(int elementNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "规则的头部元素个数应为"<<elementNum<<"个。";
#else
		ss << "The elements number of whole part should be "<<elementNum<<".";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg021(const std::string& name)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "规则`"<<name<<"'的定义缺失。";
#else
		ss << "The rule with name `"<<name<<"' hasn't been defined in index rules";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg022(const std::string& type)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "未知的规则类型`"<<type<<"'。";
#else
		ss << "Unknown rule type `"<<type<<"'.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg023()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "不能在范围型的位置定义中引用可省略条件。";
#else
		ss << "The refer of omissible keys can't be used in range type.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg024()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss << "标签规则错误：";
#else
		ss << "TagRule error: ";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg025(int keyNum, const std::string& key, int elementNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"第"<<keyNum<<"个条件`"<<key<<"'应该有"<<elementNum<<"个元素。";
#else
		ss << "The elements number of KEY"<<keyNum<<":`"<<key<<"' should be "<<elementNum<<".";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg026(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的查找方法不应该为空";
#else
		ss << "The search method of the KEY"<<keyNum<<" shouldn't be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg027(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的查找位置不应该为空";
#else
		ss << "The search position of the KEY"<<keyNum<<" shouldn't be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg028(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的字符条件不应该为空";
#else
		ss << "The charactor limit of the KEY"<<keyNum<<" shouldn't be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg029(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的单词条件不应该为空";
#else
		ss << "The word limit of the KEY"<<keyNum<<" shouldn't be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg030(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的标签条件不应该为空";
#else
		ss << "The Flag limit of the KEY"<<keyNum<<" shouldn't be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg031(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"入口条件，即条件"<<keyNum<<"不能是可省略条件";
#else
		ss << "The entry KEY"<<keyNum<<" can't be weak deny type.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg032(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的位置条件不能为`*'";
#else
		ss << "The position limitation of the KEY "<<keyNum<<" shouldn't be `*'.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg033(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"错误：";
#else
		ss << "Key"<<keyNum<<" error: ";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg034(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"根据查找条件的设定，条件"<<keyNum<<"的单词条件应该为空";
#else
		ss << "As the limitation of the search type, the word limitation of KEY"<<keyNum<<" should be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg035(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的单词条件不应该含有孤立的逗号";
#else
		ss << "The word limitation of KEY"<<keyNum<<" should not contain commas connected to words.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg036(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"根据查找条件的设定，条件"<<keyNum<<"的单词条件不应为空";
#else
		ss << "As the limitation of the search type, the word limitation of KEY"<<keyNum<<" shouldn't be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg037(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"根据查找条件的设定，条件"<<keyNum<<"的标签条件应该为空";
#else
		ss << "As the limitation of the search type, the tag limitation of KEY"<<keyNum<<" should be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg038(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的标签定义";
#else
		ss << "In KEY"<<keyNum<<", the definition of tag ";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg039(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"根据查找条件的设定，条件"<<keyNum<<"的标签条件不应为空";
#else
		ss << "As the limitation of the search type, the tag limitation of KEY"<<keyNum<<" shouldn't be empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg040(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"由于入口条件的所有条件均为否定，故无法制作入口索引。";
#else
		ss << "Can't make entry to this rule for all the elements in KEY0 are negative type.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg041(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"入口条件不能是可省略条件。";
#else
		ss << "The entry KEY can't be a weak deny KEY.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg042(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的位置条件不能只引用可省略条件。";
#else
		ss << "KEY"<<keyNum<<" error: Omissible keys cannot be the sole reference in the position condition.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg043(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的单词条件不能只引用可省略条件。";
#else
		ss << "KEY"<<keyNum<<" error: Omissible keys cannot be the sole reference in the word condition.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg044(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"引用的查找位置超过了句子的长度。";
#else
		ss << "KEY"<<keyNum<<" error: Refers to position beyond the end of the sentence.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg045(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"引用的查找位置小于了句子的起始位置。";
#else
		ss << "KEY"<<keyNum<<" error: Refers to position before the beginning of the sentence.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg046()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"使用短语搜索模式去搜索单个位置是无意义的。";
#else
		ss << "It is meaningless to use PhraseMode to search single position at KEY.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg047()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"使用连续搜索模式去搜索单个位置是无意义的。";
#else
		ss << "It is meaningless to use ContinuousMode to search a single position at KEY.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg048(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的单词条件只能引用序号小于它的条件。";
#else
		ss << "In KEY"<<keyNum<<" in word definition, a refer key must refer to a key before itself.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg049(int keyNum)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<keyNum<<"的标签条件只能引用序号小于它的条件。";
#else
		ss << "In KEY"<<keyNum<<" in tag definition, a refer key must refer to a key before itself.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg050()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"在单个位置查找连续单词条件是无意义的。";
#else
		ss << "It is meaningless to search continuous word condition at a single position at KEY.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg051()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"在单个位置查找连续标签条件是无意义的。";
#else
		ss << "It is meaningless to search continuous tag condition at a single position at KEY.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg052(const std::string& key)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"无效条件引用表达式`"<<key<<"'。";
#else
		ss << "String `"<<key<<"' does not seem to be a valid KEY.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg053(const std::string& key)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件引用表达式`"<<key<<"'的格式错误：超过了两个元素。";
#else
		ss << "KEY range string `"<<key<<"' format error, key elements over 2.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg054(const std::string& key)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件引用表达式`"<<key<<"'的格式错误：需要同时有条件引用数字和字符引用数字。";
#else
		ss << "KEY range string `"<<key<<"' format error, both key number and sub-key number are required.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg055(const std::string& key)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件引用表达式`"<<key<<"'的格式错误：条件引用数字必须为自然数，或H,T,S。";
#else
		ss << "KEY range string `"<<key<<"' format error, key number must be a natural number.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg056(const std::string& key)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件引用表达式`"<<key<<"'的格式错误：字符引用数字必须为自然数，或H,T。";
#else
		ss << "KEY range string `"<<key<<"' format error, sub-key number must be a natural number.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg057(const std::string& key)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件引用表达式`"<<key<<"'的格式错误：表达式为空字符串。";
#else
		ss << "KEY range string `"<<key<<"' format error, meet an empty KEY string.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg058()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"否定符`!'不能单独出现。";
#else
		ss << "Met an empty negative string.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg059(int tagLen, int contentLen)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"否定标签数组的长度和内容数组的长度不符。";
#else
		ss << "The length of NegativeTags and Content do not match. NegativeTags: "<<tagLen<<" Content: "<<contentLen;
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg060(const std::string& keyword)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"定义`"<<keyword<<"' 应该为基本定义或者条件定义。";
#else
		ss << "Definition `"<<keyword<<"' supposed to be basic definition or conditional definitionm, but it isn't.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg061(const std::string& keyword)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"`"<<keyword<<"' 尚未被定义。";
#else
		ss << "`"<<keyword<<"' must precede its use.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg062(const std::string& keyword)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"定义`"<<keyword<<"'中含有小写字母。";
#else
		ss << "The content of definition `"<<keyword<<"' has lower case alphabet.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg063(const std::string& keyword)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"在set`"<<keyword<<"'内部";
#else
		ss << "(in set `"<<keyword<<"') ";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg064(const std::string& keyword)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"无法从字符串`"<<keyword<<"'中读取变量名";
#else
		ss << "Can't get extend keyword from string `" << keyword << "'.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg065(const std::string& keyword)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"`"<<keyword<<"'尚未定义";
#else
		ss << "`" << keyword << "' must precede its use.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg066()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"碰到了一个空的条件。";
#else
		ss << "There is an empty condition cell.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg067()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"不允许出现不同种类的关系符。";
#else
		ss << "Mixed combine type is not allowed.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg068()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"`&'和`;'不能同时出现";
#else
		ss << "`&' and `;' mark can't be used at the same time.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg069(const std::string& keyword)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"定义`"<<keyword<<"' 应该为基本定义";
#else
		ss << "Definition `" << keyword << "' supposed to be basic definition, but it isn't.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg070(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"元素数量应为`"<<n<<"个。";
#else
		ss << "The elements size should be " << n << " .";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg071(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"第"<<n<<"个元素是空的。";
#else
		ss << "The elements " << n << " is empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg072(int n, const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"字符串`"<<s<<"'中的第"<<n<<"个元素是空的。";
#else
		ss << "The elements "<<n<<" in string `"<<s<<"' is empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg073()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"在基本定义中使用了功能符号`/'";
#else
		ss << "Using function mark `/' in basic definition.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg074()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"在基本定义中使用了功能符号`|'";
#else
		ss << "Using function mark `|' in basic definition.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg075(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"获取`"<<s<<"'的同义词扩展失败。";
#else
		ss << "Get synonym extend keyword of `"<<s<<"' error.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg076(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"`"<<s<<"'是一个基本定义，基本定义只能包含基本定义或者单词。";
#else
		ss << "`"<<s<<"' is not a basic defination, only basic definition can be used in basic definition rule.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg077(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"索引`"<<s<<"'尚未被定义。";
#else
		ss << "The index definition `"<<s<<"' must precede its use.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg078()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"定义规则的格式错误。";
#else
		ss << "The format of the content of the define rule is wrong.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg079(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"定义`"<<s<<"'已经被占用。";
#else
		ss << "Key `" << s << "' has already been used.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg080(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"定义规则的元素个数应该为"<<n<<"个。";
#else
		ss << "The elements number of define rule should be "<<n<<" .";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg081()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"定义规则为空。";
#else
		ss << "The define rule is empty.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg081(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"无意义的表达式`"<<s<<"'。";
#else
		ss << "Expression `"<<s<<"' seems without meaning.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg082(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"非法的表达式`"<<s<<"'。";
#else
		ss << "Expression `"<<s<<"' is not allowed.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg083(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"表达式`"<<s<<"'表示了一个非法的位置。";
#else
		ss << "Expression `"<<s<<"' directs to an impossible position.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg084()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"查找位置格式错误。";
#else
		ss << "Position formula format error.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg085(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"引用的位置超过了当前的位置。";
#else
		ss << "The position string of KEY "<<n<<" is error: the KEY definition is bigger than current KEY.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg086()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"位置表达式`+/-[K:M]+/-'是无意义的。";
#else
		ss << "Position formula +/-[K:M]+/- has no meaning.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg087()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"位置表达式`N+/-M'是无意义的。";
#else
		ss << "Position formula N+/-M has no meaning.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg088(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<n<<"的查找位置包含空的表达式字符串。";
#else
		ss << "The position of KEY "<<n<<" contains empty formula string.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg089(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<n<<"的查找位置包含错误的的表达式字符串。";
#else
		ss << "The position of KEY "<<n<<" contains an improperly formatted string.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg090(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<n<<"错误：";
#else
		ss << "KEY "<<n<<" error: ";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg091(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<n<<"的查找条件是一个无意义的位置。";
#else
		ss << "The position of KEY "<<n<<" is impossible to intersect.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg092(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<n<<"的查找条件错误，+KEY和-KEY是错误的。";
#else
		ss << "The position string of KEY "<<n<<" is error: the +KEY and -KEY type is not allowed.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg093(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"切分条件"<<n<<"的查找条件字符串时出错。";
#else
		ss << "An unknown error occured while dividing the position string of KEY "<<n<<" .";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg094(const std::string& s, int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"无法识别条件"<<n<<"中的搜索模式`"<<s<<"'。";
#else
		ss << "Can't recognize search mode string  `"<<s<<"' in KEY "<<n<<".";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg095(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<n<<"中的搜索模式字符串中有空的表达式。";
#else
		ss << "Met an empty search mode cell in KEY "<<n<<".";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg096(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"条件"<<n<<"中的搜索模式不能同时使用短语和普通模式。";
#else
		ss << "It is meaningless to use PhraseMode and NormalMode together at KEY "<<n<<".";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg097()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"脚本条件语法错误。";
#else
		ss << "The script condition column syntax error.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg098(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"脚本条件使用语法错误：定义`"<<s<<"'应该是一个基本定义。";
#else
		ss << "The script condition is error : Definition `"<<s<<"' is supposed to be a basic definition, but it isn't.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg099()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"脚本条件错误。";
#else
		ss << "The ScriptLimit is wrong.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg100()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"检查脚本条件成功完成。";
#else
		ss << "Check the script limitation successfully finished.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg101()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"索引规则格式错误。";
#else
		ss << "Index rule format error.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg102()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"索引规则的索引序号必须为一个正整数。";
#else
		ss << "The index number should be a positive number.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg103(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"索引规则的名称`"<<s<<"'已经被占用。";
#else
		ss <<"The index name `"<<s<<"' has already been used.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg104(const std::string& s)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"索引规则的索引序号`"<<s<<"'已经被占用。";
#else
		ss <<"The index number `"<<s<<"' has already been used.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg105(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"索引规则的索引序号不能超过数字`"<<n<<"'。";
#else
		ss <<"The index number can't over the maximum number `"<<n<<"' .";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg106(int n)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"索引序号为`"<<n<<"'的索引规则从未被使用过。";
#else
		ss <<"The index number `"<<n<<"' has never been used.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg107(int start, int end, int loopTime)
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"执行环节`"<<start<<"-"<<end<<":"<<loopTime<<"'范围内没有任何的索引序号被定义。";
#else
		ss <<"The range `"<<start<<"-"<<end<<":"<<loopTime<<"' refers to a null INDEX.";
#endif //__LANG_CHN
		
		return ss.str();
	}
	
	static std::string msg108()
	{
		std::stringstream ss;
		
#ifdef __LANG_CHN
		ss <<"入口KEY的查找范围必须为*或者绝对位置";
#else
		ss <<"The search position of the entry key has to be `*' or absolute postion.";
#endif //__LANG_CHN
		
		return ss.str();
	}
};

}//namespace sae

#endif //__SAE__MESSAGE_H__
