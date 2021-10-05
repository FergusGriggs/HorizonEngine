#pragma once

#include "MouseEvent.h"

#include <queue>

class Mouse
{
public:
	Mouse();

	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnMiddlePressed(int x, int y);
	void OnMiddleReleased(int x, int y);
	void OnScrollUp(int x, int y);
	void OnScrollDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnMouseMoveRaw(int x, int y);

	bool IsLeftDown();
	bool IsRightDown();
	bool IsMiddleDown();

	int GetPosX();
	int GetPosY();
	MousePos GetPos();

	bool EventBufferIsEmpty();
	MouseEvent ReadEvent();

private:
	std::queue<MouseEvent> eventBuffer;
	bool leftIsDown = false;
	bool rightIsDown = false;
	bool middleIsDown = false;
	MousePos pos;
};

