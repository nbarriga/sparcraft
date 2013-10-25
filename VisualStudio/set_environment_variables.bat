@echo off
echo Set SparCraft Windows Environment Variables
echo Please edit this file before running for the first time

pause


setx BOOST_DIR W:\Libraries\boost_1_53_0
setx EXTERNAL_LIB_DIR C:\Dropbox\RESEARCH_\SparCraft\external_binaries\lib
setx SDL_DIR W:\Libraries\SDL-1.2.15
setx SDL_IMAGE_DIR W:\Libraries\SDL_image-1.2.12
setx SDL_GFX_DIR W:\Libraries\SDL_gfx-2.0.24
setx BWAPI_DIR C:\Dropbox\RESEARCH_\SparCraft\bwapidata

pause