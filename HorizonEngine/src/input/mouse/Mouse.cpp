#include "Mouse.h"

Mouse::Mouse() {

}

void Mouse::OnLeftPressed(int x, int y) {
	this->leftIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::LEFT_PRESS, x, y));
}

void Mouse::OnLeftReleased(int x, int y) {
	this->leftIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::LEFT_RELEASE, x, y));
}

void Mouse::OnRightPressed(int x, int y) {
	this->rightIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RIGHT_PRESS, x, y));
}

void Mouse::OnRightReleased(int x, int y) {
	this->rightIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RIGHT_RELEASE, x, y));
}

void Mouse::OnMiddlePressed(int x, int y) {
	this->middleIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MIDDLE_PRESS, x, y));
}

void Mouse::OnMiddleReleased(int x, int y) {
	this->middleIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MIDDLE_RELEASE, x, y));
}

void Mouse::OnScrollUp(int x, int y) {
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::SCROLL_UP, x, y));
}

void Mouse::OnScrollDown(int x, int y) {
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::SCROLL_DOWN, x, y));
}

void Mouse::OnMouseMove(int x, int y) {
	this->pos.x = x;
	this->pos.y = y;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MOVE, x, y));
}

void Mouse::OnMouseMoveRaw(int x, int y) {
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RAW_MOVE, x, y));
}

bool Mouse::IsLeftDown() {
	return leftIsDown;
}

bool Mouse::IsRightDown() {
	return rightIsDown;
}

bool Mouse::IsMiddleDown() {
	return middleIsDown;
}

int Mouse::GetPosX() {
	return this->pos.x;
}

int Mouse::GetPosY() {
	return this->pos.y;
}

MousePos Mouse::GetPos() {
	return this->pos;
}

bool Mouse::EventBufferIsEmpty() {
	return this->eventBuffer.empty();
}

MouseEvent Mouse::ReadEvent() {
	if (this->eventBuffer.empty()) {
		return MouseEvent();
	}
	else {
		MouseEvent mouseEvent = this->eventBuffer.front();
		this->eventBuffer.pop();
		return mouseEvent;
	}
}