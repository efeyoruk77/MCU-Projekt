echo "Building the project and running a test!"
make project
./project --cycles 2000 --rom-size 0x8 --block-size 0x4 --rom-content test/rom2.txt test/script_test.csv
