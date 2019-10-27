#ifndef RESOURCEMANAGER_H_

#define RESOURCEMANAGER_H_
#include <string>


class ResourceManager
{
private:
	ResourceManager() {}
	~ResourceManager() {}

	static const int RESOURCE_MAX = 64;

	struct ResourceShaderResourceViews {
		int iRefNum;
		std::wstring path;
		ID3D11ShaderResourceView *SRView;
		ResourceShaderResourceViews() : iRefNum(0), SRView(nullptr) { path[0] = '\0'; }
		void Release(bool force = false) {
			if (iRefNum == 0) return;
			if (--iRefNum <= 0) force = true;
			if (force) {
				if (SRView) SRView->Release();
				SRView = nullptr;
				iRefNum = 0;
				path[0] = '\0';
			}
		}
	};
	struct ResourceVertexShaders {
		int iRefNum;
		std::wstring path;;
		ID3D11VertexShader *VShader;
		ID3D11InputLayout *Layout;
		ResourceVertexShaders() : iRefNum(0), VShader(nullptr), Layout(nullptr) { path[0] = '\0'; }
		void Release(bool force = false) {
			if (iRefNum == 0) return;
			if (--iRefNum <= 0) force = true;
			if (force) {
				if (Layout) Layout->Release();
				if (VShader) VShader->Release();
				Layout = nullptr;
				VShader = nullptr;
				iRefNum = 0;
				path[0] = '\0';
			}
		}
	};
	struct ResourcePixelShaders {
		int iRefNum;
		std::wstring path;
		ID3D11PixelShader *PShader;
		ResourcePixelShaders() : iRefNum(0), PShader(nullptr) { path[0] = '\0'; }
		void Release(bool force = false) {
			if (iRefNum == 0) return;
			if (--iRefNum <= 0) force = true;
			if (force) {
				if (PShader) PShader->Release();
				PShader = nullptr;
				iRefNum = 0;
				path[0] = '\0';
			}
		}
	};

	static ResourceShaderResourceViews	SRViews[RESOURCE_MAX];
	static ResourceVertexShaders		VertexShaders[RESOURCE_MAX];
	static ResourcePixelShaders			PixelShaders[RESOURCE_MAX];


public:
	static ResourceManager* getInstance()
	{
		static ResourceManager instance;

		return &instance;
	}

	static void Release();
	static bool LoadShaderResourceView(
		ID3D11Device *Device, std::wstring filename,
		ID3D11ShaderResourceView **SRView, D3D11_TEXTURE2D_DESC *TexDesc);
	static bool LoadVertexShader(
		ID3D11Device *Device, std::string csoFileName,
		D3D11_INPUT_ELEMENT_DESC *InputElementDesc, int numElements,
		ID3D11VertexShader **VertexShader, ID3D11InputLayout **InputLayout);
	static bool LoadPixelShader(
		ID3D11Device *Device, std::string csoFileName,
		ID3D11PixelShader **PixelShader);

	static void ReleaseShaderResourceView(ID3D11ShaderResourceView *SRView);
	static void ReleaseVertexShader(
		ID3D11VertexShader *VertexShader, ID3D11InputLayout *InputLayout);
	static void ReleasePixelShader(ID3D11PixelShader *PShader);

};

#define pResource ResourceManager::getInstance()

#endif // !RESOURCEMANAGER_H_
