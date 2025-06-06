Build Instructions
------------------

(1) make sure that all the required dependencies are available

    - Qt version 6.X ( building under 5.X should also be possible)
    - qwt
    - boost
    - spdlog
    - sqlite

(2) create a build directory (e.g. in the top level source dir)

    mkdir build
    cd build
  
(3)  create the Makefiles usin cmake (Note: use -DCMAKE_BUILD_TYPE=Debug for debugging version )
     
    cmake -DCMAKE_BUILD_TYPE=Release ..  
    
(4) run make

    make

