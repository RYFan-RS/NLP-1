1.semantic.txt是规则文件

2.semantic_test.txt是测试例子,用来检测规则是否准确
	测试格式:标签	句子1	句子2

3.效果:效果1.PNG

4.源码:RuleAnalysis

5.可执行文件:RuleAnalysis/main

6.以下为代码框架笔记

原始规则举例:
(#INDEX	什么是银联钱包？	1)
(#TAG	什么是银联钱包？	事件/输出	1)

(#RULE	什么是银联钱包？	(W	*	*	银联	*)(W	+	*	钱包	*)(W	+/-	*	是/指	*)(W	+/-	*	什么/啥/那种/哪种/哪一种/那一种/怎样的/怎么样/咋样的	*))(@TAG	[S]:什么是银联钱包？)
(#RULE	什么是银联钱包？	(W	*	*	银联	*)(W	+	*	钱包	*)(W	+/-	*	定义/含义/简介/介绍/解释/意思/概念/特点	*))(@TAG	[S]:什么是银联钱包？)
(#RULE	什么是银联钱包？	(W	*	*	银联	*)(W	+	*	钱包	*)([LW	-	*	$(单句无意义的词)	*])([LW	[1]+	*	$(单句无意义的词)	*]))(@TAG	[S]:什么是银联钱包？)

语义解析系统代码框架理解:过程非常繁琐，只写一个大概
	编译:将txt文件编译成dat文件
		1.readDataFile:读取文件
			A.记录每一行,并对每一行的内容进行MD5加密,加密与原始文件分别保存
		2.parseDataFile
			A.getLineType解析本行的规则类型,分为#INDEX,#TAG,#RULE,#DEFINE等
			B.IndexManager::parse解析#INDEX,将"什么是银联钱包？"与1进行保存在mc_lineMap
			C.TagManager::parse解析#TAG,将"什么是银联钱包？"与1进行保存在mc_lineMap,将“事件”与1保存在m_attributeMap
			D.NormalRuleManager::parse解析#RULE,信息保存在数据结构Cell中
				(1)divideRuleLine将RULE切分为(#RULE...)和(@TAG	[S]:什么是银联钱包？)两部分
				(2)NormalRule::parse,getKeyFromString支持获取"$(怎样)"同义词,同时也是将(W	*	*	银联	*)这样的内容提取出来
				(3)readKeyString:获取搜索模式为"W",getPositionFromString获取指定的位置为"*"
				
		3.makeStorage将内存中的数据写入二进制dat文件中。貌似是GIOP_format
			serialManager.writeModule(m_storage,"Storage")
			serialManager.writeModule(m_storage.getDataManager().getTagManager(),"TagManager")
	
	单元测试:
		1.ContextDictionary::init从dat格式的文件中将数据读入内存
			readModule(m_data.getDataManager().getTagManager(),"TagManager")
			readModule(m_data,"Storage")
		2.将规则文件中所有的词汇加载到动态分词系统中。
			parser.init("DynamicDic",mae->getData().getConstWordManager().getWords())
		3.解析,得到语义标签与待测试内容
			语义标签:怎么使用扫码支付？
			待测试内容:二维码怎么支付
		4.对”二维码怎么支付“进行分词,如果在规则文件中没有出现的词汇都会被切分成一个字
		5.ContextDictionary::analyze进行规则匹配
			AnalyzeEngine::analyzePhase
				AnalyzeEngine::findRulesWithWordAndAnalyze
					所有规则的入口条件都在wordEntries中,假如存在”银联“这样的词,那么把这样所有的规则找出来
					然后解析第二个括号(W	+	*	钱包	*),解析银联后面是否出现钱包关键字。如果通过则继续解析...,找出所有满足条件的rule
