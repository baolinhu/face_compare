#ifndef _COMON_FUNCTION_EE4EC4A3_71CE_4305_A386_4DADD0C80028_
#define _COMON_FUNCTION_EE4EC4A3_71CE_4305_A386_4DADD0C80028_

#include<Windows.h>
#include <iostream> 
#include <string>
#include <vector>
#include <xstring>
using namespace std;

static const std::string base64_chars =   
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"  
"abcdefghijklmnopqrstuvwxyz"  
"0123456789+/";

// Ã·π©CString ∞Ê±æ...
wstring GetBase64Image(wstring strImg);

string GetImg2PngBase64(string strImg);

string GetBase64Img(string strImg);

string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);

unsigned char ToHex(unsigned char x);

string UrlEncode(const std::string& str);

wstring string2wstring(const string &str);

string wstring2string(const wstring &wstr);

string Utf82Unicode(const string& utf8string);

wstring Utf82UnicodeW(const wstring& utf8string);

string Unicode2Utf8(const string& instring);


#endif //_COMON_FUNCTION_EE4EC4A3_71CE_4305_A386_4DADD0C80028_