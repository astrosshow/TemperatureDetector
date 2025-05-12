Write-Host "Building project..." -ForegroundColor Green
platformio run
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Build successful!" -ForegroundColor Green
Write-Host "Putting device in DFU mode and uploading..." -ForegroundColor Yellow
Write-Host "Please make sure your Longan Nano is in DFU mode (hold BOOT0, press/release RESET, release BOOT0)" -ForegroundColor Yellow
Write-Host "Press any key to continue..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

dfu-util -d 28e9:0189 -a 0 -s 0x08000000 -D .pio/build/gd32vf103c_longan_nano/firmware.bin
if ($LASTEXITCODE -ne 0) {
    Write-Host "Upload failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Upload successful!" -ForegroundColor Green
Write-Host "Press the reset button on your Longan Nano to start the program." -ForegroundColor Yellow
Write-Host "Press any key to exit..." -ForegroundColor Green
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
