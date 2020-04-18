在Wolfram语言中连接时序型数据库TDengine，并执行SQL语句。 
12版本的Mathematica已经可以支持关系型数据库相关操作，但是还没有对时序型数据库TDengine的支持。 

执行make build进行编译,编译所需的库和工具在Mathematica安装目录下的SystemFiles/Links/Mathlink/。

以下符号都在TDengineLink`下。
**TDengineConnect[ip,user,pass,db,port]**    连接TDengine数据库。执行成功返回数据库连接的描述符，形式为TDengineDescriptor[id]，其中id为非负整数；执行失败返回错误信息。
	ip: 字符串类型，ip地址或域名。
	use: 字符串类型，数据库管理员账号名。
	pass: 字符串类型，数据库管理员密码。
	db: 字符串类型，数据库名。
	port: 整数类型，数据库连接使用的端口号。
**TDengineClientInfo[]**    返回描述TDengine客户端版本信息的字符串。
**TDengineServerInfo[cd]**    返回描述TDengine服务器端版本信息的字符串。
	cd: 数据库连接后返回的数据库连接描述符。形式为TDengineDescriptor[id]，其中id为非负整数。
**TDengineQuery[cd,sqlstr]**    同步执行SQL语句，执行失败返回错误信息。
	cd: 数据库连接描述符。
	sqlstr: 字符串类型，SQL语句字符串。
**TDengineClose[cd]**    关闭时序数据库连接cd。成功返回Null。对不存在的连接进行关闭操作或是关闭已经断开的连接将返回对应错误信息。
	cd: 数据库连接描述符。