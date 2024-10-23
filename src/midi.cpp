extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}
#include <iostream>
#include <string>
#include <windows.h>

static HMIDIOUT midiOut = nullptr;

// MIDI note on message (0x90 is the status byte for note on, 0x45 is the note
// (A4), 0x7f is the velocity)
inline void cplayNoteOn(BYTE note, BYTE velocity) {
  midiOutShortMsg(midiOut, 0x90 | (note << 8) | (velocity << 16));
}

// MIDI note off message (0x80 is the status byte for note off)
inline void cplayNoteOff(BYTE note) {
  midiOutShortMsg(midiOut, 0x80 | (note << 8));
}

int playNoteOn(lua_State *L) {
  BYTE note = (BYTE)luaL_checkinteger(L, 1);
  BYTE velocity = (BYTE)luaL_checkinteger(L, 2);
  cplayNoteOn(note, velocity);
  return 0;
}

int playNoteOff(lua_State *L) {
  BYTE note = (BYTE)luaL_checkinteger(L, 1);
  cplayNoteOff(note);
  return 0;
}

int midiInit(lua_State *L) {
  if (midiOutOpen(&midiOut, 0, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
    std::cerr << "Failed to open MIDI output device" << std::endl;
    return luaL_error(L, "Failed to open MIDI output device");
  }
  return 0;
}

int fini(lua_State *L) {
  if (midiOut) {
    midiOutClose(midiOut);
    midiOut = nullptr;
  }
  return 0;
}

int init(lua_State *L) {
  fini(L);
  if (midiOutOpen(&midiOut, 0, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
    return luaL_error(L, "Failed to open MIDI output device"); // 返回 Lua 错误
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
