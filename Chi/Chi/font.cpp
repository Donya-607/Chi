#include "font.h"
#include "util.h"

namespace GameLib
{
	LoadFont fontTexture[] = {
		{ FONTNO::FONT0, L"./Data/fonts/font0.png" }, // フォント
		{ FONTNO::FONT1, L"./Data/fonts/font1.png" }, // フォント
		{ FONTNO::FONT2, L"./Data/fonts/font2.png" }, // フォント
		{ FONTNO::FONT3, L"./Data/fonts/font3.png" }, // フォント
		{ FONTNO::FONT4, L"./Data/fonts/font4.png" }, // フォント
		{ FONTNO::FONT5, L"./Data/fonts/font5.png" }, // フォント
		{ FONTNO::FONT6, L"./Data/fonts/font6.png" }, // フォント
		{ -1, nullptr }	// 終了フラグ
	};


	FontManager::FontManager() : font(nullptr)
	{
	}

	FontManager::~FontManager()
	{
		uninit();
	}

	void FontManager::init(ID3D11Device *device)
	{
		font = new SpriteBatch*[FONT_NUM];

		LoadFont *data = fontTexture;
		while ((size_t)data->fontNo < FONT_NUM)
		{
			font[data->fontNo] = new SpriteBatch(
				device, data->fileName, FONT_MAX_INSTANCE
			);
			data++;
		}
	}

	void FontManager::uninit()
	{
		releaseAll();
		safe_delete_array(font);
	}

	void FontManager::release(int i)
	{
		if ((size_t)i < FONT_NUM)
		{
			safe_delete(font[i]);
		}
	}

	void FontManager::releaseAll()
	{
		for (size_t i = 0; i < FONT_NUM; i++) release(i);
	}

	float FontManager::textOut(ID3D11DeviceContext *context, int fontNo,
		std::wstring str, float x, float y , float sx, float sy, float r, float g, float b, float a)
	{
		if (fontNo >= FONT_NUM) return -1;
		font[fontNo]->begin(context);
		float h = font[fontNo]->textout(str, x, y, sx, sy, r, g, b, a);
		font[fontNo]->end(context);
		return h;
	}
}