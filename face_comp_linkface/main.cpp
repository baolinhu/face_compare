#include <iostream>
#include<stdio.h>
#include <cstring>
#include <exception>
#include<fstream>
#include "curl/curl.h"
#include "json.h"

using namespace std;
// 添加依赖库
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")

//全局变量
ofstream fout("out.json");
size_t callback(char *ptr, size_t size, size_t nmemb, string &stream){

	size_t sizes = size*nmemb;
	string temp(ptr, sizes);
	stream += temp;
	return sizes;
}
int main(){
	cout << "你妹的，中文会乱码吗" << endl;
	CURL *curl;
	CURLM *multi_handle;
	CURLcode res;
	long code;
	string stream;
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	//string imageName = "./image/6.jpg";
	//string imageName2 = "./image/7.jpg";
	//FILE* fp = fopen("./image/6.jpg", "rb");
	//FILE *fp2 = fopen("./image/7.jpg", "rb");
	char *name1 = "./image/1.jpg";
	char *name2 = "./image/7.jpg";
	try{
		
		curl = curl_easy_init();
		res=curl_global_init(CURL_GLOBAL_DEFAULT);
		
		if (curl){

			curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "api_id",
				CURLFORM_COPYCONTENTS, "39d13427ef834f19a63b940b69d2f417", CURLFORM_END);
			curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "api_secret",
				CURLFORM_COPYCONTENTS, "0df4fb4d0f844fecad598f2230d490a0", CURLFORM_END);
			curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "selfie_file",
				CURLFORM_FILE, name1, CURLFORM_END);
			curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "historical_selfie_file",
				CURLFORM_FILE, name2, CURLFORM_END);

			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);//调试信息打开
			curl_easy_setopt(curl, CURLOPT_URL, "https://cloudapi.linkface.cn/identity/historical_selfie_verification");
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
			cout << "CURLOPT_WRITEFUNCTION" << CURLOPT_WRITEFUNCTION << endl;

#ifdef SKIP_PEER_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif
#ifdef SKIP_HOSTNAME_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
			char error[1024];
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);

			res = curl_easy_perform(curl);

			if (res != CURLE_OK){
				cout << "curl_easy_perform() failed:" << curl_easy_strerror(res) << endl;
				cout << endl << error << endl;
				return -1;
			}
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
			Json::Value res_data;
			Json::Reader *reader = new Json::Reader(Json::Features::strictMode());
			if (!reader->parse(stream, res_data)){
				cout << "parse error";
				return -1;
			}
			fout << res_data;
			cout << "HTTP Status Code:" << code << endl;
			cout << res_data << endl;
			curl_easy_cleanup(curl);
			curl_formfree(formpost);
		}
		curl_global_cleanup();
		fout.close();

	}
	catch (exception &ex){
		cout << "curl exception:" << ex.what() << endl;
	}
	//system("pause");
	return 0;
}
