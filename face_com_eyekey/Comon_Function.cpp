#include "StdAfx.h"
#include "Comon_Function.h "
#include "Ole2.h"

//#include "GdiplusImaging.h"
DEFINE_GUID(ImageFormatJPEG, 0xb96b3cae,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatPNG, 0xb96b3caf,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);

wstring GetBase64Image(wstring strImg)
{
    //return string2wstring(GetBase64Img(wstring2string(strImg)));
    return string2wstring(GetImg2PngBase64(wstring2string(strImg)));//改用此接口, 可以将超过3M的BMP图片转成jpg,小多了
}

string GetImg2PngBase64(string strImg)
{
    CImage image;
    IStream *pStream = NULL;
    HGLOBAL hGlobal = NULL;
    image.Load(string2wstring(strImg).c_str());
    image.Save(pStream, ImageFormatPNG);
    CreateStreamOnHGlobal(NULL, TRUE, &pStream);
    image.Save(pStream,ImageFormatJPEG);
    GetHGlobalFromStream(pStream, &hGlobal); // 获取 IStream 的内存句柄
    LPBYTE pBits = (LPBYTE)GlobalLock(hGlobal); // pBits 就是存储JPEG内容的内存指针
    char *cResult = NULL;
    string strImage = base64_encode(pBits, GlobalSize(hGlobal));
    return UrlEncode(strImage); // 加号会变成空格导致解码失败, 需要Url编码一下
}

string GetBase64Img(string strImg)
{
    FILE* pRead = fopen(strImg.c_str(),"rb");
    BYTE* pbuffer = NULL;
    int nlen = 0;
    if (pRead) {
        fseek(pRead, 0, SEEK_END);//把文件的位置指针移到文件尾
        nlen = ftell(pRead);     //获取文件长度;
        rewind(pRead);          //SEEK_BEGIN

        if (FALSE && nlen > 3*1024*1024)
        {
            printf("FBI Warnning : Image too large\n");
            return "";
        
        }
        pbuffer = new BYTE[nlen+1];
        if (!pbuffer){
            fclose(pRead);
            return "";
        }
        ZeroMemory(pbuffer, nlen+1);

        int nReadLen = fread(pbuffer,sizeof(BYTE), nlen,pRead);
        if( nReadLen != nlen ){
            printf("Error: fread length != file length ! \n");
            fclose(pRead);
            delete[] pbuffer;
            pbuffer = NULL;
            return "";
        }

    }else{
        return "";
    }

    string strImage = base64_encode( pbuffer, nlen) ; //base64编码

    if (pbuffer)
    {
        delete[] pbuffer;
        pbuffer = NULL;
    }
    if (pRead)
    {
        fclose(pRead);
    }
    return UrlEncode(strImage); // 加号会变成空格导致解码失败, 需要Url编码一下
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{  
    std::string ret;  
    int i = 0;  
    int j = 0;  
    unsigned char char_array_3[3];  
    unsigned char char_array_4[4];  

    while (in_len--) {  
        char_array_3[i++] = *(bytes_to_encode++);  
        if (i == 3) {  
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;  
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);  
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);  
            char_array_4[3] = char_array_3[2] & 0x3f;  

            for(i = 0; (i <4) ; i++)  
                ret += base64_chars[char_array_4[i]];  
            i = 0;  
        }  
    }  

    if (i)  
    {  
        for(j = i; j < 3; j++)  
            char_array_3[j] = '\0';  

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;  
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);  
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);  
        char_array_4[3] = char_array_3[2] & 0x3f;  

        for (j = 0; (j < i + 1); j++)  
            ret += base64_chars[char_array_4[j]];  

        while((i++ < 3))  
            ret += '=';
    }
    return ret;
}  

unsigned char ToHex(unsigned char x)   
{   
    return  x > 9 ? x + 55 : x + 48;   
}  

std::string UrlEncode(const std::string& str)  
{  
    std::string strTemp = "";  
    size_t length = str.length();  
    for (size_t i = 0; i < length; i++)  
    {  
        if (isalnum((unsigned char)str[i]) ||   
            (str[i] == '-') ||  
            (str[i] == '_') ||   
            (str[i] == '.') ||   
            (str[i] == '~'))  
            strTemp += str[i];  
        else if (str[i] == ' ')  
            strTemp += "+";  
        else  
        {  
            strTemp += '%';  
            strTemp += ToHex((unsigned char)str[i] >> 4);  
            strTemp += ToHex((unsigned char)str[i] % 16);  
        }  
    }  
    return strTemp;  
}  



wstring string2wstring(const string &str)
{  
    wstring result;  
    //获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), int(str.size()), NULL, 0);  

    WCHAR* pBuffer = new WCHAR[len + 1];  

    //多字节编码转换成宽字节编码  
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), int(str.size()), pBuffer, len);  

    //添加字符串结尾  
    pBuffer[len] = '\0';             

    //删除缓冲区并返回值  
    result.append(pBuffer);  
    delete[] pBuffer;  

    return result;  
} 
string wstring2string(const wstring &wstr)  
{  
    string result;  

    //获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.size()), NULL, 0, NULL, NULL);  
    char* buffer = new char[len + 1];  

    //宽字节编码转换成多字节编码  
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.size()), buffer, len, NULL, NULL);  
    buffer[len] = '\0';  

    //删除缓冲区并返回值  
    result.append(buffer);  
    delete[] buffer;  

    return result;  
}

wstring Utf82UnicodeW(const wstring& utf8string)
{
    return string2wstring(Utf82Unicode(wstring2string(utf8string)));
}

string Utf82Unicode(const string& utf8string)
{  
    int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);  
    if (widesize == ERROR_NO_UNICODE_TRANSLATION)  
    {  
        throw exception("Invalid UTF-8 sequence.");  
    }  
    if (widesize == 0)  
    {  
        throw exception("Error in conversion.");  
    }  

    vector<wchar_t> resultstring(widesize);  

    int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);  

    if (convresult != widesize)  
    {  
        throw exception("La falla!");  
    }  

    return wstring2string(&resultstring[0]);  
} 

string Unicode2Utf8(const string& instring)  
{  
    wstring& widestring = string2wstring(instring);

    int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);  
    if (utf8size == 0)  
    {  
        throw std::exception("Error in conversion.");  
    }  

    vector<char> resultstring(utf8size);  

    int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);  

    if (convresult != utf8size)  
    {  
        throw std::exception("La falla!");  
    }  

    return string(&resultstring[0]);  
} 

