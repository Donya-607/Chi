#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <Keyboard.h>
#include <xaudio2.h>
#include <mmsystem.h>
#include <cwchar>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
class dragDrop
{
private:
	struct file_info
	{
		std::wstring file_name;
		bool erase_flg = false;
	};
	struct directory_info
	{
		wstring directory_name;
		vector<wstring> extension;
		bool erase_flg = false;
	};
	vector<file_info> files;
	vector<directory_info> directories;
public:
	void loadDirectoryInfo();
	void saveDirectoryInfo();
	void signUpFileName(wchar_t* _file_name);
	void createDirectory(const wchar_t* _directory_name);
	void deleteDirectory(const wchar_t* _directory_name);
	void moveFile(HWND _hwnd);
};