$tempPath = Join-Path -Path $(Get-Location) -ChildPath "tmp"

New-Item -ItemType Directory -Path $tempPath -Force | Out-Null

$devZipPath = Join-Path -Path $tempPath -ChildPath "ffmpeg.dev.zip"
$binZipPath = Join-Path -Path $tempPath -ChildPath "ffmpeg.bin.zip"

(new-object System.Net.WebClient).DownloadFile('https://ffmpeg.zeranoe.com/builds/win64/dev/ffmpeg-20190716-806ac7d-win64-dev.zip', $devZipPath);
Expand-Archive $devZipPath -DestinationPath $tempPath -Force

(new-object System.Net.WebClient).DownloadFile('https://ffmpeg.zeranoe.com/builds/win64/shared/ffmpeg-20190716-806ac7d-win64-shared.zip', $binZipPath);
Expand-Archive $binZipPath -DestinationPath $tempPath -Force

Remove-Item -Path include -Force -Recurse -ErrorAction Ignore
Remove-Item -Path lib -Force -Recurse -ErrorAction Ignore
Remove-Item -Path bin -Force -Recurse -ErrorAction Ignore

Get-ChildItem -Path "tmp/ffmpeg-*-win64-dev/lib" | Move-Item -Destination $(Get-Location) -Force
Get-ChildItem -Path "tmp/ffmpeg-*-win64-dev/include" | Move-Item -Destination $(Get-Location) -Force
Get-ChildItem -Path "tmp/ffmpeg-*-win64-shared/bin" | Move-Item -Destination $(Get-Location) -Force

Remove-Item -Path $tempPath -Force -Recurse -ErrorAction Ignore





