//Keyboard.cpp
//Function implementations for the Keyboard class

#include "Keyboard.h"

Keyboard::Keyboard() {
	//Initialize key states
	for (int i = 0; i < 256; i++) {
		this->keyStates[i] = false;
	}
}

bool Keyboard::KeyIsPressed(const unsigned char keyCode) {
	return this->keyStates[keyCode];
}

bool Keyboard::KeyBufferIsEmpty() {
	return this->keyBuffer.empty();
}

bool Keyboard::CharBufferIsEmpty() {
	return this->charBuffer.empty();
}

KeyboardEvent Keyboard::ReadKey() {
	if (this->keyBuffer.empty()) {
		return KeyboardEvent();
	}
	else {
		KeyboardEvent keyboardEvent = this->keyBuffer.front();
		this->keyBuffer.pop();
		return keyboardEvent;
	}
}

unsigned char Keyboard::ReadChar() {
	if (this->charBuffer.empty()) {
		return 0u;
	}
	else {
		unsigned char charEvent = this->charBuffer.front();
		this->charBuffer.pop();
		return charEvent;
	}
}

void Keyboard::OnKeyPressed(const unsigned char key) {
	this->keyStates[key] = true;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::PRESS, key));
}

void Keyboard::OnKeyReleased(const unsigned char key) {
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::RELEASE, key));
}

void Keyboard::OnChar(const unsigned char key) {
	this->charBuffer.push(key);
}

void Keyboard::EnableAutoRepeatKeys() {
	this->autoRepeatKeys = true;
}

void Keyboard::DisableAutoRepeatKeys() {
	this->autoRepeatKeys = false;
}

void Keyboard::EnableAutoRepeatChars() {
	this->autoRepeatChars = true;
}

void Keyboard::DisableAutoRepeatChars() {
	this->autoRepeatChars = false;
}

bool Keyboard::IsKeysAutoRepeat() {
	return this->autoRepeatKeys;
}

bool Keyboard::IsCharsAutoRepeat() {
	return this->autoRepeatChars;
}