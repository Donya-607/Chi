#include "gameLib.h"
#include "vector.h"
#include "GameLibFunctions.h"
#include <Shlwapi.h>

//library//
void startUpLibrary(LPCTSTR caption, HINSTANCE instance, int width, int height, bool isFullscreen, int iconNum, double frameRate)
{
	GameLib::init(caption, instance, width, height, iconNum, isFullscreen, frameRate);
}
void startUpLibrary(LPCTSTR caption, HINSTANCE instance, DirectX::XMINT2 screenSize, bool isFullscreen, int iconNum, double frameRate)
{
	GameLib::init(caption, instance, screenSize.x, screenSize.y, iconNum, isFullscreen, frameRate);
}

void shutDownLibrary()
{
	GameLib::uninit();
}

DirectX::XMINT2 getWindowSize()
{
	return GameLib::getWindowSize();
}


//ClearWindow//
void clearWindow(const float r, const float g, const float b, const float a)
{
	GameLib::clear({ r,g,b,a });
}
void clearWindow(const DirectX::XMFLOAT4& color)
{
	GameLib::clear(color);
}


//RenderTarget
bool setRenderTarget(ID3D11RenderTargetView** _renderTarget)
{
	return GameLib::setRenderTarget(_renderTarget);
}
bool resetRendertarget()
{
	return GameLib::resetRendertarget();
}
void clearRendertarget(ID3D11RenderTargetView* _renderTarget, const DirectX::XMFLOAT4& color)
{
	GameLib::clearRT(_renderTarget, color);
}


//ShaderResourceView
bool createSRV(ID3D11ShaderResourceView** _SRV,ID3D11RenderTargetView** RT)
{
	return GameLib::createSRV(_SRV,RT);
}

void postEffect_Bloom_SRV(ID3D11ShaderResourceView** _shaderResource, DirectX::XMFLOAT4 _judge_color)
{
	GameLib::postEffect_Bloom_SRV(_shaderResource, _judge_color);
}

void postEffect_Bloom(float _blur_value, bool flg)
{
	GameLib::postEffect_Bloom(_blur_value,flg);
}
void setBloomRT()
{
	GameLib::setBloomRT();
}

void filterScreen(float saturate,float bright,float contrast)
{
	GameLib::filter_screen(bright,contrast,saturate);
}

//BLENDMODE//

void setBlendMode_NONE(const float alpha)
{
	GameLib::blend::setBlendMode(GameLib::blend::NONE, alpha);
}

void setBlendMode_ALPHA(const float alpha)
{
	GameLib::blend::setBlendMode(GameLib::blend::ALPHA, alpha);
}

void setBlendMode_ADD(const float alpha)
{
	GameLib::blend::setBlendMode(GameLib::blend::ADD, alpha);
}

void setBlendMode_SUBTRACT(const float alpha)
{
	GameLib::blend::setBlendMode(GameLib::blend::SUBTRACT, alpha);
}

void setBlendMode_REPLACE(const float alpha)
{
	GameLib::blend::setBlendMode(GameLib::blend::REPLACE, alpha);
}

void setBlendMode_MULTIPLY(const float alpha)
{
	GameLib::blend::setBlendMode(GameLib::blend::MULTIPLY, alpha);
}

void setBlendMode_LIGHTEN(const float alpha)
{
	GameLib::blend::setBlendMode(GameLib::blend::LIGHTEN, alpha);
}

void setBlendMode_DARKEN(const float alpha)
{
	GameLib::blend::setBlendMode(GameLib::blend::DARKEN, alpha);
}

void setBlendMode_SCREEN(const float alpha)
{
	GameLib::blend::setBlendMode(GameLib::blend::SCREEN, alpha);
}


//PRIMITIVE//
void drawRect(const float x, const float y, const float w, const float h, const float cx, const float cy, const float angle, const float r, const float g, const float b)
{
	GameLib::primitive::rect(x, y, w, h, cx, cy, angle, r, g, b);
}
void drawRect(const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT2& center, const float angle, const DirectX::XMFLOAT3& color)
{
	GameLib::primitive::rect(pos, size, center, angle, color);
}

