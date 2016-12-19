#include "stdafx.h"
#include "login_config.h"


LoginConfig::LoginConfig()
{
}


LoginConfig::~LoginConfig()
{
}

bool LoginConfig::CompareVersion(int number_1, int number_2, int number_3)
{
	if (_version.versionNumber_1 == number_1 && _version.versionNumber_2 == number_2)
	{
		return true;
	}
	return false;
}


void LoginConfig::SetNeedPlatformVerify(bool b)
{
	_need_platform_verify = b;
}


bool LoginConfig::IsNeedPlatformVerify( )
{
	return _need_platform_verify;
}

bool LoginConfig::CompareChannel(int channel)
{
	bool ret = false;
	std::vector<int>::iterator it = _channels.begin();
	for (; it != _channels.end(); ++ it)
	{
		if ((*it) == channel)
		{
			ret = true;
			break;
		}
	}
	return ret;
}

void LoginConfig::SetPlatform(const char* sz)
{
	_platform_url = sz;
}

const char* LoginConfig::GetPlatformUrl()
{
	return _platform_url.c_str();
}

const version_tg* LoginConfig::GetVersion()
{
	return &_version;
}

void LoginConfig::SetVersion(int number_1, int number_2, int number_3)
{
	_version.versionNumber_1 = number_1;
	_version.versionNumber_2 = number_2;
	_version.versionNumber_3 = number_3;
}
void LoginConfig::SetChannels(const char* channel)
{
	std::vector<std::string> vc_temp;
	std::string str_channel = channel;
	SplitStringA(str_channel, ";", vc_temp);
	std::vector<std::string>::iterator it = vc_temp.begin();
	for (; it != vc_temp.end(); ++ it)
	{
		int channel = atoi((*it).c_str());
		_channels.push_back(channel);
	}
}
