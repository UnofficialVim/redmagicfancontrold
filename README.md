# redmagicfancontrold

A native C daemon that provides a Unix socket API for controlling and monitoring the RedMagic cooling fan.

The daemon is designed to run on a rooted Android device. It maintains fan state, processes client requests, and interfaces with the underlying hardware.

## Status

- [x] Fan control based on CPU temperature
- [x] Custom config (fan steps, refresh rate, per-profile settings)
- [x] Unix socket IPC transport (accept/receive/send over a domain socket)
- [x] Structured logging (console + file sinks, per-level filtering)
- [x] Cross-compiled CI builds (Linux, aarch64-musl static, Android aarch64)

More functionality still in progress

## Build
Presets:<br>
 - "linux"        - Linux Release <br>
 - "aarch64-musl" - ARM64 musl Release <br>
 - "aarch64"      - Android ARM64 Release <br>
  
`cmake --preset <preset-name>`<br>

`cmake --build --preset <preset-name>`

Or just use the workflow builds
