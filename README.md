# Game Mods

This is a monorepo with my game mods.

## Building from source

### Requirements

- [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/?q=build+tools#build-tools-for-visual-studio-2022) (or later): `winget install Microsoft.VisualStudio.2022.BuildTools`
  - MSBuild Tools
  - Desktop development with C++
  - MSVC (latest)
  - .Net SDK
  - .Net 6.0 Runtime
- [CMake >= 3.24](https://cmake.org/download/): This is probably included with **VS Desktop Dev w/ C++**, but you can also get it from winget: `winget install Kitware.CMake`
- [Ninja](https://ninja-build.org/): You can pip install it, but I couldn't build without using the VS dev shell, so might as well use the one that comes with **VS Desktop Dev w/ C++**...
- [PowerShell](https://docs.microsoft.com/en-us/powershell/scripting/install/installing-powershell-on-windows?view=powershell-7.2): `winget install --id Microsoft.Powershell`

### The make script

There's a _make.ps1_ script that you should find useful. (It's not a Makefile because winget has an old AF version of GNU Make).

I have this alias in my profile:

```pwsh
Set-Alias make ./make.ps1
```

This lets me use this script like it's `make`. But you can just run the script like `./make.ps1`

> See `make help` for commands that are available.

### Building

You can probably open this in VS and it'll maybe do the right thing? But use the cli.

Before you can build, you have to enter a VS dev shell to get build tools in you path and whatever strange variables set.
VS has some helper scripts not in your path to get you there.
You should probably google how other people do it. I just have this in my profile:

```pwsh
function dev() {
    $MSVS = "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools"
    Import-Module $MSVS\Common7\Tools\Microsoft.VisualStudio.DevShell.dll
    Enter-VsDevShell -SkipAutomaticLocation -SetDefaultWindowTitle  -DevCmdArguments '-arch=x64' -InstallPath $MSVS
}
```

Typing `dev` gets me there. Now you can:

1. `make deps`: Restores NuGet packages and generates the Ninja build project. Do this one time.
2. `make build`: Build all the projects.
3. `make clean`: Clean up build output... You'll have to `make deps` again, though. Don't do this if you want incremental builds

#### Environment variables

I added a `make env` that lists the following environment variables with their current value or the default.
You probably don't want to set these. They are really just for the CI.
The best DX is provided by defaults. You're on your own if you set these:

- Set `$env:BuildConfiguration = "Release"` if you want to build release binaries.
