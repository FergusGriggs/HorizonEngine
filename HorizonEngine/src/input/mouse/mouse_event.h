#pragma once

namespace hrzn::input
{
	struct MousePos
	{
		MousePos() :
			x(0),
			y(0)
		{
		}

		MousePos(int x, int y) :
			x(x),
			y(y)
		{
		}

		int x;
		int y;
	};

	class MouseEvent
	{
	public:
		enum class EventType
		{
			eLeftPress,
			eLeftRelease,

			eRightPress,
			eRightRelease,

			eMiddlePress,
			eMiddleRelease,

			eScrollUp,
			eScrollDown,

			eMove,
			eRawMove,

			eInvalid
		};

	public:
		MouseEvent();
		MouseEvent(const EventType type, const int x, const int y);

		bool            isValid() const;
		EventType       getType() const;
		const MousePos& getPos() const;

	private:
		EventType m_type;
		MousePos  m_pos;
	};
}
