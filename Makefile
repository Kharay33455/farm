g++ setup.c shooter.c auxfunc.c send.c -o saver.exe -I"C:/libjpeg-turbo-gcc/include" -L"C:/libjpeg-turbo-gcc/lib" -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x86" -ljpeg -lgdi32 -lwinhttp -mconsole
cl setup.c /I"C:/libjpeg-turbo-gcc/include" /link /LIBPATH:"C:/libjpeg-turbo-gcc/lib" libjpeg.lib
