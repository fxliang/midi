-- for windows only
-- current directory of script
local script_dir = debug.getinfo(1, "S").source:match("@(.*[/\\])")
-- add DLL in cwd to package.cpath
package.cpath = package.cpath .. ";" .. script_dir .. "?.dll"

print(package.cpath)
local midi = require("midi")
-- print(midi)  -- check if midi is loaded

local function sleep(n)
  os.execute("sleep " .. tonumber(n))
end

midi.init()
midi.playNoteOn(42, 0x7f)
sleep(1)
midi.playNoteOff(42)
midi.fini()
