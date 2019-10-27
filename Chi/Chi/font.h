#ifndef FONT_H_
#define FONT_H_

#include <Windows.h>
#include "sprite.h"

namespace GameLib
{
	const UINT FONT_NUM = 7;
	const UINT FONT_MAX_INSTANCE = 256;

	// �t�H���g���[�h�f�[�^
	struct LoadFont
	{
		int             fontNo;     // �e�N�X�`���ԍ�
		const wchar_t*  fileName;   // �t�@�C����
	};

	// ���x����`
	enum FONTNO
	{
		// �^�C�g��
		FONT0 = 0,
		FONT1,
		FONT2,
		FONT3,
		FONT4,
		FONT5,
		FONT6,
	};

	//==========================================================================
	//
	//      �t�H���g�}�l�[�W��
	//
	//==========================================================================
	class FontManager
	{

	private:
		FontManager();
		~FontManager();
		SpriteBatch** font;

	public:
		void init(ID3D11Device*);
		void uninit();
		void release(int);
		void releaseAll();

		float textOut(ID3D11DeviceContext*, int,
			std::wstring,
			float, float, float, float,
			float, float, float, float);

		static FontManager* getInstance()
		{
			static FontManager instance;
			return &instance;
		}
	};
}



#endif // !FONT_H_
