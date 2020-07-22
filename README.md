# Linear Program Solver

[![npm version](https://badge.fury.io/js/linear-program-solver.svg)](https://badge.fury.io/js/linear-program-solver)
[![Build Status](https://travis-ci.org/kchinzei/linear-program-solver.svg?branch=master)](https://travis-ci.org/kchinzei/linear-program-solver)
[![Coverage Status](https://coveralls.io/repos/github/kchinzei/linear-program-solver/badge.svg?branch=master)](https://coveralls.io/github/kchinzei/linear-program-solver?branch=master)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

JS/TS wrapper for [simplex](https://github.com/jeronimonunes/simplex) linear programming solver by jeronimonunes

## Requirements

- Linux or macOS (Windows also by manual installation, see [Trouble shooting](#trouble-shooting))
- C++ compiler with C++-17 or newer support.
- Node 10 or newer except 11. (To be exact, N-API version 6 or newer)
- [linear-program-parser](https://www.npmjs.com/package/linear-program-parser) 1.0.11 or newer (npm module).
- [jeronimonunes/simplex](https://github.com/jeronimonunes/simplex) (C++ code).
- [jeronimonunes/bigint](https://github.com/jeronimonunes/bigint) (C++ code).

This module will be built even these conditions do not meet. When so, **simplex() always returns 'inviavel' (infeasible) without actually solving the problem.** To check if simplex() works, use simplexIsOK().

## Description

Linear-program-solver is a JavaScript / TypeScript wrapper to [simplex C++ engine by jeronimonunes](https://github.com/jeronimonunes/simplex).
It's intended to be used together with [linear-program-parser](https://www.npmjs.com/package/linear-program-parser) written in TypeScript.
There are several linear program solvers found in npm.
[Linear-program-parser](https://www.npmjs.com/package/linear-program-parser) is relatively new, written in TypeScript and personally easy to use among them.
Linear-program-solver can accept the output of [linear-program-parser](https://www.npmjs.com/package/linear-program-parser) directly.

## Code snippets

```TypeScript
import { parse, Fpi } from 'linear-program-parser';
import { simplex, findSolution, simplexIsOK  } from 'linear-program-solver';
import { SimplexTableau, SimplexSolution } from 'linear-program-solver';

const linearProgram = parse('max(-3a -4b +5c -5d)
    st:
        +1a +1b +0c +0d <= +5;
        -1a +0b -5c +5d <= -10;
        +2a +1b +1c -1d <= +10;
        -2a -1b -1c +1d <= -10;
        a >= 0;
        b >= 0;
        c >= 0;
        d >= 0;
');

const fpi = linearProgram.toFPI();
const val: SimplexSolution = simplex(fpi.toMatrix());
const a: number = findSolution('a', val.solution, val.vars);
...
```

##### type SimplexTableau\<T\>

Generic type for argument of simplex(). Currently `T` is `Fraction` for Simplex().

##### type SimplexSolution

Type of return object of simplex().

```TypeScript
extern type SimplexSolution {
  result: ( 'otima' | 'ilimitada' | 'inviavel' );
  solution: number[];
  vars: string[];
}
```

##### function simplex(arg: SimplexTableau<Fraction>): SimplexSolution

Solves standard form matrix and vectors provided by Fpi.
Input arguments are those of the output of Fpi.toMatrix().

_Result_ is either of 'otima', 'ilimitada', 'inviavel' (in Portuguese), which mean

- _great, a feasible solution found in limited iterations_,
- _okey, a solution found but iteration didn't converge_,
- _sorry, it's infeasible to solve_.

When build condition did not meet, it always returns 'inviavel' without solving simplex.

_Solution_ and _vars_ are in pair. _Vars_ is an array of given variables to solve with slack variables automatically introduced by the parser (_f_1_ ... ).
If a given variable is not constrained nonnegative, it is replaced by two nonnegative variables.
If your variable is _x_, it will be _xp_ and _xn_.

##### function findSolution(variableName: string, solution: number[], vars: string[]): number

Obtain a solution that corresponds to _variableName_. _Solution_ and _vars_ are those given by simplex().
If variable is replaced to positive and negative parts (e.g., _xp_ and _xn_ such that _x = xp - xn_),
findSolution() will find both parts and returns the final solution (in above example, _x_).
If _variableName_ is not in _vars_, it returns NaN.

##### function simplexIsOK(): boolean

Returns true when build condition meets. Else, return false;

## Install

```Shell
> npm i linear-program-solver
```

### Trouble shooting

#### Warning _'Compiler supporting C++-17 or newer required'_

You may see this message during installation, if your default C++ compiler is so. You can specify other compiler by setting CXX environment parameter.

```Shell
> export CXX=/your/new/c++/Compiler
```

#### Solution seems wrong.

Three possible reasons.

1. When the build conditions do not meet, it returns 'inviavel' regardless the given problem.
1. The given problem is grammatically wrong.
1. Bugs of the solver or wrapper.

During installation of this module, it compiles C++ code on your computer. Compiling this module requires N-API (node API) version 6 or greater, that comes with BigInt support. Node 10.20.0 has it. Node 11.15.0 seems not. See [the version matrix](https://nodejs.org/api/n-api.html#n_api_n_api_version_matrix).

In some cases, node-gyp you are running is old. Some Linux has a pretty old node-gyp that came with apt install. You can install the latest node-gyp by

```Shell
> npm i -g node-gyp
```

To check the grammar of the problem, you may try [Simplex-web](https://jeronimonunes.github.io/simplex-web/).

#### Failed to install to Windows

Sorry, install scripts are written for Unix and I don' have a Windows PC. But the main code would and should work on Windows. If you can volunteer a script for Windows, very welcome.

If you already have a C++ compiler, python, node-gyp and installed every necessary module, you can manually build it from command console like this way (sorry not tested),

```Shell
> cd c:\users\your\prefered\folder
> git clone https://github.com/kchinzei/linear-program-solver.git
> cd linear-program-solver
> node-gyp rebuild
> npm run build
> cd c:\users\your\root
> npm i c:\users\your\prefered\folder\linear-program-solver
```

## Known issues

The tableau data generated by [linear-program-parser](https://www.npmjs.com/package/linear-program-parser) is represented by `Fraction` (but not that of fraction.js).
To translate javascript `Fraction` to C++ class, it uses `BigInt` to `long` coercion, that can be a lossy conversion.

## Credits

- [linear-program-parser](https://www.npmjs.com/package/linear-program-parser) and
  [simplex](https://github.com/jeronimonunes/simplex) by jeronimonunes.
- [bigint](https://github.com/jeronimonunes/bigint) originally by [Matt McCutchen](https://mattmccutchen.net/bigint/) with modification by jeronimonunes.

## License

The MIT License (MIT)
Copyright (c) K. Chinzei (kchinzei@gmail.com)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

### simplex and modified bigint by jeronimonunes

https://github.com/jeronimonunes/simplex

MIT License

Copyright (c) 2020 Jerônimo Nunes Rocha

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

### bigint originally by Matt McCutchen

https://mattmccutchen.net/bigint/

I, Matt McCutchen, the sole author of the original Big Integer Library, waive my copyright to it, placing it in the public domain. The library comes with absolutely no warranty.
