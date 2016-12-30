// CurlTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include "cur_http_client.h"
#include "curl/curl.h"
#include "curl/easy.h"
#include "http_server.h"
#include "platform_client.h"

int main()
{
	IPlatformPayProto::protobuf_AssignDesc_msg_5fgame_5fplatform_2eproto();
	net_global::init_net_service(3, 1, NULL, true, 5123000);
	PlatformClient g_plat_form;
	g_plat_form.connect("121.196.203.220", 26000);
	while (true)
	{
		g_plat_form.run_no_wait();
		net_global::update_net_service();
#ifdef WIN32
		Sleep(2);
#else
		usleep(2);
#endif
	}
	//boost::asio::io_service service;
	//HttpServer server(service, 2000);

	//server.start();
	//service.run();
	
    return 0;
}