void drawLine(const float x1, const float y1, const float x2, const float y2, const float width, const float r, const float g, const float b)
{
	GameLib::primitive::line(x1, y1, x2, y2, r, g, b, width);
}
void drawLine(const DirectX::XMFLOAT2& begin, const DirectX::XMFLOAT2& end, const float width, const DirectX::XMFLOAT3& color)
{
	GameLib::primitive::line(begin, end, color, width);
}

void drawCircle(const float x, const float y, const float radius, const float r, const float g, const float b, int n)
{
	GameLib::primitive::circle(x, y, radius, r, g, b, n);
}
void drawCircle(const DirectX::XMFLOAT2& pos, const float radius, const DirectX::XMFLOAT3& color, const int n)
{
	GameLib::primitive::circle(pos, radius, color, n);
}

void drawQuad(const DirectX::XMFLOAT2(&v)[4], const float r, const float g, const float b)
{
	GameLib::primitive::quad(v, r, g, b);
}

void drawQuad(const DirectX::XMFLOAT2(&v)[4], const DirectX::XMFLOAT3& color)
{
	GameLib::primitive::quad(v, color);
}


//SPRITE//

void spriteLoad(Sprite* _sprite, std::wstring _spriteName)
{
	GameLib::texture::SpriteRoad(_sprite, _spriteName);
}

