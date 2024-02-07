:: Copyright (c) 2024 Traian Avram. All rights reserved.
:: SPDX-License-Identifier: BSD-3-Clause.

@echo off
:: Push the directory where the batch file is located, as it also represents
:: the root of the project. This is done in order to ensure the same behaviour,
:: no matter from where the script file is invoked.
pushd "%~dp0"

:: Create the 'Build' directory, if it doesn't exist.
if not exist "Build/" ( mkdir "Build" )

pushd "Build"

:: Create the configuration-specific directories, if they don't exist.
if not exist "Debug-Clang/" ( mkdir "Debug-Clang" )
if not exist "Release-Clang/" ( mkdir "Release-Clang" )

cmake -S ../ -B "Debug-Clang"   -DCMAKE_CXX_COMPILER=clang -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
cmake -S ../ -B "Release-Clang" -DCMAKE_CXX_COMPILER=clang -G "Ninja" -DCMAKE_BUILD_TYPE=Release

popd

:: Pop the directory where the batch file is located.
popd