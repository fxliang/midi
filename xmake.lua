target("lua54")
  set_kind("static")
  add_files("lua/*.c")
  remove_files("lua/lua.c", "lua/onelua.c")

target("midi")
  set_kind("shared")
  add_files("./src/midi.cpp")
  add_includedirs("./lua")
  add_deps("lua54")
  add_links('user32', 'winmm')
  add_defines("UNICODE", "_UNICODE")
  -- insure file name
  set_filename("midi.dll")
  -- if mingw platform
  if is_plat("mingw") then
    -- static libgcc and libstdc++
    add_shflags("-static-libgcc", "-static-libstdc++", {force=true})
  end
  -- copy midi.dll to project dir
  after_build(function(target)
    os.cp(path.join(target:targetdir(), "midi.dll"), "$(projectdir)")
  end)
