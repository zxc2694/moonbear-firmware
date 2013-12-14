file QuadrotorFlightControl.elf
target remote :3333
b  QCopterFC_it.c:324
b  QCopterFC_it.c:326
b  QCopterFC.c:90
monitor reset halt
c
