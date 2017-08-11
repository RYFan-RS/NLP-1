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
	(2)searchByTFIDF:求两个vector见的欧式距离的余弦相似度numerator/denominator
		numerator:v1和v2的TF-IDF值两两相乘然后累加,denominator:v1自己与自己相乘然后累加的结果开平方根,v2一样，然后两者相乘。
		待解决问题:tfidfValues2是什么
	(3)searchByPinyin
	(4)searchByInvertIndex