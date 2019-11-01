#ifndef SYSTEM_H_
#define SYSTEM_H_

class System
{
private:
	System() {}
	~System() {}
public:
	//âÊñ ÉTÉCÉY
	int SCREEN_WIDTH = 1280;
	int SCREEN_HEIGHT = 720;

	float alpha = 1.0f;

	const int TEXTURE_NUM = 256U;
	const int DEBUG_INSTANCE_NUM = 8192U;
	const int PRIMITIVE_BATCH_INSTANCE_NUM = 4096U;

	static System* getInstance()
	{
		static System instance;
		return &instance;
	}
};
#define pSystem System::getInstance()

#endif // !SYSTEM_H_
