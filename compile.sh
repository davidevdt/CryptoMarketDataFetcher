echo -e "\nCompiling the executables...\n"
mkdir build 
cd build 
cmake .. 
make 

cd .. 
rm -rf build 
echo -e "\nAll done. To run, use ./bin/programName\n"
