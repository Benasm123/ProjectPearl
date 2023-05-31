@ECHO OFF
forfiles /p Source /m *.vert /c "cmd /c ECHO Compiling @file&glslc.exe @file -o ../Compiled/@file.spv"
forfiles /p Source /m *.frag /c "cmd /c ECHO Compiling @file&glslc.exe @file -o ../Compiled/@file.spv"
forfiles /p Source /m *.tess /c "cmd /c ECHO Compiling @file&glslc.exe @file -o ../Compiled/@file.spv"
forfiles /p Source /m *.geom /c "cmd /c ECHO Compiling @file&glslc.exe @file -o ../Compiled/@file.spv"
forfiles /p Source /m *.comp /c "cmd /c ECHO Compiling @file&glslc.exe @file -o ../Compiled/@file.spv"
PAUSE
