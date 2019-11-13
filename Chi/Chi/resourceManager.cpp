#include <fstream>
#include <d3d11.h>
#include <WICTextureLoader.h>
#include "./liblary/DirectXTex-master/DirectXTex/DirectXTex.h"
#include "ResourceManager.h"
#include "misc.h"


ResourceManager::ResourceShaderResourceViews	ResourceManager::SRViews[RESOURCE_MAX];
ResourceManager::ResourceVertexShaders			ResourceManager::VertexShaders[RESOURCE_MAX];
ResourceManager::ResourcePixelShaders			ResourceManager::PixelShaders[RESOURCE_MAX];

bool ReadBinaryFile(const char *filename, char **blob, unsigned int &size)
{
	std::ifstream inputFile(filename, std::ifstream::binary);
	inputFile.seekg(0, std::ifstream::end);
	size = static_cast<int>(inputFile.tellg());
	inputFile.seekg(0, std::ifstream::beg);
	*blob = new char[size];
	inputFile.read(*blob, size);
	return true;
}

bool ResourceManager::LoadShaderResourceView(
	ID3D11Device *Device, std::wstring filename,
	ID3D11ShaderResourceView **SRView, D3D11_TEXTURE2D_DESC *TexDesc)
{
	std::wstring onlyFileName{};
	size_t slashStart		= filename.find_last_of( '/' );
	size_t backSlashStart	= filename.find_last_of( '\\' );
	if ( slashStart != std::wstring::npos )
	{
		onlyFileName = filename.substr( slashStart );
	}
	if ( backSlashStart != std::wstring::npos )
	{
		onlyFileName = filename.substr( backSlashStart );
	}

	HRESULT hr = 0;
	int no = -1;
	ResourceShaderResourceViews *find = nullptr;
	ID3D11Resource *Resource = nullptr;
	bool use_SRGB = false;
	//	�f�[�^�T��
	for (int n = 0; n < RESOURCE_MAX; n++) {
		ResourceShaderResourceViews *p = &SRViews[n];//	�G�C���A�X

													 //	�f�[�^�������Ȃ疳��
													 //	�A���A�ŏ��Ɍ������̈�Ȃ�Z�b�g�p�Ɋm��
		if (p->iRefNum == 0) {
			if (no == -1) no = n;
			continue;
		}

		//	�t�@�C���p�X���Ⴄ�Ȃ疳��
		if (wcscmp(p->path.c_str(), onlyFileName.c_str()) != 0) continue;

		//	�����t�@�C�������݂���
		find = p;
		p->SRView->GetResource(&Resource);
		break;
	}

	//	�f�[�^��������Ȃ��������V�K�ǂݍ���
	if (!find)
	{
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;
		std::wstring dummy = onlyFileName;
		size_t start = dummy.find_last_of(L".");


		std::wstring extension = dummy.substr(start, dummy.size() - start);

		ResourceShaderResourceViews *p = &SRViews[no];

		if (L".png" == extension || L".jpeg" == extension || L".jpg" == extension || L".jpe" == extension || L".gif" == extension || L".tiff" == extension || L".tif" == extension || L".bmp" == extension)
		{
			DirectX::LoadFromWICFile( filename.c_str(), 0, &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else if (L".dds" == extension)
		{
			DirectX::LoadFromDDSFile( filename.c_str(), 0, &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else if (L".tga" == extension || L".vda" == extension || L".icb" == extension || L".vst" == extension)
		{
			use_SRGB = true;

			DirectX::LoadFromTGAFile( filename.c_str(), &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		if
			(
				FAILED
				(
					hr = DirectX::CreateShaderResourceViewEx
					(
						Device,
						image.GetImages(),
						image.GetImageCount(),
						metadata,
						D3D11_USAGE_DEFAULT,
						D3D11_BIND_SHADER_RESOURCE,
						0,
						D3D11_RESOURCE_MISC_TEXTURECUBE,
						true,
						&(p->SRView)
					)
				)
			)
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		find = p;
		p->path = onlyFileName;
		p->SRView->GetResource(&Resource);
	}

	//	�ŏI����
	ID3D11Texture2D *tex2D;
	Resource->QueryInterface(&tex2D);
	*SRView = find->SRView;
	tex2D->GetDesc(TexDesc);
	find->iRefNum++;
	tex2D->Release();
	Resource->Release();
	return true;
}

//--------------------------------
//	���_�V�F�[�_�̃��[�h
//--------------------------------
bool ResourceManager::LoadVertexShader(
	ID3D11Device *Device, std::string csoFileName,
	D3D11_INPUT_ELEMENT_DESC *InputElementDesc, int numElements,
	ID3D11VertexShader **VertexShader, ID3D11InputLayout **InputLayout)
{
	*VertexShader = nullptr;
	*InputLayout = nullptr;

	ResourceVertexShaders *find = nullptr;
	int no = -1;

	//	const char * -> wchar_t *
	std::wstring filename(csoFileName.begin(), csoFileName.end());

	//	�f�[�^����
	for (int n = 0; n < RESOURCE_MAX; n++) {
		ResourceVertexShaders *p = &VertexShaders[n];

		if (p->iRefNum == 0) {
			if (no == -1) no = n;
			continue;
		}

		if (wcscmp(p->path.c_str(), filename.c_str()) != 0) continue;

		//	�����t�@�C�������݂���
		find = p;
		break;
	}

	//	�f�[�^��������Ȃ�����
	if (!find) {
		ResourceVertexShaders *p = &VertexShaders[no];
		char *blob;
		unsigned int size;

		//	�R���p�C���ς݃s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̓ǂݍ���
		if (!ReadBinaryFile(csoFileName.c_str(), &blob, size)) return false;
		
		//	���_�V�F�[�_�[�I�u�W�F�N�g�̐���
		if (FAILED(Device->CreateVertexShader(blob, size, nullptr, &p->VShader)))
			return false;

		HRESULT hr = Device->CreateInputLayout(InputElementDesc, numElements, blob, size, &p->Layout);
		//	���̓��C�A�E�g�̍쐬
		delete[] blob;
		if (FAILED(hr)) return false;

		//	�V�K�f�[�^�̕ۑ�
		find = p;
		find->path = filename;
	}

	//	�ŏI����
	*VertexShader = find->VShader;
	*InputLayout = find->Layout;
	find->iRefNum++;

	return true;
}

//--------------------------------
//	�s�N�Z���V�F�[�_�̃��[�h
//--------------------------------
bool ResourceManager::LoadPixelShader(
	ID3D11Device *Device, std::string csoFileName,
	ID3D11PixelShader **PixelShader)
{
	*PixelShader = nullptr;
	ResourcePixelShaders *find = nullptr;
	int no = -1;

	//	const char * -> wchar_t *
	std::wstring filename(csoFileName.begin(), csoFileName.end());

	//	�f�[�^����
	for (int n = 0; n < RESOURCE_MAX; n++) {
		ResourcePixelShaders *p = &PixelShaders[n];
		if (p->iRefNum == 0) {
			if (no == -1) no = n;
			continue;
		}
		if (wcscmp(p->path.c_str(), filename.c_str()) != 0) continue;

		//	�����t�@�C�������݂���
		find = p;
		break;
	}

	//	�V�K�쐬
	if (!find) {
		ResourcePixelShaders *p = &PixelShaders[no];
		char *blob;
		unsigned int size;

		//	�R���p�C���ς݃s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̓ǂݍ���
		if (!ReadBinaryFile(csoFileName.c_str(), &blob, size)) return false;

		HRESULT hr = Device->CreatePixelShader(blob, size, nullptr, &p->PShader);
		delete[] blob;
		if (FAILED(hr)) return false;

		find = p;
		find->path = filename;
	}

	//	�ŏI�����i�Q�Ɠn���Ńf�[�^��n���j
	*PixelShader = find->PShader;
	find->iRefNum++;

	return true;
}

//--------------------------------
//	�e�N�X�`���������
//--------------------------------
void ResourceManager::ReleaseShaderResourceView(ID3D11ShaderResourceView *SRView)
{
	if (!SRView) return;
	for (int n = 0; n < RESOURCE_MAX; n++) {
		ResourceShaderResourceViews *p = &SRViews[n];

		//	�f�[�^�������Ȃ疳��
		if (p->iRefNum == 0) continue;

		//	�f�[�^���Ⴄ�Ȃ疳��
		if (SRView != p->SRView) continue;

		//	�f�[�^�����݂���
		p->Release();
		break;
	}
}

//--------------------------------
//	���_�V�F�[�_�������
//--------------------------------
void ResourceManager::ReleaseVertexShader(
	ID3D11VertexShader *VertexShader, ID3D11InputLayout *InputLayout)
{
	if (!VertexShader) return;
	if (!InputLayout) return;

	for (int n = 0; n < RESOURCE_MAX; n++) {
		ResourceVertexShaders *p = &VertexShaders[n];
		if (p->iRefNum == 0) continue;
		if (VertexShader != p->VShader) continue;
		if (InputLayout != p->Layout) continue;

		//	�f�[�^�����݂���
		p->Release();
		break;
	}
}

//--------------------------------
//	�s�N�Z���V�F�[�_�������
//--------------------------------
void ResourceManager::ReleasePixelShader(ID3D11PixelShader *PShader)
{
	if (!PShader) return;
	for (int n = 0; n < RESOURCE_MAX; n++) {
		ResourcePixelShaders *p = &PixelShaders[n];

		if (p->iRefNum == 0) continue;

		if (PShader != p->PShader) continue;

		//	�f�[�^�����݂���
		p->Release();
		break;
	}
}

//--------------------------------
//	�������
//--------------------------------
void ResourceManager::Release()
{
	for (int i = 0; i < RESOURCE_MAX; i++) {
		SRViews[i].Release(true);
		VertexShaders[i].Release(true);
		PixelShaders[i].Release(true);
	}
}