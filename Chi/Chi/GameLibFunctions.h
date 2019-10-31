#ifndef GAMELIBFUNCTIONS_H_
#define GAMELIBFUNCTIONS_H_

#include "gameLib.h"
#include "vector.h"

//library//
void startUpLibrary(LPCTSTR caption, HINSTANCE instance, int width = 640, int height = 480, bool isFullscreen = false, int iconNum = -1, double frameRate = 0.0);
void startUpLibrary(LPCTSTR caption, HINSTANCE instance, DirectX::XMINT2 screenSize = { 640,480 }, bool isFullscreen = false, int iconNum = -1, double frameRate = 0.0);

void shutDownLibrary();

DirectX::XMINT2 getWindowSize();

//ClearWindow//
void clearWindow(const float r = 1.0f, const float g = 1.0f, const float b = 1.0f, const float a = 1.0f);
void clearWindow(const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });


//BLENDMODE//

void setBlendMode_NONE(const float alpha);

void setBlendMode_ALPHA(const float alpha);

void setBlendMode_ADD(const float alpha);

void setBlendMode_SUBTRACT(const float alpha);

void setBlendMode_REPLACE(const float alpha);

void setBlendMode_MULTIPLY(const float alpha);

void setBlendMode_LIGHTEN(const float alpha);

void setBlendMode_DARKEN(const float alpha);

void setBlendMode_SCREEN(const float alpha);


//DEBUG//
template <typename ... Args>
void setString(DirectX::XMINT2 _pos, const std::wstring& fmt, Args ... args)
{
	size_t len = swprintf(nullptr, 0, fmt.c_str(), args ...);
	std::vector<wchar_t> buf(len + 1);
	swprintf(&buf[0], len + 1, fmt.c_str(), args ...);
	std::wstring str(&buf[0], &buf[0] + len);
	GameLib::debug::drawDebug(_pos, str);
}




//PRIMITIVE//
void drawRect(const float x, const float y, const float w, const float h, const float cx = .0f, const float cy = .0f, const float angle = .0f, const float r = 1.0f, const float g = 1.0f, const float b = 1.0f);
void drawRect(const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT2& center = { .0f,.0f }, const float angle = .0f, const DirectX::XMFLOAT3& color = { 1.0f,1.0f,1.0f });

void drawLine(const float x1, const float y1, const float x2, const float y2, const float width = 1.0f, const float r = .0f, const float g = .0f, const float b = .0f);
void drawLine(const DirectX::XMFLOAT2& begin, const DirectX::XMFLOAT2& end, const float width = 1.0f, const DirectX::XMFLOAT3& color = { 1.0f,1.0f,1.0f });

void drawCircle(const float x, const float y, const float radius = 1.0f, const float r = 1.0f, const float g = 1.0f, const float b = 1.0f, int n = 32);
void drawCircle(const DirectX::XMFLOAT2& pos, const float radius = 1.0f, const DirectX::XMFLOAT3& color = { 1.0f,1.0f,1.0f }, const int n = 32);

void drawQuad(const DirectX::XMFLOAT2(&v)[4], const float r = 1.0f, const float g = 1.0f, const float b = 1.0f);
//void drawQuad(const DirectX::XMFLOAT2(&v)[4], const DirectX::XMFLOAT3& color = { 1.0f,1.0f,1.0f });



//SPRITE//

void spriteLoad(Sprite* _sprite, std::wstring _spriteName);

void spriteRender(Sprite* _sprite, const float pos_x, const float pos_y, bool _tempX = false, bool _tempY = false);
void spriteRender(Sprite* _sprite, const DirectX::XMFLOAT2& pos, bool _tempX = false, bool _tempY = false);

void spriteRenderRect(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, bool _tempX = false, bool _tempY = false);
void spriteRenderRect(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, bool _tempX = false, bool _tempY = false);

void spriteRenderExtend(Sprite* _sprite, const float pos_x, const float pos_y, const float Magnification = 1.0f, bool _tempX = false, bool _tempY = false);
void spriteRenderExtend(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const float Magnification = 1.0f, bool _tempX = false, bool _tempY = false);

