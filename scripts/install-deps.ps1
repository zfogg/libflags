$ErrorActionPreference = 'Stop'

foreach ($tool in @('cmake', 'ninja', 'cl')) {
    if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
        throw "Required Windows build tool not found: $tool"
    }
}

Write-Host 'MSVC, CMake, and Ninja are available.'
