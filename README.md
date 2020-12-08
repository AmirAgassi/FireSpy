# FireSpy
FireSpy allows for monitoring RemoteEvents, along with editing, blocking, or firing custom RemoteEvents to exploit filtering-enabled games on Roblox.

FireSpy runs on Roblox's internal Lua C functions on a custom lua_state. Simple Lua C functions are exceptionally easy to fix once Roblox patches the exploit, preserving it's longevity and basically ensuring my time isn't wasted.

# Functionality Plans
  - Manual Lua C execution on a new lua_state.
  - Complete monitoring of every fired event for every RemoteEvent, along with all arguments and passed items.
  - Ability to create and fire any RemoteEvent in the game with custom arguments.
  - Nice GUI with an explorer to manually look through objects of interest in Workspace.
  - Context level 7 with access to all possible objects such as CoreGUI.