void spriteRender(Sprite* _sprite, const float pos_x, const float pos_y, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y, pos_x + _sprite->getSize().x, pos_y + _sprite->getSize().y,
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);

}
void spriteRender(Sprite* _sprite, const DirectX::XMFLOAT2& pos, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y, pos.x + _sprite->getSize().x, pos.y + _sprite->getSize().y,
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderRect(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y, pos_x + texSize_x, pos_y + texSize_y,
		texPos_x, texPos_y, texPos_x + texSize_x, texPos_y + texSize_y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}
void spriteRenderRect(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y, pos.x + texSize.x, pos.y + texSize.y,
		texPos.x, texPos.y, texPos.x + texSize.x, texPos.y + texSize.y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderExtend(Sprite* _sprite, const float pos_x, const float pos_y, const float Magnification, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y, pos_x + (_sprite->getSize().x*Magnification), pos_y + (_sprite->getSize().y*Magnification),
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}
void spriteRenderExtend(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const float Magnification, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y, pos.x + (_sprite->getSize().x*Magnification), pos.y + (_sprite->getSize().y*Magnification),
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderExtend2(Sprite* _sprite, const float pos_x, const float pos_y, const float Magnification_x, const float Magnification_y, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y, pos_x + (_sprite->getSize().x*Magnification_x), pos_y + (_sprite->getSize().y*Magnification_y),
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}
void spriteRenderExtend2(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& Magnification, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y, pos.x + (_sprite->getSize().x*Magnification.x), pos.y + (_sprite->getSize().y*Magnification.y),
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderExtend3(Sprite* _sprite, const float pos_x, const float pos_y, const float centerPos_x, const float centerPos_y, const float Magnification_x, const float Magnification_y, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y,
		pos_x + (_sprite->getSize().x - Magnification_x), pos_y + (_sprite->getSize().y - Magnification_y),
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		0, 1, 1, 1, centerPos_x, centerPos_y, _tempX, _tempY);
}
void spriteRenderExtend3(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2&centerPos, const DirectX::XMFLOAT2& Magnification, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y,
		pos.x + (_sprite->getSize().x - Magnification.x), pos.y + (_sprite->getSize().y - Magnification.y),
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		0, 1, 1, 1, centerPos.x, centerPos.y, _tempX, _tempY);
}

void spriteRenderRectExtend(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float Magnification, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y,
		pos_x + (texSize_x*Magnification), pos_y + (texSize_y*Magnification),
		texPos_x, texPos_y, texPos_x + texSize_x, texPos_y + texSize_y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}
void spriteRenderRectExtend(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2&texPos, const DirectX::XMFLOAT2&texSize, const float Magnification, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y,
		pos.x + (texSize.x*Magnification), pos.y + (texSize.y*Magnification),
		texPos.x, texPos.y, texPos.x + texSize.x, texPos.y + texSize.y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderRectExtend2(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float Magnification_x, const float Magnification_y, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y,
		pos_x + (texSize_x*Magnification_x), pos_y + (texSize_y*Magnification_y),
		texPos_x, texPos_y, texPos_x + texSize_x, texPos_y + texSize_y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}
void spriteRenderRectExtend2(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2&texPos, const DirectX::XMFLOAT2&texSize, const DirectX::XMFLOAT2& Magnification, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y,
		pos.x + (texSize.x*Magnification.x), pos.y + (texSize.y*Magnification.y),
		texPos.x, texPos.y, texPos.x + texSize.x, texPos.y + texSize.y,
		0, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderRectExtend3(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float centerPos_x, const float centerPos_y, const float Magnification_x, const float Magnification_y, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y,
		pos_x + texSize_x*Magnification_x, pos_y + texSize_y*Magnification_y,
		texPos_x, texPos_y, texPos_x + texSize_x, texPos_y + texSize_y,
		0, 1, 1, 1, centerPos_x, centerPos_y, _tempX, _tempY);
}
void spriteRenderRectExtend3(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2&texPos, const DirectX::XMFLOAT2&texSize, const DirectX::XMFLOAT2& centerPos, const DirectX::XMFLOAT2& Magnification, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y,
		pos.x + texSize.x*Magnification.x, pos.y + texSize.y*Magnification.y,
		texPos.x, texPos.y, texPos.x + texSize.x, texPos.y + texSize.y,
		0, 1, 1, 1, centerPos.x, centerPos.y, _tempX, _tempY);
}

void spriteRenderRota(Sprite* _sprite, const float pos_x, const float pos_y, const float Magnification, float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y, pos_x + _sprite->getSize().x*Magnification, pos_y + _sprite->getSize().y*Magnification,
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		angle, 1, 1, 1, 0, 0, _tempX, _tempY);
}
void spriteRenderRota(Sprite* _sprite, const DirectX::XMFLOAT2 pos, const float Magnification, float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y, pos.x + _sprite->getSize().x*Magnification, pos.y + _sprite->getSize().y*Magnification,
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		angle, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderRota2(Sprite* _sprite, const float pos_x, const float pos_y, const float Magnification_x, const float Magnification_y, float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y,
		pos_x + _sprite->getSize().x*Magnification_x, pos_y + _sprite->getSize().y*Magnification_y,
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		angle, 1, 1, 1, 0, 0, _tempX, _tempY);
}
void spriteRenderRota2(Sprite* _sprite, const DirectX::XMFLOAT2 pos, const DirectX::XMFLOAT2 Magnification, float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y,
		pos.x + _sprite->getSize().x*Magnification.x, pos.y + _sprite->getSize().y*Magnification.y,
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		angle, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderRota3(Sprite* _sprite, const float pos_x, const float pos_y, const float centerPos_x, const float centerPos_y, const float Magnification_x, const float Magnification_y, float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y,
		pos_x + _sprite->getSize().x*Magnification_x, pos_y + _sprite->getSize().y*Magnification_y,
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		angle, 1, 1, 1, centerPos_x, centerPos_y, _tempX, _tempY);
}
void spriteRenderRota3(Sprite* _sprite, const DirectX::XMFLOAT2 pos, const DirectX::XMFLOAT2& centerPos, const DirectX::XMFLOAT2 Magnification, float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y,
		pos.x + _sprite->getSize().x*Magnification.x, pos.y + _sprite->getSize().y*Magnification.y,
		0, 0, _sprite->getSize().x, _sprite->getSize().y,
		angle, 1, 1, 1, centerPos.x, centerPos.y, _tempX, _tempY);
}

