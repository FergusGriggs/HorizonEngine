//Colour.cpp
//Function implementations for the Colour class

#include "Colour.h"

#pragma once

Colour::Colour() : colour(0)
{
}

Colour::Colour(unsigned int numVal) : colour(numVal)
{
}

Colour::Colour(BYTE r, BYTE g, BYTE b) : Colour(r, g, b, 255)
{
}

Colour::Colour(BYTE r, BYTE g, BYTE b, BYTE a)
{
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
}

Colour::Colour(const Colour& src) : colour(src.colour)
{
}

Colour& Colour::operator=(const Colour& src)
{
	this->colour = src.colour;
	return *this;
}
bool Colour::operator==(const Colour& rhs) const
{
	return (this->colour == rhs.colour);
}

bool Colour::operator!=(const Colour& rhs) const
{
	return !(*this == rhs);
}

constexpr BYTE Colour::GetR() const
{
	return this->rgba[0];
}

void Colour::SetR(BYTE r)
{
	this->rgba[0] = r;
}

constexpr BYTE Colour::GetG() const
{
	return this->rgba[1];
}

void Colour::SetG(BYTE g)
{
	this->rgba[1] = g;
}

constexpr BYTE Colour::GetB() const
{
	return this->rgba[2];
}

void Colour::SetB(BYTE b)
{
	this->rgba[2] = b;
}

constexpr BYTE Colour::GetA() const
{
	return this->rgba[3];
}

void Colour::SetA(BYTE a)
{
	this->rgba[3] = a;
}