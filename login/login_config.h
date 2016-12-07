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
	const version_tg* GetVersion();
protected:
	version_tg _version;
	std::vector<int> _channels;
	
};

