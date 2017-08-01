#ifndef string_parser_h__
#define string_parser_h__

namespace CommonLib
{
// 寻找字符串
void FindStrings(const char* pattern, const char* inString, std::vector<std::string>& outString);

// 字符串转小写
void ToLower(std::string& str);


}

#endif // string_parser_h__
