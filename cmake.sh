#!/usr/bin/env tcsh

rm -rf build-debug-vpx
cmake -S. -Bbuild-debug-vpx -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_EXECUTE_PROCESS_COMMAND_ECHO=STDOUT
echo -n "\e[34m-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-\e[0m\n"
cd build-debug-vpx
make VERBOSE=1

echo -n "\n\e[34m----------- testing traits ------------\e[0m\n\n"
./traits

echo -n "\n\e[34m----------- testing bridge ------------\e[0m\n\n"
./bridge
