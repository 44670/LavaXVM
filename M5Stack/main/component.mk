#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the 
# src/ directory, compile them and link them into lib(subdirectory_name).a 
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#


COMPONENT_OBJS :=  FTPCallbacks.o FTPServer.o platform.o main.o ../../filelist.o ../../lava.o ../../message.o ../../py2gb.o ../../screen.o
COMPONENT_SRCDIRS := . ../..
COMPONENT_ADD_INCLUDEDIRS:= . ../..

CFLAGS = -O3  -mlongcalls -Wno-unused-variable -Wno-format -Wno-pointer-sign

CPPFLAGS += -Wno-format