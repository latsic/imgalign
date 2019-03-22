#!/bin/bash

#needs python and emscripten installed and available on the command line

cd opencv_src

if [[ "${1}" == "small" ]] ; then

  export IMGALIGN_OPT="-Oz"

  #python ./platforms/js/build_js.py build_Oz --build_wasm --enable_exception --allow_memory_growth --binaryen_trap_mode_clamp
  python ./platforms/js/build_js.py build_Oz --build_wasm --binaryen_trap_mode_clamp --allow_memory_growth --Oz
 
  cd ..
  mv ./opencv_src/build_Oz/bin/opencv.js ./public/js/opencv_3_4_custom_Oz.js

elif [[ "${1}" == "both" ]] ; then

  export IMGALIGN_OPT="-O3"

  python ./platforms/js/build_js.py build_O3 --build_wasm --binaryen_trap_mode_clamp --allow_memory_growth --O3
  cd ..
  mv ./opencv_src/build_O3/bin/opencv.js ./public/js/opencv_3_4_custom_O3.js

  cd opencv_src

  export IMGALIGN_OPT="-Oz"

  python ./platforms/js/build_js.py build_Oz --build_wasm --binaryen_trap_mode_clamp --allow_memory_growth --Oz

  cd ..
  mv ./opencv_src/build_Oz/bin/opencv.js ./public/js/opencv_3_4_custom_Oz.js

else

  export IMGALIGN_OPT="-O3"

  python ./platforms/js/build_js.py build_O3 --build_wasm --binaryen_trap_mode_clamp --allow_memory_growth --O3

  cd ..
  mv ./opencv_src/build_O3/bin/opencv.js ./public/js/opencv_3_4_custom_O3.js
fi
