#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "resourceManager.h"
#include "util.h"

namespace GameLib
{
	class Primitive
	{

		ID3D11VertexShader*         vertexShader;
		ID3D11PixelShader*          pixelShader;
		ID3D11InputLayout*          inputLayout;
		ID3D11Buffer*               buffer;
		ID3D11RasterizerState*      rasterizerState;
		ID3D11DepthStencilState*    depthStencilState;

		ID3D11Device*               device;
	public:
		Primitive(ID3D11Device*);
		~Primitive();

		//------------------------------------------------------
		//  ��`�`��
		//------------------------------------------------------
		void rect(ID3D11DeviceContext*, const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
			const DirectX::XMFLOAT2&, float, const DirectX::XMFLOAT4&) const;

		//------------------------------------------------------
		//  ��`�`��
		//------------------------------------------------------
		void rect(ID3D11DeviceContext*, float, float, float, float,
			float, float, float,
			float, float, float, float) const;

		//------------------------------------------------------
		//  ���`��
		//------------------------------------------------------
		void line(ID3D11DeviceContext*, const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
			const DirectX::XMFLOAT4&, float) const;

		//------------------------------------------------------
		//  ���`��
		//------------------------------------------------------
		void line(ID3D11DeviceContext*, float, float, float, float,
			float, float, float, float,
			float) const;

		//------------------------------------------------------
		//  �~�`��
		//------------------------------------------------------
		void circle(ID3D11DeviceContext*, const DirectX::XMFLOAT2&, float,
			const DirectX::XMFLOAT4&, int) const;

		//------------------------------------------------------
		//  �~�`��
		//------------------------------------------------------
		void circle(ID3D11DeviceContext*, float, float, float,
			float, float, float, float,
			int) const;

		//------------------------------------------------------
		//  �l�p�|���S���`��i���_�w��j
		//------------------------------------------------------
		void quad(ID3D11DeviceContext*, const DirectX::XMFLOAT2(&)[4],
			const DirectX::XMFLOAT4&) const;

		//------------------------------------------------------
		//  �l�p�|���S���`��i���_�w��j
		//------------------------------------------------------
		void quad(ID3D11DeviceContext*, const DirectX::XMFLOAT2(&)[4],
			float, float, float, float) const;

		//------------------------------------------------------
		//  �\���̒�`
		//------------------------------------------------------
		struct vertex { DirectX::XMFLOAT3 position; DirectX::XMFLOAT4 color; };

	};



}

#endif // !PRIMITIVE_H_
