[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/aaylafetzer/CppVideoTool)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/aaylafetzer/CppVideoTool)

# CppVideoTool

This is an extremely broken C++ rewrite of [PyVideoTool](https://github.com/aaylafetzer/pyvideotool). However, only the CMake configuration is broken. If you can fix that and my header imports, the rest of the code should still work.

## Usage
This program, although a spiritual successor of PyVideoTool, has a very different style of usage. Instead of various arguments passed as manipulations to a single input file, multiple files can be specified and will be handled according to a string passed to the ``--filters`` argument.

Currently supported filters are:
- ``fps=x`` sets the framerate of the output file to ``x``
- ``rescale=1280x720``: Sets the size of the output to ``1280x720`` (or any other integer input)
- ``slice=0,0:1920,1080``: Selects the rectangle between two, comma-separated pixel locations as the output.

### Example
To take two input files, concatenate them, select a 640x480 rectangle from the top left, and write them to a 1280x720 output file:
``./cppvideotool --filters "rescale=1280x720;slice=0,0:640,480" output.mp4 input.mp4 input2.mp4``
