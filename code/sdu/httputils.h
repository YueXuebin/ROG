#pragma once

#ifndef HTTPUTILS_H
#define HTTPUTILS_H

#include <string>

namespace Crown
{
//std::string UrlEncode(const std::string &orig);

//std::string CharToHex(char ch);

std::string UrlEncode(const std::string& str);
std::string UrlDecode(const std::string& str);

std::string Base64Encode(const std::string s);
std::string Base64Encode(unsigned char const* , unsigned int len);
std::string Base64Decode(std::string const& s);
}

#endif // HTTPUTILS_H
