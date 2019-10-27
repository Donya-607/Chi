#include "scene.h"
#include "sceneManager.h"



struct SceneGame::Impl
{
public:

public:
	Impl()
	{

	}
	~Impl() = default;
public:
	void Init()
	{

	}
	void Uninit()
	{

	}

	void Update()
	{

	}

	void Draw() const
	{

	}

public:
	void UseImGui()
	{

	}
};

SceneGame::SceneGame() : pImpl( std::make_unique<Impl>() )
{}
SceneGame::~SceneGame()
{
	pImpl.reset( nullptr );
}

void SceneGame::init()
{
	pImpl->Init();
}
void SceneGame::uninit()
{
	pImpl->Uninit();
}

void SceneGame::update()
{
	pImpl->Update();
}

void SceneGame::render()
{
	pImpl->Draw();
}

void SceneGame::imGui()
{
	pImpl->UseImGui();
}
