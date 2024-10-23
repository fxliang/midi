-- for windows only
local script_cpath = rime_api.get_user_data_dir() .. '/lua/?.dll'
package.cpath = package.cpath .. ';' .. script_cpath
-------------------------------------------------------------------------------
local midi = require("midi")
-------------------------------------------------------------------------------
-- a sample with xiaoxingxing
-- audio midi notes data
local _1 = 60
local _2 = 62
local _3 = 64
local _4 = 65
local _5 = 67
local _6 = 69

local song = {
	_1, _1, _5, _5, _6, _6, _5,
	_4, _4, _3, _3, _2, _2, _1,
	_5, _5, _4, _4, _3, _3, _2,
	_5, _5, _4, _4, _3, _3, _2,
	_1, _1, _5, _5, _6, _6, _5,
	_4, _4, _3, _3, _2, _2, _1
}
--[[
-- you can also import from external lua file (in package.path)
-- local song = require("kv331") -- kv331.lua in cwd
-- local song = require("songs.kv331") -- kv331.lua in ./songs
]]

-------------------------------------------------------------------------------
-- init keyStates
local keyStates = {}
for i = 1, 256 do
  keyStates[i] = false
end
-------------------------------------------------------------------------------
local index = 1
local count = #song
local note = 0
-------------------------------------------------------------------------------

local function get_key()
  local res = 0
  if index < count then
    res = song[index]
    index = index + 1
  else
    index = 1
    res = song[index]
  end
  return res
end

-------------------------------------------------------------------------------
--- librime-lua functions
-------------------------------------------------------------------------------
local function init(env)
  midi.init()
  env.kRejected, env.kAccepted, env.kNoop = 0, 1, 2
end

local function fini(env)
  midi.fini()
end

local function func(key, env)
  if key.keycode == 0 then return env.kNoop end
  local keycode = key.keycode
  -- return to reset
  if key:repr() == 'Return' then
    index = 1
    return env.kNoop
  end
  -- this example, only for key a-z, or A-Z
  -- for number keys, (keycode <= 0x39 and keycode >=0x30)
  -- other keys, you need to reference librime documents for exactly values
  -- or determine it with key:repr()
  if (keycode >= 0x41 and keycode <= 0x5a) or (keycode >= 0x61 and keycode <= 0x7a) then
    if key:release() then
      if keyStates[key.keycode] then  -- only when this key is down
        --midi.playNoteOff(note)
        keyStates[key.keycode] = false
      end
    else
      if keyStates[key.keycode] == false then -- only when this key is not down
        note = get_key()
        midi.playNoteOn(note, 0x7f) -- max velocity
        keyStates[key.keycode] = true
      end
    end
  end
  return env.kNoop
end

return {init = init, func = func, fini = fini}
