Build Instructions
------------------

(1) make sure that all the required dependencies are available

    - Qt version 6.X ( buildding under 5.X should also be possible)
    - qwt
    - boost
    - spdlog
    - sqlite

(2) create a build directory (e.g. in the top level source dir)
    mkdir build

  
(3)  create the Makefiles

    cmake -DCMAKE_BUILD_TYPE=RElease ..
    
(4) run make

    make

