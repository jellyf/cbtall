#pragma once
#include "LoadScene.h"
#include "SFSRequest.h"

LoadScene::LoadScene()
{
}


LoadScene::~LoadScene()
{
}

bool LoadScene::init()
{
	if (!Scene::init()) {
		return false;
	}
	return true;
}

void LoadScene::onEnter()
{
	scheduleUpdate();
	SFSRequest::getSingleton().Connect("115.84.179.242", 8636);
	Scene::onEnter();
}

void LoadScene::update(float delta)
{
	Scene::update(delta);
	SFSRequest::getSingleton().ProcessEvents();
}
