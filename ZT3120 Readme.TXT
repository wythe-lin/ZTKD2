
Q: How to generate ZT3120.BIN?

1. Open zt3120.h
2. Find the solution list between "Start of ZT3120 solutions" and "End of ZT3120 solutions".  
3. Select one of the solution that you are going to generate LIB file.
4. Save zt3120.h
5. Open UV4 project ZT3120.uvproj
6. Compile, link, and generate ZT3120.HEX and ZT3120.BIN

Notes that ZT3120.HEX and ZT3120.LIB may be different for different solutions.


Q: How to generate ZT3120.LIB?

1. Open zt3120.h
2. Find the solution list between "Start of ZT3120 solutions" and "End of ZT3120 solutions".  
3. Select one of the solution that you are going to generate LIB file.
4. Save zt3120.h
5. Open UV4 project ZT3120LIB.uvproj
6. Compile, link, and generate ZT3120.LIB

Notes that ZT3120.LIB may be different for different solutions.


Q: What are the files in ZT3120 released package?

1. The following include files in folder ZT3120:
    zt3120.h, system.h, sensor.h, sensor0.h, sensor1.h,
    d2.h, gendef.h
2. Common solution source file in folder ZT3120:
    zt3120_sensor.c
3. One of the solution source file in folder ZT3120:
    zt3120_ov7725.c, zt3120_nt99140.c, zt3120_nt99141.c
4. UV4 project related files in folder ZT3120:
    zt3120.uvproj, zt3120.uvopt, zt3120.lib
5. UV4 project related files in current folder:
    hex2bin.exe

Be sure to check the file consistency for the include files before releasing the package.
