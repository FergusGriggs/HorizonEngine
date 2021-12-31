
//Function implementations for the Colour class

#pragma once

#include "colour.h"

namespace hrzn::gfx
{
	Colour::Colour() :
		m_colour(0)
	{
	}

	Colour::Colour(unsigned int numVal) :
		m_colour(numVal)
	{
	}

	Colour::Colour(BYTE r, BYTE g, BYTE b) :
		Colour(r, g, b, 255)
	{
	}

	Colour::Colour(BYTE r, BYTE g, BYTE b, BYTE a)
	{
		m_rgba[0] = r;
		m_rgba[1] = g;
		m_rgba[2] = b;
		m_rgba[3] = a;
	}

	Colour::Colour(const Colour& src) :
		m_colour(src.m_colour)
	{
	}

	Colour& Colour::operator=(const Colour& src)
	{
		m_colour = src.m_colour;
		return *this;
	}
	bool Colour::operator==(const Colour& rhs) const
	{
		return (m_colour == rhs.m_colour);
	}

	bool Colour::operator!=(const Colour& rhs) const
	{
		return !(*this == rhs);
	}

	constexpr BYTE Colour::getR() const
	{
		return m_rgba[0];
	}

	void Colour::setR(BYTE r)
	{
		m_rgba[0] = r;
	}

	constexpr BYTE Colour::getG() const
	{
		return m_rgba[1];
	}

	void Colour::setG(BYTE g)
	{
		m_rgba[1] = g;
	}

	constexpr BYTE Colour::getB() const
	{
		return m_rgba[2];
	}

	void Colour::setB(BYTE b)
	{
		m_rgba[2] = b;
	}

	constexpr BYTE Colour::getA() const
	{
		return m_rgba[3];
	}

	void Colour::setA(BYTE a)
	{
		m_rgba[3] = a;
	}

	unsigned int Colour::getUnsignedInt() const
	{
		return m_colour;
	}
}