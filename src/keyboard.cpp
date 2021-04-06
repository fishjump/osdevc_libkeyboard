#include <keyboard/keyboard.hpp>

namespace {

using namespace system::io::entity;

const size_t          MAX_BUFFER_SIZE = 4096;
uint32_t              bufferBegin     = 0;
uint32_t              bufferEnd       = 1;
size_t                bufferCount     = 0;
KeyboardEvent         buffer[MAX_BUFFER_SIZE];
KeyStatus             capsLockStatus = KeyStatus::Up;
KeyboardEventHandler *handler        = nullptr;

const KeyboardEvent keyboardScanCodeTable[] = {
    {Key::None, KeyStatus::Unknown},          // 0x00 <undefined>
    {Key::Escape, KeyStatus::Down},           // 0x01 escape pressed
    {Key::D1, KeyStatus::Down},               // 0x02 1 pressed
    {Key::D2, KeyStatus::Down},               // 0x03 2 pressed
    {Key::D3, KeyStatus::Down},               // 0x04 3 pressed
    {Key::D4, KeyStatus::Down},               // 0x05 4 pressed
    {Key::D5, KeyStatus::Down},               // 0x06 5 pressed
    {Key::D6, KeyStatus::Down},               // 0x07 6 pressed
    {Key::D7, KeyStatus::Down},               // 0x08 7 pressed
    {Key::D8, KeyStatus::Down},               // 0x09 8 pressed
    {Key::D9, KeyStatus::Down},               // 0x0a 9 pressed
    {Key::D0, KeyStatus::Down},               // 0x0b 0 pressed
    {Key::OemMinus, KeyStatus::Down},         // 0x0c - pressed
    {Key::OemEnlw, KeyStatus::Down},          // 0x0d = pressed
    {Key::Back, KeyStatus::Down},             // 0x0e backspace pressed
    {Key::Tab, KeyStatus::Down},              // 0x0f tab pressed
    {Key::Q, KeyStatus::Down},                // 0x10 q pressed
    {Key::W, KeyStatus::Down},                // 0x11 w pressed
    {Key::E, KeyStatus::Down},                // 0x12 e pressed
    {Key::R, KeyStatus::Down},                // 0x13 r pressed
    {Key::T, KeyStatus::Down},                // 0x14 t pressed
    {Key::Y, KeyStatus::Down},                // 0x15 y pressed
    {Key::U, KeyStatus::Down},                // 0x16 u pressed
    {Key::I, KeyStatus::Down},                // 0x17 i pressed
    {Key::O, KeyStatus::Down},                // 0x18 o pressed
    {Key::P, KeyStatus::Down},                // 0x19 p pressed
    {Key::OemOpenBrackets, KeyStatus::Down},  // 0x1a [ pressed
    {Key::OemCloseBrackets, KeyStatus::Down}, // 0x1b ] pressed
    {Key::Enter, KeyStatus::Down},            // 0x1c enter pressed
    {Key::LeftCtrl, KeyStatus::Down},         // 0x1d left ctrl pressed
    {Key::A, KeyStatus::Down},                // 0x1e a pressed
    {Key::S, KeyStatus::Down},                // 0x1f s pressed
    {Key::D, KeyStatus::Down},                // 0x20 d pressed
    {Key::F, KeyStatus::Down},                // 0x21 f pressed
    {Key::G, KeyStatus::Down},                // 0x22 g pressed
    {Key::H, KeyStatus::Down},                // 0x23 h pressed
    {Key::J, KeyStatus::Down},                // 0x24 j pressed
    {Key::K, KeyStatus::Down},                // 0x25 k pressed
    {Key::L, KeyStatus::Down},                // 0x26 l pressed
    {Key::OemSemicolon, KeyStatus::Down},     // 0x27 ; pressed
    {Key::OemQuotes, KeyStatus::Down},        // 0x28 ' pressed
    {Key::OemTilde, KeyStatus::Down},         // 0x29 ` pressed
    {Key::LeftShift, KeyStatus::Down},        // 0x2a left shift pressed
    {Key::OemBackslash, KeyStatus::Down},     // 0x2b \ pressed
    {Key::Z, KeyStatus::Down},                // 0x2c z pressed
    {Key::X, KeyStatus::Down},                // 0x2d x pressed
    {Key::C, KeyStatus::Down},                // 0x2e c pressed
    {Key::V, KeyStatus::Down},                // 0x2f v pressed
    {Key::B, KeyStatus::Down},                // 0x30 b pressed
    {Key::N, KeyStatus::Down},                // 0x31 n pressed
    {Key::M, KeyStatus::Down},                // 0x32 m pressed
    {Key::OemComma, KeyStatus::Down},         // 0x33 , pressed
    {Key::OemPeriod, KeyStatus::Down},        // 0x34 .  pressed
    {Key::OemQuestion, KeyStatus::Down},      // 0x35 / pressed
    {Key::RightShift, KeyStatus::Down},       // 0x36 right shift pressed
    {Key::Multiply, KeyStatus::Down},         // 0x37 (keypad) * pressed
    {Key::LeftAlt, KeyStatus::Down},          // 0x38 left alt pressed
    {Key::Space, KeyStatus::Down},            // 0x39 space pressed
    {Key::CapsLock, KeyStatus::Down},         // 0x3a CapsLock pressed
    {Key::F1, KeyStatus::Down},               // 0x3b F1 pressed
    {Key::F2, KeyStatus::Down},               // 0x3c F2 pressed
    {Key::F3, KeyStatus::Down},               // 0x3d F3 pressed
    {Key::F4, KeyStatus::Down},               // 0x3e F4 pressed
    {Key::F5, KeyStatus::Down},               // 0x3f F5 pressed
    {Key::F6, KeyStatus::Down},               // 0x40 F6 pressed
    {Key::F7, KeyStatus::Down},               // 0x41 F7 pressed
    {Key::F8, KeyStatus::Down},               // 0x42 F8 pressed
    {Key::F9, KeyStatus::Down},               // 0x43 F9 pressed
    {Key::F10, KeyStatus::Down},              // 0x44 F10 pressed
};

} // namespace

namespace system::io::entity {

void Keyboard::setHandler(KeyboardEventHandler _handler) {
    handler = _handler;
}

void Keyboard::serve() {
    while(!empty()) {
        if(handler != nullptr) {
            KeyboardEvent event = dequeue();
            handler(event);
        }
    }
}

void Keyboard::enqueue(uint8_t keyCode) {
    if(bufferCount < MAX_BUFFER_SIZE) {
        buffer[bufferEnd] = keyboardScanCodeTable[keyCode];
        bufferEnd         = (bufferEnd + 1) % MAX_BUFFER_SIZE;
        bufferCount++;
    }
}

KeyboardEvent Keyboard::dequeue() {
    if(!empty()) {
        bufferCount--;
        KeyboardEvent event = front();
        bufferBegin         = (bufferBegin + 1) % MAX_BUFFER_SIZE;
        return event;
    }

    return {Key::None, KeyStatus::Unknown, KeyStatus::Unknown};
}

KeyboardEvent Keyboard::front() const {
    KeyboardEvent event = buffer[bufferBegin];
    return {event.key, event.status, capsLockStatus};
}

KeyboardEvent Keyboard::end() const {
    return buffer[(bufferEnd - 1) % MAX_BUFFER_SIZE];
}

bool Keyboard::empty() const {
    return 0 == bufferCount;
}

size_t Keyboard::count() const {
    return bufferCount;
}

} // namespace system::io::entity
