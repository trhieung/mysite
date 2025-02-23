# Compiler settings for MSVC
ifeq ($(OS),Windows_NT)
    CC=cl
    CFLAGS=/nologo /W3 \
           /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\ucrt" \
           /I "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\include" \
           /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared" \
           /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um" \
           /D "NOMINMAX"  # Optional: Disable min and max macros
    LDFLAGS=/link \
            /LIBPATH:"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\lib\x64" \
            /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\ucrt\x64" \
            /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x64"
    
    RM=del /Q
    RMDIR=rmdir /S /Q
else
    $(error "Not supported yet!")
endif

# Ensure FILE is provided only for exe/libs targets
ifeq ($(filter exe libs,$(MAKECMDGOALS)),)
    ifndef FILE
        $(if $(filter clean,$(MAKECMDGOALS)),,$(error "Usage: make [exe|libs] FILE=<filename>"))
    endif
endif

# Source file and output paths
SRC=$(FILE).cpp
OUTPUT_EXE=exe/$(FILE).exe
OUTPUT_LIB=libs/$(FILE).dll

# Create necessary directories
exe_dir:
	@if not exist exe mkdir exe

libs_dir:
	@if not exist libs mkdir libs

# Build executable
exe: exe_dir
	$(CC) $(CFLAGS) $(SRC) /Fe$(OUTPUT_EXE) $(LDFLAGS)

# Build DLL
libs: libs_dir
	$(CC) $(CFLAGS) /LD $(SRC) /Fe$(OUTPUT_LIB) $(LDFLAGS)

# Clean rule
clean:
	@if exist exe $(RMDIR) exe
	@if exist libs $(RMDIR) libs
