#ifndef kugou_login_h__
#define kugou_login_h__


// kugou平台登陆
void VerifyKugouModeLogin(uint32 gateSvrID,  uint64 loginID, const std::string& token, uint32 addrIP, uint16 addrPort);

std::string  GetKugouKgext(uint64 loginID);
void RemoveKugouKgext(uint64 loginID);

#endif // kugou_login_h__
