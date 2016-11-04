// CurlTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include "cur_http_client.h"
#include "curl/curl.h"
#include "curl/easy.h"
std::string cur_request = "{ 
	\"signature\" = \"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\";
	\"purchase-info\" = \"ewoJIm9yaWdpbmFsLXB1cmNoYXNlLWRhdGUtcHN0IiA9ICIyMDE2LTExLTAzIDA0OjAwOjI0IEFtZXJpY2EvTG9zX0FuZ2VsZXMiOwoJInVuaXF1ZS1pZGVudGlmaWVyIiA9ICI2OWM4M2RkMjM1OTEwZWE2NTM2ZDU2OGQ0NTVhYWI5MjU2MTAyNGVkIjsKCSJvcmlnaW5hbC10cmFuc2FjdGlvbi1pZCIgPSAiMTAwMDAwMDI0NzQ0Mjk1MSI7CgkiYnZycyIgPSAiMCI7CgkidHJhbnNhY3Rpb24taWQiID0gIjEwMDAwMDAyNDc0NDI5NTEiOwoJInF1YW50aXR5IiA9ICIxIjsKCSJvcmlnaW5hbC1wdXJjaGFzZS1kYXRlLW1zIiA9ICIxNDc4MTcwODI0NjI2IjsKCSJ1bmlxdWUtdmVuZG9yLWlkZW50aWZpZXIiID0gIkFGN0YwRkZFLTQ0NzEtNDE1Mi1CM0I3LTM3Q0Q5MTU3QTRCOSI7CgkicHJvZHVjdC1pZCIgPSAiY29tLmJvZGhpLmRyZWFtaGVyb2VzLmdvbGQ2IjsKCSJpdGVtLWlkIiA9ICIxMTcxMzIxMjc5IjsKCSJiaWQiID0gImNvbS5ib2RoaS5kcmVhbWhlcm9lcyI7CgkicHVyY2hhc2UtZGF0ZS1tcyIgPSAiMTQ3ODE3MDgyNDYyNiI7CgkicHVyY2hhc2UtZGF0ZSIgPSAiMjAxNi0xMS0wMyAxMTowMDoyNCBFdGMvR01UIjsKCSJwdXJjaGFzZS1kYXRlLXBzdCIgPSAiMjAxNi0xMS0wMyAwNDowMDoyNCBBbWVyaWNhL0xvc19BbmdlbGVzIjsKCSJvcmlnaW5hbC1wdXJjaGFzZS1kYXRlIiA9ICIyMDE2LTExLTAzIDExOjAwOjI0IEV0Yy9HTVQiOwp9\"\;
	\"environment\" = \"Sandbox\";
	\"pod" = \"100\"\;
	\"signing-status\" = \"0\"\;
\}"
int main()
{

	CurlHttpClient client;
	std::string url = "https://www.baidu.com/";
	//std::string post = "ie=utf-8&f=8&rsv_bp=0&rsv_idx=1&tn=baidu&wd=ceshi%20&rsv_pq=960690750002daa8&rsv_t=cb3fUOZ%2FooHMIwdUt0qvS91oTJo5BdPKsuxu89G048JlCsC4pZ4WiNJ3wZ8&rsv_enter=1&rsv_sug3=7&rsv_sug1=5&rsv_sug7=100&rsv_sug2=0&inputT=1849&rsv_sug4=6044";
	std::string res = "";
	client.Get(url, res);
	std::cout << "get_res:" << res << std::endl;

	std::string urlContainParam = "https://www.baidu.com/";
	std::string paramRes = "";
	client.Get(urlContainParam, paramRes);
	std::cout << "urlContainParam:" << paramRes << std::endl;
	int i;
	std::cin >> i;
    return 0;
}

