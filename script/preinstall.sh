#!/bin/sh
# The MIT License (MIT)
#
# Copyright (c) Kiyo Chinzei (kchinzei@gmail.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Constants
github_simplex="https://github.com/jeronimonunes/simplex"
github_bigint="https://github.com/jeronimonunes/bigint"

# Analyze arguments
verbose=""
if [ $# -eq 1 -a "$1" == "-v" ]; then
    verbose=$1
fi

# Test C++ compiler. We need C++-17 or newer.
iscpp17_src="./iscpp17.cc"
iscpp17_exe="./iscpp17.exe"
scriptDir="./script"
currentDir=`pwd`

echo ${CXX:=c++}

if [ "$verbose" != "" ]; then
    ${CXX} --version
fi

cd $scriptDir
${CXX} -o $iscpp17_exe -std=c++1z $iscpp17_src
if [ $? -eq 0 ]; then
    iscpp17=`$iscpp17_exe`
    rm $iscpp17_exe
    echo $iscpp17
    if [ "$iscpp17" != "C++17" ]; then
        echo "Compiler needs to support C++-17 or newer. Abort"
        exit 1
    fi
else
    echo "Compiler check failed. We continue, but it's not good sign."
fi

# TODO: Use gcc_raspbian if it's Raspi.

# Get simplex code
cd $currentDir

if ! [ -d ./simplex -a -f ./simplex/src/Base.hh ]; then
    git clone $github_simplex
    if [ $? -ne 0 ]; then
        echo "Downloading C++ code for simplex failed. Abort"
        exit 1
    fi
fi

cd simplex

if ! [ -d ./bigint -a -f ./bigint/BigInteger.hh ]; then
    git clone $github_bigint
    if [ $? -ne 0 ]; then
        echo "Downloading C++ code for bigint failed. Abort"
        exit 1
    fi
fi

# When using own preinstall script, we must take care of running node-gyp.
# cf: https://docs.npmjs.com/misc/scripts

# Find our node-gyp
node_gyp=`npm ls -g node-gyp -ps |  head -n 1`/bin/node-gyp.js

if ! [ -f "$node_gyp" ]; then
    echo "Cannot locate installed node-gyp execulable as expected"
    exit 1
fi

cd $currentDir

$node_gyp --version
$node_gyp $verbose configure
$node_gyp $verbose install

$node_gyp $verbose rebuild

exit 0