void spriteRenderExtend2(Sprite* _sprite, const float pos_x, const float pos_y, const float Magnification_x = 1.0f, const float Magnification_y = 1.0f, bool _tempX = false, bool _tempY = false);
void spriteRenderExtend2(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& Magnification = { 1.0f,1.0f }, bool _tempX = false, bool _tempY = false);

void spriteRenderExtend3(Sprite* _sprite, const float pos_x, const float pos_y, const float centerPos_x, const float centerPos_y, const float Magnification_x = 1.0f, const float Magnification_y = 1.0f, bool _tempX = false, bool _tempY = false);
void spriteRenderExtend3(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& centerPos, const DirectX::XMFLOAT2& Magnification = { 1.0f,1.0f }, bool _tempX = false, bool _tempY = false);

void spriteRenderRectExtend(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float Magnification = 1.0f, bool _tempX = false, bool _tempY = false);
void spriteRenderRectExtend(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const float Magnification = 1.0f, bool _tempX = false, bool _tempY = false);

void spriteRenderRectExtend2(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float Magnification_x = 1.0f, const float Magnification_y = 1.0f, bool _tempX = false, bool _tempY = false);
void spriteRenderRectExtend2(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const DirectX::XMFLOAT2& Magnification = { 1.0f,1.0f }, bool _tempX = false, bool _tempY = false);

void spriteRenderRectExtend3(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float centerPos_x, const float centerPos_y, const float Magnification_x = 1.0f, const float Magnification_y = 1.0f, bool _tempX = false, bool _tempY = false);
void spriteRenderRectExtend3(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const DirectX::XMFLOAT2& centerPos, const DirectX::XMFLOAT2& Magnification = { 1.0f,1.0f }, bool _tempX = false, bool _tempY = false);

void spriteRenderRota(Sprite* _sprite, const float pos_x, const float pos_y, const float Magnification = 1.0f, float angle = .0f, bool _tempX = false, bool _tempY = false);
void spriteRenderRota(Sprite* _sprite, const DirectX::XMFLOAT2 pos, const float Magnification = 1.0f, float angle = .0f, bool _tempX = false, bool _tempY = false);

void spriteRenderRota2(Sprite* _sprite, const float pos_x, const float pos_y, const float Magnification_x = 1.0f, const float Magnification_y = 1.0f, float angle = .0f, bool _tempX = false, bool _tempY = false);
void spriteRenderRota2(Sprite* _sprite, const DirectX::XMFLOAT2 pos, const DirectX::XMFLOAT2 Magnification = { 1.0f,1.0f }, float angle = .0f, bool _tempX = false, bool _tempY = false);

void spriteRenderRota3(Sprite* _sprite, const float pos_x, const float pos_y, const float centerPos_x = .0f, const float centerPos_y = .0f, const float Magnification_x = 1.0f, const float Magnification_y = 1.0f, float angle = .0f, bool _tempX = false, bool _tempY = false);
void spriteRenderRota3(Sprite* _sprite, const DirectX::XMFLOAT2 pos, const DirectX::XMFLOAT2& centerPos = { .0f,.0f }, const DirectX::XMFLOAT2 Magnification = { 1.0f,1.0f }, float angle = .0f, bool _tempX = false, bool _tempY = false);

void spriteRenderRectRota(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float angle = 0, bool _tempX = false, bool _tempY = false);
void spriteRenderRectRota(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const float angle = 0, bool _tempX = false, bool _tempY = false);

void spriteRenderRectRota2(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float Magnification_x = 1.0f, const float Magnification_y = 1.0f, const float angle = .0f, bool _tempX = false, bool _tempY = false);
void spriteRenderRectRota2(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const DirectX::XMFLOAT2& Magnification = { 1.0f,1.0f }, const float angle = .0f, bool _tempX = false, bool _tempY = false);

