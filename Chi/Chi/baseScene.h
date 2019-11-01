#ifndef BASESCENE_H_
#define BASESCENE_H_


class baseScene
{
private:


protected:
	int timer;
	int state;
	bool isStack;
public:
	baseScene():timer(0),state(0),isStack(0) {}
	baseScene(baseScene& _copy)
	{
		timer = _copy.timer;
		state = _copy.state;
		isStack = _copy.isStack;
	}
	virtual ~baseScene() 
	{
	
	}
	virtual void init() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void uninit() {}
	virtual void imGui() {}
	bool getIsStack() { return isStack; }
	void setIsStack(bool _flg) { isStack = _flg; }
	virtual bool saveLightInfo() { return true; }
	virtual void loadLightInfo() {}
};

#endif // !BASESCENE_H_
