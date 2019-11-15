#pragma once

struct MousePos {
	int x;
	int y;

	MousePos();
	MousePos(int x, int y);
};

class MouseEvent
{
public:
	enum class EventType {
		LEFT_PRESS,
		LEFT_RELEASE,
		RIGHT_PRESS,
		RIGHT_RELEASE,
		MIDDLE_PRESS,
		MIDDLE_RELEASE,
		SCROLL_UP,
		SCROLL_DOWN,
		MOVE,
		RAW_MOVE,
		INVALID
	};

private:
	EventType type;
	MousePos pos;

public:
	MouseEvent();
	MouseEvent(const EventType type, const int x, const int y);
	bool IsValid() const;
	EventType GetType() const;
	MousePos GetPos() const;
	int GetPosX() const;
	int GetPosY() const;
};

