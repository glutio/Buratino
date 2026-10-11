# Builds and runs the native tests with MSVC (Build Tools found via vswhere).
$ErrorActionPreference = 'Stop'
$root = Resolve-Path "$PSScriptRoot\..\.."
$build = Join-Path $root 'build'
New-Item -ItemType Directory -Force $build | Out-Null
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw 'Microsoft C++ Build Tools are required.' }
$command = 'call "{0}\VC\Auxiliary\Build\vcvars64.bat" >nul && cd /d "{1}" && cl /nologo /std:c++14 /permissive- /EHsc /W4 /I"{2}\src" "{2}\extras\tests\BuratinoTests.cpp" /Fe:tests.exe >nul && .\tests.exe' -f $vs, $build, $root
& $env:ComSpec /d /c $command
if ($LASTEXITCODE) { throw "Tests failed (exit $LASTEXITCODE)." }
