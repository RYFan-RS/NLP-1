代码框架及原理介绍:

1.知识库初始化:bool VKnowledgeBase::init(const acl::FileName& sFileName) 
	(1)initMySqlKB,从Mysql那里获取到标签,标题,正文,其中对正文部分分词后,建立倒排索引
	(2)initIDF对正文部分求IDF,对标题求TF-IDF(TfidfValue对应单词索引和TF-IDF值)
	(3)Initialize初始化word2vec的二进制文件
	
2.处理用户输入:ChatEngine::process
	(1)分词，计算输入的TF-IDF

3.对话:KnowledgeBase::chat,按照以下对话依次进行
	(1)searchByTags:用标签计算匹配度
		以空格做切分,从用户输入的内容中获取到标签,与每一个标题所在文章的标签求交集，nScore = 匹配标签数量*2/(用户输入文中标签数量+本文章标签数量),如果得分大于0.01则认为是匹配的
		匹配的插入setMatched,并加入最终结果vResult中
	(2)searchByTFIDF:求两个vector见的欧式距离的余弦相似度numerator/denominator
		numerator:v1和v2的TF-IDF值两两相乘然后累加,denominator:v1自己与自己相乘然后累加的结果开平方根,v2一样，然后两者相乘。
		匹配的插入setMatched,并加入最终结果vResult中
	(3)searchByPinyin:
		转化成拼音，求编辑距离
		匹配的插入setMatched,并加入最终结果vResult中
	(4)searchByInvertIndex:倒排索引
		1.根据单词,找到文章以及在文章中出现的次数
		2.在setMatched中找到对应文章中应该高亮的单词，以备后续发给Java端
		3.对于未在(1),(2),(3)中找到对应的文章的,开始对其正文进行处理,
				fScore = fScore*number*10+sameWordNumber*distance;
				fScore是单词的IDF值累加,IDF值越大，说明这个单词对应的分类效果越好，那么他属于这篇文章的可能性越高
				number是本次输入的单词数量,sameWordNumber是所有单词在本文章中出现的次数,distance为0.000001。
			对正文部分满足条件的的加入最终结果vResult
		4.将结果存储为HTML给Java