# 源码注释规范与 reference v2

一个条目是一种学生可以独立查找的操作；同一操作的 int、ll、double 变体放在一起。源码文件按实现组织，与标签无关。公开符号使用 @public，直接依赖使用 @requires，@title 创建条目，@id 默认取声明符号名；@members 声明同条目下的公共符号，@variant KEY SYMBOLS 声明数据类型变体。函数签名从真实声明提取，不重复维护。

@tags 使用有限词表（见构建器），1–4 个；@keywords 用于自然语言检索。@summary 为一句用途说明。@param NAME TEXT、@return、@pre、@error、@complexity 为结构化字段，适用于语义明确的参数和行为；其余细节保留在正文。@example 之后为多行代码，支持 {{type}}、{{fn1}}，以及 @example KEY、@note KEY。@related 仅引用存在的条目 ID。不要嵌套块注释。不要为字段填充无意义信息或猜测跨变体的行为。

reference.json schemaVersion 2：version、notes、tags、entries；每个条目包括 id、title、summary、tags、description、keywords、related、signatures、example，以及适用的 parameters、returns、requirements、errors、complexity、variants、commonSignatures、advancedSignatures。参数名须出现在实际签名中，标签须来自受控词表。源码注释为唯一业务信息来源；GUIDE.md 为全局约定。manifest v2 不改变。

每次修改接口同步更新相关说明和示例。编译全部示例、逐个公开符号和依赖选取，并执行回归测试。对于需要若干接口共同完成的操作（如前缀和的建表与查询），可在一个条目中保留完整工作流。