void spriteRenderRectRota3(Sprite* _sprite, const float pos_x, const float pos_y, const float texPos_x, const float texPos_y, const float texSize_x, const float texSize_y, const float centerPos_x = .0f, const float centerPos_y = .0f, const float Magnification_x = 1.0f, const float Magnification_y = 1.0f, const float angle = .0f, bool _tempX = false, bool _tempY = false);
void spriteRenderRectRota3(Sprite* _sprite, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const DirectX::XMFLOAT2& centerPos = { .0f,.0f }, const DirectX::XMFLOAT2& Magnification = { 1.0f,1.0f }, const float angle = .0f, bool _tempX = false, bool _tempY = false);


//CAMERA//
DirectX::XMMATRIX getProjectionMatrix();

DirectX::XMMATRIX getViewMatrix();

void setCamPos(const DirectX::XMFLOAT3& pos);

void setCamPos(const float x, const float y, const float z);

void setTarget(const DirectX::XMFLOAT3& _target);

void setTarget(const float x, const float y, const float z);

DirectX::XMFLOAT4 getCamPos();

//light//
void setLightAmbient(const DirectX::XMFLOAT4& _lightAmbient, const DirectX::XMFLOAT4& lightColor);
void setLightAmbient(const float x, const float y, const float z, const float w, const float r, const float g, const float b, const float a);

line_light& getLineLight();

void setPointLight(const PointLight& _info);
void setPointlightInfo(const int index, const PointLight& _info);
void removePointLight(const int i);
void resetPointLight();
std::vector<point_light>& getPointLight();


//static_mesh//
void createCube(static_mesh* cube);

void createSphere(static_mesh* sphere, u_int slice, u_int stack);

void createPlane(static_mesh* _plane, u_int _vertical = 1, u_int _side = 1);

void loadOBJ(static_mesh* staticMesh, const wchar_t* objName);

void loadOBJ_MTL(static_mesh* staticMesh, wchar_t* objName, wchar_t* mtlName);

static_mesh::primitive_material& getPrimitiveMaterial(static_mesh* _mesh);

void OBJRender(static_mesh* staticMesh, const DirectX::XMFLOAT4X4& SynthesisMatrix, const DirectX::XMFLOAT4X4& worldMatrix, const DirectX::XMFLOAT4& materialColor = { 1.0f,1.0f,1.0f,1.0f }, bool wireFlg = false);



//skinned_mesh//
void loadFBX(skinned_mesh* skinnedMesh, const std::string& FBXName);

void setLoopFlg(skinned_mesh* _mesh, const bool _is_loop);

void setStopAnimation(skinned_mesh* _mesh, const bool _is_stop);

void setStopTime(skinned_mesh* _mesh, const int _stop_time);

void setAnimFlame(skinned_mesh* _mesh, const int _anim_flame);

const int getAnimFlame(skinned_mesh*);

bool calcTransformedPosBySpecifyMesh(skinned_mesh* _mesh, DirectX::XMFLOAT3& _pos, std::string _mesh_name);

void FBXRender(skinned_mesh* skinnedMesh, const DirectX::XMFLOAT4X4& SynthesisMatrix, const DirectX::XMFLOAT4X4& worldMatrix, const DirectX::XMFLOAT4& materialColor = { 1.0f,1.0f,1.0f,1.0f }, bool wireFlg = false);


//Xinput_Pad
int getState();

/// <summary>
/// -1 : ó£ÇµÇΩèuä‘Å@0 : âüÇµÇƒÇ»Ç¢Å@1 : âüÇµÇΩèuä‘ Å@2 : âüÇµÇ¡ÇœÇ»Çµ
/// </summary>
int pressedButtons(int _padNum, int _button);

DirectX::XMINT2 getThumbL(int _padNum);
DirectX::XMINT2 getThumbR(int _padNum);


//keyborad

/// <summary>
/// -1 : ó£ÇµÇΩèuä‘Å@0 : âüÇµÇƒÇ»Ç¢Å@1 : âüÇµÇΩèuä‘ Å@2 : âüÇµÇ¡ÇœÇ»Çµ
/// </summary>
int getKeyState(int _keyNum);


#endif // !GAMELIBFUNCTIONS_H_
