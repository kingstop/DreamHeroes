#pragma once
#include <vector>
struct version_tg
{
	int versionNumber_1;
	int versionNumber_2;
	int versionNumber_3;
};
class LoginConfig
{
public:
	LoginConfig();
	virtual ~LoginConfig();
public:
	bool CompareVersion(int number_1, int number_2, int number_3);
	bool CompareChannel(int channel);
	void SetVersion(int number_1, int number_2, int number_3);
	void SetChannels(const char* channel);
	void SetPlatform(const char* sz);
	void SetNeedPlatformVerify(bool b);
	bool IsNeedPlatformVerify();
	const char* GetPlatformUrl();
	const version_tg* GetVersion();
protected:
	version_tg _version;
	std::vector<int> _channels;
	std::string _platform_url;
	bool _need_platform_verify;
	
};

