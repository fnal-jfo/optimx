------------------
Build Instructions
------------------


(1) make sure that all the required dependencies are available

    - Qt version 6.X ( building under 5.X should also be possible)
    - qwt              [https://qwt.sourceforge.io/]
    - boost            [https://boost.org]
    - spdlog           [https://github.com/gabime/spdlog]
    - sqlite           [https://sqlite.org]
    - fmt              [https://github.com/fmtlib/fmt]
    - vsqlite++        [https://vsqlite.virtuosic-bytes.com/]

(2) create a build directory (e.g. in the top level source dir)

    mkdir build
    cd build
  
(3)  create the Makefiles using cmake (Note: use -DCMAKE_BUILD_TYPE=Debug for debugging version )
     
    cmake -DCMAKE_BUILD_TYPE=Release ..  
    
(4) run make

    make

-----
NOTES 
------
In the CMakeList.* file you will see lines like

TARGET_LINK_LIBRARIES(optimx vsqlitepp)
....

This works only if the library libvsqlitepp.a is found in a location known to cmake.
You can specify a custom location as follows

TARGET_LINK_LIBRARIES(optimx /some/path/libvsqlitepp.a)    # linux
TARGET_LINK_LIBRARIES(optimx d:\some\path\libvsqlitepp.a)  # windows

etc ... 