@ECHO OFF
@SET OPENOCD_PATH=C:\devtools\xpack-openocd-0.12.0-7\bin
@SET path=%path%;%OPENOCD_PATH%

openocd ^
-f interface/altera-usb-blaster2.cfg -f target/altera_fpgasoc_de_dual.cfg ^
-c "init; reset init; c5_spl bsp/u-boot-spl-nocache; sleep 200; halt; arm core_state arm; load_image ../app2/Debug/app2.elf; targets fpgasoc.cpu.1; resume 0; targets fpgasoc.cpu.0; load_image Debug/app1.elf; resume 0x4000000"
pause