#ifndef base64_encode_h__
#define base64_encode_h__

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif // base64_encode_h__
