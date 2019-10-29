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


//light//

void setLightAmbient(const DirectX::XMFLOAT4& _lightAmbient,const DirectX::XMFLOAT4& _lightColor)
{
	GameLib::light::setLineLight(_lightAmbient,_lightColor);
}
void setLightAmbient(const float x, const float y, const float z, const float w,const float r,const float g, const float b, const float a)
{
	GameLib::light::setLineLight({ x, y, z, w }, {r,g,b,a});
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


//skinned_mesh//
void loadFBX(skinned_mesh* skinnedMesh, const std::string& FBXName)
{
	GameLib::skinnedMesh::loadFBX(skinnedMesh, FBXName);
}

void setLoopFlg(skinned_mesh* _mesh, const bool _is_loop)
{
	GameLib::skinnedMesh::setLoopFlg(_mesh, _is_loop);
}
void setStopAnimation(skinned_mesh* _mesh, const bool _is_stop)
{
	GameLib::skinnedMesh::setStopAnimation(_mesh, _is_stop);
}

void setStopTime(skinned_mesh* _mesh, const int _stop_time)
{
	GameLib::skinnedMesh::setStopTime(_mesh, _stop_time);
}


void FBXRender(skinned_mesh* skinnedMesh, const DirectX::XMFLOAT4X4&SynthesisMatrix, const DirectX::XMFLOAT4X4&worldMatrix, const DirectX::XMFLOAT4&materialColor, bool wireFlg)
{
	GameLib::skinnedMesh::skinnedMeshRender(skinnedMesh, SynthesisMatrix, worldMatrix,getCamPos(),getLineLight(),getPointLight(), materialColor, wireFlg);
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
