# redmagicfancontrold

A native C daemon that provides a Unix socket API for controlling and monitoring the RedMagic cooling fan.

The daemon is designed to run as part of a Magisk module on rooted Android devices. It maintains fan state, processes client requests, and interfaces with the underlying hardware.

## Status

Work in progress.

## Build
Presets:<br>
 - "linux"        - Linux Release <br>
 - "aarch64-musl" - ARM64 musl Release <br>
 - "aarch64"      - Android ARM64 Release <br>
  
`cmake --preset <preset-name>`<br>

`cmake --build --preset <preset-name>`

Or just use the workflow builds
