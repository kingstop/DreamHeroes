#pragma once
#ifndef __HTTP_CURL_H__
#define  __HTTP_CURL_H__
#include <string>
class CurlHttpClient
{
public:
	CurlHttpClient();
	~CurlHttpClient();
public:
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);
	int Get(const std::string & strUrl, std::string & strResponse);
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char* pCaPath = NULL);
	int Gets(const std::string & strUrl, std::string & strResponse, const char* pCaPath = NULL);
public:
	void SetDebug(bool b);
private:
	bool _debug;

};


#endif // !__HTTP_CURL_H__
