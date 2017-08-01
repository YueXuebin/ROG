
#ifndef url_encode_h__
#define url_encode_h__



unsigned char ToHex(unsigned char x);
unsigned char FromHex(unsigned char x);

std::string UrlEncode(const std::string& str);          // ±àÂë
std::string UrlDecode(const std::string& str);          // ½âÂë


#endif // url_encode_h__