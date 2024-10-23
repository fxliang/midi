# midi: a simple lua module for midi on Windows

# how to build

## msvc

```bat
git clone -v --recursive https://github.com/fxliang/midi.git
cd midi
xmake f -p windows
xmake 
```

## mingw

```bat
git clone -v --recursive https://github.com/fxliang/midi.git
cd midi
xmake f -p mingw
xmake 
```

# API in lua

example code to run with `lua.exe`

```lua
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

```

# how to play with librime on Windows

- copy `midi.dll` and `play.lua` to `<path-to-user-data-dir>\lua\`
- patch your schema, add  `- lua_processor@*play` to `engine/processors`.
- deploy and enjoy.

# how to generate notes data file(lua) from a midi file

install dependences
```bat
pip install argparser
pip install mido
```

use python script to get data file

```bat
python.exe .\get_notes.py -h
usage:
get track notes data to a lua file
--input input_file
--output output_file
--track track_index
       [-h] [--input INPUT] [--output OUTPUT] [--track TRACK]

optional arguments:
  -h, --help       show this help message and exit
  --input INPUT    input midi file
  --output OUTPUT  output data file
  --track TRACK    track index
```

# Credits
[lua](https://github.com/lua/lua.git) (MIT License)