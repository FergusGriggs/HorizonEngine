
//Stores a byte array and an unsigned int which both store the same rgba colour

#pragma once

namespace hrzn::gfx
{
	typedef unsigned char BYTE;

	class Colour
	{
	public:
		Colour();
		Colour(unsigned int numVal);
		Colour(BYTE r, BYTE g, BYTE b);
		Colour(BYTE r, BYTE g, BYTE b, BYTE a);
		Colour(const Colour& src);

		Colour& operator=(const Colour& src);
		bool operator==(const Colour& rhs) const;
		bool operator!=(const Colour& rhs) const;

		constexpr BYTE getR() const;
		void setR(BYTE r);

		constexpr BYTE getG() const;
		void setG(BYTE g);

		constexpr BYTE getB() const;
		void setB(BYTE b);

		constexpr BYTE getA() const;
		void setA(BYTE a);

	private:
		union
		{
			BYTE         m_rgba[4];
			unsigned int m_colour;
		};
	};

	namespace colours
	{
		static const Colour sc_unloadedTextureColour(100, 100, 100);
		static const Colour sc_unloadedSpecularTextureColour(128, 128, 128);
		static const Colour sc_unloadedNormalTextureColour(128, 128, 255);
		static const Colour sc_unhandledTextureColour(255, 0, 0);
		static const Colour sc_unloadedDepthTextureColour(0, 0, 0);
	}
}