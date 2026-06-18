#!/bin/bash
echo "Збірка проєкту..."
mkdir -p build
cd build
cmake ..
make
echo "Збірка завершена!"