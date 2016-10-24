// CurlTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include "../common/cur_http_client.h"

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

