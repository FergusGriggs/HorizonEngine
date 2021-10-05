#pragma once
class KeyboardEvent
{
public:
	enum EventType {
		PRESS,
		RELEASE,
		INVALID
	};

	KeyboardEvent();
	KeyboardEvent(const EventType type, const unsigned char key);
	bool IsPressed() const;
	bool IsReleased() const;
	bool IsValid() const;
	unsigned char GetKeyCode() const;

private:
	EventType type;
	unsigned char key;
};

