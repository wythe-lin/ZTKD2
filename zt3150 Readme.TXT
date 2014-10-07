
Q: How to generate ZT3150.BIN?

1. Open zt3150.h
2. Find the solution list between "Start of ZT3150 solutions" and "End of ZT3150 solutions".  
3. Select one of the solution that you are going to generate LIB file.
4. Save zt3150.h
5. Open UV4 project ZT3150.uvproj
6. Compile, link, and generate ZT3150.HEX and ZT3150.BIN

Notes that ZT3150.HEX and ZT3150.LIB may be different for different solutions.


Q: How to generate ZT3150.LIB?

1. Open zt3150.h
2. Find the solution list between "Start of ZT3150 solutions" and "End of ZT3150 solutions".  
3. Select one of the solution that you are going to generate LIB file.
4. Save zt3150.h
5. Open UV4 project ZT3150LIB.uvproj
6. Compile, link, and generate ZT3150.LIB

Notes that ZT3150.LIB may be different for different solutions.


Q: What are the files in ZT3150 released package?

1. The following include files in folder ZT3150:
    zt3150.h, system.h, sensor.h, sensor0.h, sensor1.h,
    d2.h, gendef.h
2. Common solution source file in folder ZT3150:
    zt3150_sensor.c
3. One of the solution source file in folder ZT3150:
    zt3150_ov7725.c, zt3150_nt99140.c, zt3150_nt99141.c
4. UV4 project related files in folder ZT3150:
    zt3150.uvproj, zt3150.uvopt, zt3150.lib
5. UV4 project related files in current folder:
    hex2bin.exe

Be sure to check the file consistency for the include files before releasing the package.
