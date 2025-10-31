extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}
#include <stdexcept>
#include <windows.h>

class MidiOutDevice {
public:
  MidiOutDevice() = default;
  ~MidiOutDevice() { close(); }

  MidiOutDevice(const MidiOutDevice &) = delete;
  MidiOutDevice &operator=(const MidiOutDevice &) = delete;
  MidiOutDevice(MidiOutDevice &&other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
  }
  MidiOutDevice &operator=(MidiOutDevice &&other) noexcept {
    if (this != &other) {
      close();
      handle_ = other.handle_;
      other.handle_ = nullptr;
    }
    return *this;
  }

  void open(UINT deviceId = 0) {
    HMIDIOUT newHandle = nullptr;
    MMRESULT result = midiOutOpen(&newHandle, deviceId, 0, 0, CALLBACK_NULL);
    if (result != MMSYSERR_NOERROR) {
      throw std::runtime_error("Failed to open MIDI output device");
    }
    close();
    handle_ = newHandle;
  }

  void close() noexcept {
    if (handle_) {
      midiOutClose(handle_);
      handle_ = nullptr;
    }
  }

  bool isOpen() const noexcept { return handle_ != nullptr; }

  void sendShortMessage(DWORD message) const {
    if (!handle_) {
      throw std::runtime_error("MIDI output device not initialized");
    }
    MMRESULT result = midiOutShortMsg(handle_, message);
    if (result != MMSYSERR_NOERROR) {
      throw std::runtime_error("Failed to send MIDI message");
    }
  }

private:
  HMIDIOUT handle_ = nullptr;
};

static MidiOutDevice g_midiOutDevice;

inline DWORD makeShortMessage(BYTE status, BYTE data1, BYTE data2 = 0) {
  return static_cast<DWORD>(status) | (static_cast<DWORD>(data1) << 8) |
         (static_cast<DWORD>(data2) << 16);
}

// MIDI note on message (0x90 is the status byte for note on, 0x45 is the note
// (A4), 0x7f is the velocity)
inline void cplayNoteOn(BYTE note, BYTE velocity) {
  g_midiOutDevice.sendShortMessage(makeShortMessage(0x90, note, velocity));
}

// MIDI note off message (0x80 is the status byte for note off)
inline void cplayNoteOff(BYTE note) {
  g_midiOutDevice.sendShortMessage(makeShortMessage(0x80, note));
}

int playNoteOn(lua_State *L) {
  BYTE note = (BYTE)luaL_checkinteger(L, 1);
  BYTE velocity = (BYTE)luaL_checkinteger(L, 2);
  try {
    cplayNoteOn(note, velocity);
  } catch (const std::exception &e) {
    return luaL_error(L, e.what());
  }
  return 0;
}

int playNoteOff(lua_State *L) {
  BYTE note = (BYTE)luaL_checkinteger(L, 1);
  try {
    cplayNoteOff(note);
  } catch (const std::exception &e) {
    return luaL_error(L, e.what());
  }
  return 0;
}

int fini(lua_State *L) {
  g_midiOutDevice.close();
  return 0;
}

int init(lua_State *L) {
  try {
    g_midiOutDevice.open();
  } catch (const std::exception &e) {
    return luaL_error(L, e.what());
  }
  return 0;
}

extern "C" __declspec(dllexport) int luaopen_midi(lua_State *L) {
  static const luaL_Reg midi[] = {{"init", init},
                                  {"fini", fini},
                                  {"playNoteOn", playNoteOn},
                                  {"playNoteOff", playNoteOff},
                                  {NULL, NULL}};
  luaL_newlib(L, midi);
  return 1;
}
