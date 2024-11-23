# Raspberry Pi Pico MPU6050 Controller
## Since ZB2025

To compile, clone the repository
`git clone https://github.com/ZipsBaja/ZipsPicoAccelerometer`
And create a build directory, run CMake, and compile.
```
mkdir build
cd build
cmake ..
make
```

### Compile Options
The top of the `main.cpp` file has `#define`s which turn on or off features of the program.
`#define USING_PRINT` enables the output console and writes logs to it. This should be disabled (set to 0) when the final product is ready.
`#define USING_MULTIPLEXING` enables the use of multiple accelerometers.
`#define USING_ENCRYPTION` enables birary writes to the SD card instead of formatted ones. This is faster and lowers the write bitrate for smaller files, but requires to be decoded with an external program. *THIS IS CURRENTLY FAILING!*.
`#define USING_GYRO` enables the use of the MPU6050's gyroscope. Currently not needed, but it is there if rotational data is needed.
All these options can be added to the CMake file instead if you prefer (the source file would redefine them however. This feature may be added soon and set to the default).
