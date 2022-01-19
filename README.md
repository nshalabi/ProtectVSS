# Protect VSS
POC to protect VSS copies by terminating `vssadmin delete shadows` command and killing the initiating parent process (reference for the dicussion https://twitter.com/cyb3rops/status/1191345532352057344)

## Driver files
* x86 driver (debug build) : ProtectVSS/Debug/ProtectVSS.sys
* x64 driver (debug build) : ProtectVSS/x64/Debug/ProtectVSS.sys

## Setup driver

Installation

`sc create ProtectVSS type= kernel binPath= {Path to .sys file}\ProtectVSS.sys`

Starting driver

`sc start ProtectVSS`

Stopping driver

`sc stop ProtectVSS`

To see the logging (KdPrint) messages in DbgView, apply the registry setting in "EnableKdPrintRegistrySetting.reg" file

You might also need to enable test sigining (it is better to use a VM for testing):

`bcdedit /set testsigning on`

## WARNING
This POC was tested with Windows 10 (1909), **USE A TESTING VIRTUAL MACHINE to experiment with the driver**

## Preview

![Alt](https://github.com/nshalabi/ProtectVSS/blob/master/Media/Demo.ProtectVSS.gif "Preview")

# License

```
Copyright 2019 Nader Shallabi. All rights reserved. 

"PROTECT VSS" CAN BE COPIED AND/OR DISTRIBUTED WITHOUT ANY EXPRESS PERMISSION OF NADER SHALLABI.

THIS SOFTWARE IS PROVIDED BY NADER SHALLABI ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NADER SHALLABI
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the authors and
should not be interpreted as representing official policies, either expressed or implied, of Nader Shallabi.
```
