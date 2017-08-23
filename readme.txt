注意:文件的包含顺序不准确，需要调整

1.动态分词:使用Trie树，目录为:/SentenceParser/DynamicParser,目录下的main文件为可执行文件


2.include是相关的头文件

3.tools介绍:
	一期需求：将运营数据的语义需求用规则生成,工具自动化标注，标注出规则句式、关键词、及关键词的标签。
	二期需求:推荐相似句式。
	三期需求:(1)支持同一个词对应不同的标签  (2)支持对文件中的规则进行聚类(在tools\tools下) (3)优化了推荐方法，修复bug,使推荐内容更加合理

4.TextRank:文本摘要功能,可执行文件:TextRank/main

5.百度LDA开源Femilia源码及详细注释
	(1)LDA学习:
	http://blog.csdn.net/a123456ei/article/details/22767429
	(2)Femilia为源码，针对run_query_doc_sim_demo.sh短长文本进行匹配计算的功能进行了详细备注
	
6.E代驾智能客服知识库:为客服人员提供知识库搜索功能,按照"标签">"标题">"正文"的优先级进行排序

7.为Jieba提供基于json-rpc 2.0的http服务

8.语义解析:附有源码,规则案例,效果演示


