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
	void loadDirectoryInfo()
	{
		FILE* file = nullptr;
		directory_info info;
		int indent = 0;
		if (fopen_s(&file, "./Data/directory_info.txt", "w") != 0)
			return;

		while (1)
		{
			wchar_t copy[1000] = { 0 };
			if (fscanf_s(file, "%ls", &copy) == EOF)
				break;
			info.directory_name = copy;
			
			fscanf_s(file, "%d", &indent);

			for (int i = 0; i < indent; i++)
			{
				wchar_t _copy[1000] = { 0 };
				fscanf_s(file, "%ls", &_copy);

				info.extension.push_back(_copy);
			}

			directories.push_back(info);
		}

		fclose(file);
	}
	void saveDirectoryInfo()
	{
		FILE* file = nullptr;
		if (fopen_s(&file, "./Data/directory_info.txt", "w") != 0)
			return;

		for (directory_info* p = directories.data(); p < directories.data() + directories.size(); p++)
		{

			fprintf_s(file, "%ls", p->directory_name.c_str());


			fprintf_s(file, "%d", p->extension.size());

			for (wstring* pt = p->extension.data(); pt < p->extension.data() + p->extension.size(); pt++)
			{
				fprintf_s(file, "%ls", pt->c_str());
			}

		}

		fclose(file);
	}
	void signUpFileName(wchar_t* _file_name)
	{
		file_info dummy;
		dummy.file_name = _file_name;
		dummy.erase_flg = false;
		files.push_back(dummy);
	}
	void createDirectory(const wchar_t* _directory_name)
	{
		wchar_t buf[1000];

		GetCurrentDirectory(sizeof(buf), buf);
		if (!SetCurrentDirectory(L"./Data"))
		{
			SetCurrentDirectory(L".");
			CreateDirectory(L"Data", NULL);
		}
		SetCurrentDirectory(L"Data");
		CreateDirectory(_directory_name, NULL);

	}
	void deleteDirectory(const wchar_t* _directory_name)
	{
		if (!SetCurrentDirectory(L"./Data"))//Dataフォルダーの有無
		{
			//無かった時は作って終了
			SetCurrentDirectory(L".");
			CreateDirectory(L"Data", NULL);
			SetCurrentDirectory(L"Data");
			return;
		}
		if (!RemoveDirectory(_directory_name))
		{
			SetCurrentDirectory(_directory_name);
			std::vector<WIN32_FIND_DATA> data;
			HANDLE hnd;

			WIN32_FIND_DATA copy;
			hnd = FindFirstFile(L"*", &copy);
			data.push_back(copy);

			while (1)
			{
				if (FindNextFile(hnd, &copy))
					data.push_back(copy);
				else
				{
					FindClose(hnd);
					break;
				}
			}

			for (WIN32_FIND_DATA* p = data.data(); p < data.data() + data.size(); p++)
			{
				DeleteFile(p->cFileName);
			}

			SetCurrentDirectory(L"..");
			RemoveDirectory(_directory_name);

		}
		auto removeIt = std::remove_if(directories.begin(), directories.end(),
			[](directory_info& p)
			{
				return p.erase_flg == true;
			}
		);
		directories.erase(removeIt, directories.end());

	}
	void moveFile(HWND _hwnd)
	{
		SetCurrentDirectory(L"./Data");
		for (file_info* p = files.data(); p < files.data() + files.size(); p++)
		{
			std::wstring name;
			size_t start = p->file_name.find_last_of(L"\\") + 1;
			name = p->file_name.substr(start, p->file_name.size() - start);
			if (CopyFile(p->file_name.c_str(), name.c_str(), true))
			{

				p->erase_flg = true;
			}
		}
		auto removeIt = std::remove_if(files.begin(), files.end(),
			[](file_info& p)
			{
				return p.erase_flg == true;
			}
		);
		files.erase(removeIt, files.end());

		if (files.empty())
			return;


		while (1)
		{
			int judge = -1;
			judge = MessageBox
			(
				_hwnd,
				TEXT("同名のファイルが存在します\n上書きしますか？"),
				L"",
				MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1 | MB_SETFOREGROUND
			);
			if (judge == IDYES)
			{
				for (file_info* p = files.data(); p < files.data() + files.size(); p++)
				{
					std::wstring name;
					size_t start = p->file_name.find_last_of(L"\\") + 1;
					name = p->file_name.substr(start, p->file_name.size() - start);
					if (CopyFile(p->file_name.c_str(), name.c_str(), true))
					{

						p->erase_flg = true;
					}
				}
				files.erase(files.begin(), files.end());

				break;
			}

			if (judge == IDCANCEL || judge == IDNO)
			{
				//for (file_info* p = files.data(); p<files.data() + files.size(); p++)
				//{
				//	int index = 1;
				//	while (!p->erase_flg)
				//	{
				//		std::wstring name;
				//		size_t start1 = p->file_name.find_last_of(L"\\") + 1;
				//		size_t start2 = p->file_name.find_last_of(L".");
				//		name = p->file_name.substr(start1, (start2-start1)) + +L"("+std::to_wstring(index)+L")"+p->file_name.substr(start2,p->file_name.size());
				//		if (CopyFile(p->file_name.c_str(), name.c_str(), true))
				//		{

				//			p->erase_flg = true;
				//		}
				//		index++;
				//	}
				//}
				files.erase(files.begin(), files.end());

				break;
			}
		}
	}
};