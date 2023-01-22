
Сделать тулчейн из файла toolchain-mips-rtems4.10.cmake.example 
Скопировать, удалить ".example"
Указать путь к компилятору
SET(CMAKE_FIND_ROOT_PATH  c:/Compilers/mingw+msys+mips-rtems4.10/opt/mips-rtems4.10/bin)


Запустить bat

Запустить Eclipse
Открыть созданный файл проекта

Правой кнопкой по открытому проекту, меню C/C++ Include Paths and Symbols, Add Preprocessor Symbol.
Добавить "VM014" (без кавычек)



Настроить debug configuration, GDB Hardware Debugging.
Выбрать файл бинарников C/C++ application: 
C:\work\workNew\vme\vmeTechProject\bin\vmeTech

? GDB command ??????? ??? ????? GDB:
C:\Compilers\mingw+msys+mips-rtems4.10\opt\mips-rtems4.10\bin\mips-rtems4.10-gdb.exe

Выбрать отладчик , JTAG Device OpenOCD(via socket)
ip   172.20.54.221
port 4444