void spriteRenderRectRota(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y, pos_x + texSize_x, pos_y + texSize_y,
		texPos_x, texPos_y, texPos_x + texSize_x, texPos_y + texSize_y,
		angle, 1, 1, 1, 0, 0, _tempX, _tempY);
}
void spriteRenderRectRota(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y, pos.x + texSize.x, pos.y + texSize.y,
		texPos.x, texPos.y, texPos.x + texSize.x, texPos.y + texSize.y,
		angle, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderRectRota2(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float Magnification_x, const float Magnification_y, const float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y,
		pos_x + texSize_x*Magnification_x, pos_y + texSize_y*Magnification_y,
		texPos_x, texPos_y, texPos_x + texSize_x, texPos_y + texSize_y,
		angle, 1, 1, 1, 0, 0, _tempX, _tempY);
}
void spriteRenderRectRota2(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const DirectX::XMFLOAT2& Magnification, const float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y,
		pos.x + texSize.x*Magnification.x, pos.y + texSize.y*Magnification.y,
		texPos.x, texPos.y, texPos.x + texSize.x, texPos.y + texSize.y,
		angle, 1, 1, 1, 0, 0, _tempX, _tempY);
}

void spriteRenderRectRota3(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float centerPos_x, const float centerPos_y, const float Magnification_x, const float Magnification_y, const float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos_x, pos_y,
		pos_x + texSize_x*Magnification_x, pos_y + texSize_y*Magnification_y,
		texPos_x, texPos_y, texPos_x + texSize_x, texPos_y + texSize_y,
		angle, 1, 1, 1, centerPos_x, centerPos_y, _tempX, _tempY);
}
void spriteRenderRectRota3(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const DirectX::XMFLOAT2& centerPos, const DirectX::XMFLOAT2& Magnification, const float angle, bool _tempX, bool _tempY)
{
	GameLib::texture::SpriteRender(
		_sprite,
		pos.x, pos.y,
		pos.x + texSize.x*Magnification.x, pos.y + texSize.y*Magnification.y,
		texPos.x, texPos.y, texPos.x + texSize.x, texPos.y + texSize.y,
		angle, 1, 1, 1, centerPos.x, centerPos.y, _tempX, _tempY);
}


//CAMERA//
DirectX::XMMATRIX getProjectionMatrix()
{
	return GameLib::camera::GetProjectionMatrix();
}

DirectX::XMMATRIX getViewMatrix()
{
	return GameLib::camera::GetViewMatrix();
}

void setCamPos(const DirectX::XMFLOAT3& pos)
{
	GameLib::camera::setPos(pos);
}

void setCamPos(const float x, const float y, const float z)
{
	GameLib::camera::setPos({ x,y,z });
}

void setTarget(const DirectX::XMFLOAT3& _target)
{
	GameLib::camera::setTarget(_target);
}

void setTarget(const float x, const float y, const float z)
{
	GameLib::camera::setTarget({x,y,z});
}

DirectX::XMFLOAT4 getCamPos()
{
	return GameLib::camera::getPos();
}

DirectX::XMFLOAT4 getCamTarget()
{
	return GameLib::camera::getTarget();
}

//light//

void setLineLight(const DirectX::XMFLOAT4& _position,const DirectX::XMFLOAT4& _lightAmbient,const DirectX::XMFLOAT4& _lightColor)
{
	GameLib::light::setLineLight(_position,_lightAmbient,_lightColor);
}
void setLineLight(const float px,const float py,const float pz,const float pw,const float x, const float y, const float z, const float w,const float r,const float g, const float b, const float a)
{
	GameLib::light::setLineLight({px,py,pz,pw}, { x, y, z, w }, { r,g,b,a });
}

line_light& getLineLight()
{
	return GameLib::light::getLineLight();
}

void setPointLight(const PointLight& _info)
{
	GameLib::light::setPointLight(_info);
}

void setPointlightInfo(const int index, const PointLight& _info)
{
	GameLib::light::setPointlightInfo(index, _info);
}

void removePointLight(const int i)
{
	GameLib::light::removePointLight(i);
}

void resetPointLight()
{
	return GameLib::light::resetPointLight();
}

std::vector<point_light>& getPointLight()
{
	return GameLib::light::getPointLight();
}


//static_mesh//
void createCube(static_mesh* cube)
{
	GameLib::staticMesh::createCube(cube);
}

void createSphere(static_mesh* sphere, u_int slice, u_int stack)
{
	GameLib::staticMesh::createSphere(sphere, slice, stack);
}

void createPlane(static_mesh * _plane, u_int _vertical, u_int _side)
{
	GameLib::staticMesh::createPlane(_plane, _vertical, _side);
}

