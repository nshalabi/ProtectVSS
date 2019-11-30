# ProtectVSS
POC to protect VSS copies by terminating `vssadmin delete shadows` command and killing the initiating parent process

## Driver files
* x86 driver (debug build) : ProtectVSS/Debug/ProtectVSS.sys
* x64 driver (debug build) : ProtectVSS/x64/Debug/ProtectVSS.sys

## Setup driver

Installation

`sc create booster type= kernel binPath= {Path to .sys file}\ProtectVSS.sys`

Starting driver

`sc start ProtectVSS`

Stopping driver

`sc stop ProtectVSS`

To see the logging (KdPrint) messages in DbgView, apply the registry setting in "EnableKdPrintRegistrySetting.reg" file

## WARNING
This POC was tested with Windows 10 (1909), **USE A TESTING virtual machine to experiment**

## Preview

![Alt](https://github.com/nshalabi/ProtectVSS/blob/master/Media/Demo.ProtectVSS.gif "Preview")