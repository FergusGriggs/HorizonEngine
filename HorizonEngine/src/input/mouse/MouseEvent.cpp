#include "MouseEvent.h"

MousePos::MousePos() : x(0), y(0)
{
}

MousePos::MousePos(int x, int y) : x(x), y(y)
{
}

MouseEvent::MouseEvent() : type(MouseEvent::EventType::INVALID), pos({0, 0})
{
}

MouseEvent::MouseEvent(const MouseEvent::EventType type, const int x, const int y) : type(type), pos({x, y})
{
}

bool MouseEvent::IsValid() const {
	return this->type != EventType::INVALID;
}

MouseEvent::EventType MouseEvent::GetType() const {
	return this->type;
}

MousePos MouseEvent::GetPos() const {
	return pos;
}

int MouseEvent::GetPosX() const {
	return pos.x;
}

int MouseEvent::GetPosY() const {
	return pos.y;
}