void createBillboard(static_mesh* _mesh, const wchar_t* _textureName)
{
	GameLib::staticMesh::createBillboard(_mesh, _textureName);
}

void loadOBJ(static_mesh* staticMesh, const wchar_t* objName)
{
	GameLib::staticMesh::loadMesh(staticMesh, objName);
}

void loadOBJ_MTL(static_mesh* staticMesh, wchar_t* objName, wchar_t* mtlName)
{
	GameLib::staticMesh::loadMeshMTL(staticMesh, objName, mtlName);
}

static_mesh::primitive_material & getPrimitiveMaterial(static_mesh* _mesh)
{
	return GameLib::staticMesh::getPrimitiveMaterial(_mesh);
}

void OBJRender(static_mesh* staticMesh, const DirectX::XMFLOAT4X4&SynthesisMatrix, const DirectX::XMFLOAT4X4&worldMatrix,const DirectX::XMFLOAT4&materialColor, bool wireFlg)
{
	GameLib::staticMesh::staticMeshRender(staticMesh, SynthesisMatrix,worldMatrix, getCamPos(), getLineLight(),getPointLight(), materialColor, wireFlg);
}

void billboardRender(static_mesh* _mesh, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& _pos, const DirectX::XMFLOAT2 _scale, const float _angle, const DirectX::XMFLOAT4& _cam_pos, const DirectX::XMFLOAT2& texpos, const DirectX::XMFLOAT2& texsize,const float alpha,const DirectX::XMFLOAT3& color)
{
	GameLib::staticMesh::builboradRender(_mesh, view_projection, _pos, _scale, _angle, _cam_pos, texpos, texsize,alpha,color);
}

void billboard_z_Render(static_mesh* _mesh, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4&_pos, const DirectX::XMFLOAT2 _scale, const float angle, const DirectX::XMFLOAT4& _cam_pos, const DirectX::XMFLOAT2& texpos, const DirectX::XMFLOAT2& texsize, const float alpha, const DirectX::XMFLOAT3& color)
{
	GameLib::staticMesh::builborad_z_Render(_mesh, view_projection, _pos, _scale, angle, _cam_pos, texpos, texsize, alpha, color);
}

void billboard_bloom_Render(static_mesh* _mesh, const DirectX::XMFLOAT4X4&view_projection, const DirectX::XMFLOAT4&_pos, const DirectX::XMFLOAT2 _scale, const float angle, const DirectX::XMFLOAT4& _cam_pos, const DirectX::XMFLOAT2& texpos, const DirectX::XMFLOAT2& texsize, const DirectX::XMFLOAT4& judge_color, const float alpha, const DirectX::XMFLOAT3& color)
{
	GameLib::staticMesh::builborad_bloom_Render(_mesh, view_projection, _pos, _scale, angle, _cam_pos, texpos, texsize, alpha, color, judge_color);
}


//skinned_mesh//
void loadFBX(skinned_mesh* skinnedMesh, const std::string& FBXName, bool load_cerealize,bool isTpose)
{
	GameLib::skinnedMesh::loadFBX(skinnedMesh, FBXName, load_cerealize,isTpose);
}

void loadShader(fbx_shader& shader, std::string vertex, std::string pixel, std::string noBoneVertex, std::string notexPS)
{
	GameLib::skinnedMesh::loadShader(shader, vertex, pixel, noBoneVertex, notexPS);
}

void setLoopFlg(skinned_mesh* _mesh, const bool _loop_flg)
{
	GameLib::skinnedMesh::setLoopanimation(_mesh,_loop_flg);
}

void setStopAnimation(skinned_mesh* _mesh, const bool _is_stop)
{
	GameLib::skinnedMesh::setStopAnimation(_mesh, _is_stop);
}

void setStopTime(skinned_mesh* _mesh, const float _stop_time)
{
	GameLib::skinnedMesh::setStopTime(_mesh, _stop_time);
}

void setAnimFlame(skinned_mesh* _mesh, const int _anim_flame)
{
	GameLib::skinnedMesh::setAnimFlame(_mesh, _anim_flame);
}

const int getAnimFlame(skinned_mesh*_mesh)
{
	return GameLib::skinnedMesh::getAnimFlame(_mesh);
}

