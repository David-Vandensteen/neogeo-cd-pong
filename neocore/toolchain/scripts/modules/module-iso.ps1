Import-Module "$($Config.project.neocorePath)\toolchain\scripts\modules\module-install-component.ps1"
Import-Module "$($Config.project.neocorePath)\toolchain\scripts\modules\module-mp3towav.ps1"

#Import-Module "$($buildConfig.pathToolchain)\scripts\modules\module-install-component.ps1"
#Import-Module "$($buildConfig.pathToolchain)\scripts\modules\module-mp3towav.ps1"

function Write-ISO {
  param (
    [Parameter(Mandatory=$true)][String] $PRGFile,
    [Parameter(Mandatory=$true)][String] $OutputFile,
    [Parameter(Mandatory=$true)][String] $SpriteFile,
    [Parameter(Mandatory=$true)][String] $PathISOBuildFolder,
    [Parameter(Mandatory=$true)][String] $PathCDTemplate
  )

  Logger-Step -Message "compiling ISO"

  & mkisofs.exe -o $OutputFile -pad $PathISOBuildFolder

  if ((Test-Path -Path $OutputFile) -eq $true) {
    Logger-Success -Message "builded ISO is available to $OutputFile"
    Write-Host ""
  } else { Logger-Error -Message "$OutputFile was not generated" }
}

function Write-Cache {
  param (
    [Parameter(Mandatory=$true)][String] $PathCDTemplate,
    [Parameter(Mandatory=$true)][String] $PRGFile,
    [Parameter(Mandatory=$true)][String] $SpriteFile,
    [Parameter(Mandatory=$true)][String] $PathISOBuildFolder
  )

  if ((Test-Path -Path $PathCDTemplate) -eq $false) {
    Install-Component -URL "$($buildConfig.baseURL)/neobuild-cd_template.zip" -PathDownload $buildConfig.pathSpool -PathInstall $buildConfig.pathNeocore
  }

  if (Test-Path -Path $PathISOBuildFolder) { Remove-Item $PathISOBuildFolder -Recurse -Force }
  if (-Not(Test-Path -Path $PathISOBuildFolder)) { mkdir -Path $PathISOBuildFolder | Out-Null }
  if (-Not(Test-Path -Path $PathCDTemplate)) { Logger-Error -Message "$PathCDTemplate not found" }
  if (-Not(Test-Path -Path $PRGFile)) { Logger-Error -Message "$PRGFile not found" }
  if (-Not(Test-Path -Path $SpriteFile)) { Logger-Error -Message "$SpriteFile not found" }

  Copy-Item -Path "$PathCDTemplate\*" -Destination $PathISOBuildFolder -Recurse -Force -ErrorAction Stop
  Copy-Item -Path $PRGFile -Destination "$PathISOBuildFolder\DEMO.PRG" -Force -ErrorAction Stop
  Copy-Item -Path $SpriteFile -Destination "$PathISOBuildFolder\DEMO.SPR" -Force -ErrorAction Stop
}

function Write-SFX {
  param(
    [Parameter(Mandatory=$true)][String] $PathISOBuildFolder,
    [String] $PCMFile,
    [String] $Z80File
  )

  Logger-Step -Message "soundFX"

  if ($PCMFile) { Logger-Info -Message $PCMFile }
  if ($Z80File) { Logger-Info -Message $Z80File }

  Logger-Info -Message "Destination folder $PathISOBuildFolder"

  if ($PCMFile) { Copy-Item -Path $PCMFile -Destination "$PathISOBuildFolder\DEMO.PCM" -Force -ErrorAction Stop }
  if ($Z80File) { Copy-Item -Path $Z80File -Destination "$PathISOBuildFolder\DEMO.Z80" -Force -ErrorAction Stop }
}

function Write-CUE {
  param (
    [Parameter(Mandatory=$true)][String] $Rule,
    [Parameter(Mandatory=$true)][String] $OutputFile,
    [Parameter(Mandatory=$true)][String] $ISOName,
    [System.Xml.XmlElement] $Config
  )
  function Get-CUETrack {
    param (
      [Parameter(Mandatory=$true)][String] $Rule,
      [Parameter(Mandatory=$true)][String] $File,
      [Parameter(Mandatory=$true)][int] $Index,
      [Parameter(Mandatory=$true)][String] $Pregap
    )

    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($File)
    $ext = [System.IO.Path]::GetExtension($File)
    $path = [System.IO.Path]::GetDirectoryName($File)

    Write-Host $baseName
    Write-Host $ext
    Write-Host $path

    if ($ext -eq ".mp3" -and $Rule -like "*mame") {
      if ((Test-Path -Path "$($buildConfig.pathNeocore)\bin\mpg123-1.31.3-static-x86-64") -eq $false) {
        Install-Component -URL "$($buildConfig.baseURL)/mpg123-1.31.3-static-x86-64.zip" -PathDownload $buildConfig.pathSpool -PathInstall "$($buildConfig.pathNeocore)\bin"
      }
      # Write-WAV `
      #   -mpg123 "$($buildConfig.pathNeocore)\bin\mpg123-1.31.3-static-x86-64\mpg123.exe" `
      #   -WAVFile "$($buildConfig.pathBuild)\$path\$baseName.wav" `
      #   -MP3File "$($buildConfig.pathBuild)\$path\$baseName.mp3"
      Write-WAV `
        -mpg123 "$($buildConfig.pathNeocore)\bin\mpg123-1.31.3-static-x86-64\mpg123.exe" `
        -WAVFile "$($buildConfig.pathBuild)\$path\$baseName.wav" `
        -MP3File "$path\$baseName.mp3"
      $File = "$path\$baseName.wav"
    }

    return (
      'FILE "{0}" WAVE
  TRACK {1:d2} AUDIO
    PREGAP {2}
    INDEX 01 00:00:00') -f $File, $Index, $Pregap
  }

  ('FILE "{0}" BINARY ' -f $ISOName) | Out-File -Encoding utf8 -FilePath $OutputFile -Force
  "  TRACK 01 MODE1/2048 " | Out-File -Encoding utf8 -FilePath $OutputFile -Append -Force
  "    INDEX 01 00:00:00 " | Out-File -Encoding utf8 -FilePath $OutputFile -Append -Force

  if ($Config) {
    $tracks = $Config.tracks.track
    $tracks | ForEach-Object {
      Get-CUETrack -Rule $Rule -File $_.file -Index $_.id -Pregap $_.pregap | Out-File -Encoding ascii -FilePath $OutputFile -Append -Force
    }
  }
  (Get-Content -Path $OutputFile -Raw).Replace("`r`n","`n") | Set-Content -Path $OutputFile -Force -NoNewline

  if ((Test-Path -Path $OutputFile) -eq $true) {
    Logger-Success -Message "builded CUE is available to $OutputFile"
    Write-Host ""
  } else { Logger-Error -Message "error - $OutputFile was not generated" }
}


