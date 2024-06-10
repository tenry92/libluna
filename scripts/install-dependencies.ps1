# PowerShell script to install development libraries using vcpkg

# Function to check if a command exists
function Command-Exists {
  param (
    [string]$command
  )
  return Get-Command $command -ErrorAction SilentlyContinue
}

# Install Chocolatey if it's not already installed
if (-not (Command-Exists choco)) {
  Write-Host "Installing Chocolatey..."
  Set-ExecutionPolicy Bypass -Scope Process -Force;
  [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072;
  iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'));
}

# Install git if it's not already installed
if (-not (Command-Exists git)) {
  Write-Host "Installing Git..."
  choco install git -y
}

# Clone vcpkg if it's not already cloned
if (-not (Test-Path -Path "C:\vcpkg")) {
  Write-Host "Cloning vcpkg..."
  git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
}

# Bootstrap vcpkg if it's not already bootstrapped
if (-not (Test-Path -Path "C:\vcpkg\vcpkg.exe")) {
  Write-Host "Bootstrapping vcpkg..."
  & "C:\vcpkg\bootstrap-vcpkg.bat"
}

# Define the libraries to install
$libraries = @(
  "zlib",
  "libpng",
  "opus",
  "opusfile",
  "glm",
  "utfcpp",
  "sdl2"
)

# Install the libraries using vcpkg
foreach ($lib in $libraries) {
  Write-Host "Installing $lib..."
  & "C:\vcpkg\vcpkg.exe" install $lib
}

# Integrate vcpkg with MSBuild (optional, useful for Visual Studio integration)
Write-Host "Integrating vcpkg with MSBuild..."
& "C:\vcpkg\vcpkg.exe" integrate install

Write-Host "All libraries installed successfully."
