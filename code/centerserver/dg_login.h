#ifndef dg_login_h__
#define dg_login_h__

// 测试登陆模式
void VerifyTestModeLogin(uint32 centGateID,  uint64 loginID, const std::string& token, uint32 addrIP, uint16 addrPort);

// 动艺网站登陆模式
void VerifyAfModeLogin(uint32 gateSvrID,  uint64 loginID, const std::string& token, uint32 addrIP, uint16 addrPort);
void OnVerifyAfModeLogin(uint32 gateSvrID,  uint64 loginID, const std::string& account, uint32 addrIP, uint16 addrPort);


#endif // dg_login_h__