bool calcTransformedPosBySpecifyMesh(skinned_mesh* _mesh, DirectX::XMFLOAT3& _pos, std::string _mesh_name)
{
	return GameLib::skinnedMesh::calcTransformedPosBySpecifyMesh(_mesh, _pos, _mesh_name);
}

bool calcTransformedPosBySpecifyMesh(skinned_mesh* _mesh, DirectX::XMFLOAT3& _pos, std::string _mesh_name,bone_animation* anim)
{
	return GameLib::skinnedMesh::calcTransformedPosBySpecifyMesh(_mesh, _pos, _mesh_name,anim);
}

void FBXRender(skinned_mesh* _mesh, fbx_shader& hlsl, const DirectX::XMFLOAT4X4& SynthesisMatrix, const DirectX::XMFLOAT4X4& worldMatrix,float magnification, bool animation_flg, const DirectX::XMFLOAT4& materialColor, bool wireFlg)
{
	GameLib::skinnedMesh::skinnedMeshRender(_mesh, hlsl, magnification, SynthesisMatrix, worldMatrix, getCamPos(), getLineLight(), getPointLight(), materialColor, wireFlg,animation_flg);
}

void z_render(skinned_mesh* _mesh, fbx_shader& hlsl, const DirectX::XMFLOAT4X4& SynthesisMatrix, const DirectX::XMFLOAT4X4& worldMatrix)
{
	GameLib::skinnedMesh::z_render(_mesh, hlsl, SynthesisMatrix, worldMatrix);
}

void bloom_SRVrender(skinned_mesh* _mesh, fbx_shader& hlsl, const DirectX::XMFLOAT4X4& SynthesisMatrix, const DirectX::XMFLOAT4X4& worldMatrix, const DirectX::XMFLOAT4& judge_color,const DirectX::XMFLOAT4& materialColor)
{
	GameLib::skinnedMesh::bloom_SRVrender(_mesh, hlsl, SynthesisMatrix, worldMatrix, materialColor, judge_color);
}

void FBXRender(skinned_mesh* skinnedMesh, fbx_shader& hlsl,bone_animation* anim,const DirectX::XMFLOAT4X4&SynthesisMatrix, const DirectX::XMFLOAT4X4&worldMatrix, const DirectX::XMFLOAT4&materialColor, bool wireFlg)
{
	GameLib::skinnedMesh::skinnedMeshRender(skinnedMesh,hlsl,anim, SynthesisMatrix, worldMatrix,getCamPos(),getLineLight(),getPointLight(), materialColor, wireFlg);
}


void loadAnimation(bone_animation* _anim, std::string _anim_name)
{
	GameLib::skinnedMesh::loadAnimation(_anim, _anim_name);
}

void playAnimation(bone_animation* _anim,float magnification, bool _is_loop)
{
	GameLib::skinnedMesh::playAnimation(_anim, magnification, _is_loop);
}

void setStopAnimation(bone_animation* _anim, const bool _is_stop)
{
	GameLib::skinnedMesh::setStopAnimation(_anim, _is_stop);
}

void setStopTime(bone_animation* _anim, const float _stop_time)
{
	GameLib::skinnedMesh::setStopTime(_anim, _stop_time);
}

void setAnimFlame(bone_animation* _anim, const int _anim_flame)
{
	GameLib::skinnedMesh::setAnimFlame(_anim, _anim_flame);
}

const int getAnimFlame(bone_animation* _anim)
{
	return GameLib::skinnedMesh::getAnimFlame(_anim);
}



//Xinput_pad

int getState()
{
	return GameLib::input::xInput::getState();
}

int pressedButtons(int _padNum, int _button)
{
	return GameLib::input::xInput::pressedButtons(_padNum, _button);
}

DirectX::XMINT2 getThumbL(int _padNum)
{
	return GameLib::input::xInput::getThumbL(_padNum);
}
DirectX::XMINT2 getThumbR(int _padNum)
{
	return GameLib::input::xInput::getThumbR(_padNum);
}



int getKeyState(int _keyNum)
{
	return GameLib::input::keyboard::getState(_keyNum);
}
