#pragma once

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

	constexpr BYTE GetR() const;
	void SetR(BYTE r);

	constexpr BYTE GetG() const;
	void SetG(BYTE g);

	constexpr BYTE GetB() const;
	void SetB(BYTE b);

	constexpr BYTE GetA() const;
	void SetA(BYTE a);

private:
	union
	{
		BYTE rgba[4];
		unsigned int colour;
	};
};

namespace Colours
{
	const Colour UnloadedTextureColour(100, 100, 100);
	const Colour UnloadedSpecularTextureColour(255, 255, 255);
	const Colour UnloadedNormalTextureColour(128, 128, 255);
	const Colour UnhandledTextureColour(255, 0, 0);
	const Colour UnloadedDepthTextureColour(0, 0, 0);
}