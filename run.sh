#!/bin/bash

# Если build директория не существует, создаём её
if [ ! -d build ]; then
    mkdir build
fi

cd build

# Генерация сборочных файлов с использованием Ninja
cmake -G Ninja .. || exit 1

# Собираем проект с помощью Ninja
ninja || exit 1

# Запускаем программу
./yal
