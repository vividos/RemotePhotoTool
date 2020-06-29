<#
	RemotePhotoTool - remote camera control software
	Copyright (c) 2008-2020 Michael Fink

.SYNOPSIS
	Writes a version number to the files version.h
#>

param (
	[Parameter(Mandatory=$true)][string]$version = "1.0.0.0"
)

$scriptPath = split-path -parent $MyInvocation.MyCommand.Definition

Write-Host "Patching build version $version..."

# split version number
$array = $version.Split(".")
$majorVersion = $array[0]
$minorVersion = $array[1]
$releaseNumber = $array[2]
$buildNumber = $array[3]
$buildYear = Get-Date -format yyyy

# modify version.h
$versionHeader = Get-Content "$scriptPath\version.h"

$versionHeader = $versionHeader -replace "MAIN_VERSION [0-9]+","MAIN_VERSION $majorVersion"
$versionHeader = $versionHeader -replace "MINOR_VERSION [0-9]+","MINOR_VERSION $minorVersion"
$versionHeader = $versionHeader -replace "SUB_VERSION [0-9]+","SUB_VERSION $releaseNumber"
$versionHeader = $versionHeader -replace "BUILD_NUMBER [0-9]+","BUILD_NUMBER $buildNumber"
$versionHeader = $versionHeader -replace "BUILD_YEAR [0-9]+","BUILD_YEAR $buildYear"

Out-File -FilePath "$scriptPath\version.h" -InputObject $versionHeader -Encoding ASCII

Write-Host "Done patching."
