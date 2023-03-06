#!/usr/bin/env bash

rm -rf build *.exe
/home/utils/cmake-3.22.1/bin/cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_EXECUTE_PROCESS_COMMAND_ECHO=STDOUT \
                                   -S. -B build -DCMAKE_BUILD_TYPE=Debug && \
echo -e "\n\e[35m==================== Compiling =====================\e[0m\n" && \
cd build && \
make VERBOSE=1 && cp *.exe .. && cd .. && rm -rf build

for exe in *.exe; do
  echo -e "\n\e[0;35m==================== Running $exe ===================="
  echo -e "\e[0;34m"
  $exe || break  # execute successfully or break
done
