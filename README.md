# Linear Program Solver

[![npm version](https://badge.fury.io/js/linear-program-solver.svg)](https://badge.fury.io/js/linear-program-solver)
[![Build Status](https://travis-ci.org/kchinzei/linear-program-solver.svg?branch=master)](https://travis-ci.org/kchinzei/linear-program-solver)
[![Coverage Status](https://coveralls.io/repos/github/kchinzei/linear-program-solver/badge.svg?branch=master)](https://coveralls.io/github/kchinzei/linear-program-solver?branch=master)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

JS/TS wrapper for [simplex](https://github.com/jeronimonunes/simplex) linear programming solver by jeronimonunes

## Requirements

- Linux or macOS (may work also on Windows by manual installation, see [Trouble shooting](#trouble-shooting))
- C++ compiler with C++-17 or newer support.
- Node 10.20.0 or newer except 11. (To be exact, N-API version 6 or newer)
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

Sync version example, as in [linear-program-parser](https://www.npmjs.com/package/linear-program-parser)

```TypeScript
import { parse } from 'linear-program-parser';
import { simplex, findSolution, simplexIsOK  } from 'linear-program-solver';
import { SimplexSolution } from 'linear-program-solver';

const linearProgram = parse(`max(-3a -4b +5c -5d)
    st:
        +1a +1b +0c +0d <= +5;
        -1a +0b -5c +5d <= -10;
        +2a +1b +1c -1d <= +10;
        -2a -1b -1c +1d <= -10;
        a >= 0;
        b >= 0;
        c >= 0;
        d >= 0;
`);

if (simplexIsOK()) {
  const fpi = linearProgram.toFPI();
  const val: SimplexSolution = simplex(fpi.toMatrix());
  const a: number = findSolution('a', val.solution, val.vars);
  ...
}
```

Async version(1)

```TypeScript
import { parse } from 'linear-program-parser';
import { simplexAsync, findSolution, simplexIsOK  } from 'linear-program-solver';
import { SimplexSolution } from 'linear-program-solver';

const linearProgram = parse(`max(-3/10*a -4/10*b +5/10*c -5/10*d)
    st:
        +1a +1b <= +5;
        -1a +0b -5c +5d <= -10;
        a >= 0;
        b >= 0;
        c >= 0;
        d >= 0;
`);

const fpi = linearProgram.toFPI();
try {
  const val: SimplexSolution = await simplexAsync(fpi.toMatrix());
  const a: number = findSolution('a', val.solution, val.vars);
  ...
```

Async version(2) all-in-one

```TypeScript
import { solveAsync, SimplexSolution } from 'linear-program-solver';

const problem = `max(-3/10*a -4/10*b +5/10*c -5/10*d)
    st:
        +1a +1b <= +5;
        -1a +0b -5c +5d <= -10;
        +2a +1b +1c -1d <= +10;
        -2a -1b -1c +1d <= -10;
        a >= 0;
        b >= 0;
        c >= -3;
`;

try {
  const val: SimplexSolution = await solveAsync(problem);
} catch (e) {
  console.log(e);
}
...
```

### Functions

#### `function simplex(arg: SimplexTableau<Fraction>): SimplexSolution`

#### `async function simplexAsync(arg: SimplexTableau<Fraction>): Promise<SimplexSolution>`

Solves standard form matrix and vectors provided by Fpi.
Input arguments are those of the output of Fpi.toMatrix().

_Result_ is either of 'otima', 'ilimitada', 'inviavel' (in Portuguese), which mean

- _great, a feasible solution found in limited iterations_,
- _okay, a solution found but iteration didn't converge_,
- _sorry, it's infeasible to solve_.

When build condition did not meet, it always returns 'inviavel' without solving simplex.

_Solution_ and _vars_ are in pair. _Vars_ is an array of given variables to solve with slack variables automatically introduced by the parser (_f_1_ ... ).
If a given variable is not constrained nonnegative, it is replaced by two nonnegative variables.
If your variable is _x_, it will be _xp_ and _xn_.

#### `function findSolution(variableName: string, solution: number[], vars: string[]): number`

Obtain a solution that corresponds to _variableName_. _Solution_ and _vars_ are those given by simplex().
If variable is replaced to positive and negative parts (slack variables), e.g., _xp_ and _xn_ such that _x = xp - xn_,
`findSolution()` will find both parts and returns the final solution (in above example, _x_).
If _variableName_ is not in _vars_, it returns NaN.

#### `function simplexIsOK(): boolean`

Returns true when build condition meets. Else, return false;

#### `async function solveAsync(problem: string): Promise<SimplexSolution>`

A service function that does everything from parsing the _problem_ to solve.

### Types

#### `type SimplexTableau<T>`

Generic type for argument of simplex(). Currently `T` is `Fraction` for `simplex()` and `simplexAsync()`.

#### `type SimplexSolution` (To be obsolete)

Type of return object of simplex().

```TypeScript
extern type SimplexSolution {
  result: ( 'otima' | 'ilimitada' | 'inviavel' );
  solution: number[];
  vars: string[];
}
```

Note: This type will be obsolete, by replacing function by async ones.

## Tips to make LP program string

[Linear-program-parser](https://www.npmjs.com/package/linear-program-parser) accepts the problem by a string. Here are some tips to make the problem string.

- Fractional numbers (e.g., 123/100) are acceptable. You can convert floating numbers (e.g., 1.23) to fractional numbers using `Fraction.toFraction()` in [fraction.js](https://www.npmjs.com/package/fraction.js).
- When you do so, don't forget '`*`' between number and variable: e.g., '123/100\*_a_'.
- You can use both '_max_' or '_min_' for the objective function.
- Only one objective function is accepted.
  As a compromise, you can connect multiple objective functions by introducing weight factors applied to functions.
- Variable constraint can be any. You can let linear-program-parser replace a nonzero constraint (>= 3, etc) by slack variables.
- Variable names can be any string

  - Starting by alphabet (not number, signs),
  - Case sensitive,
  - Not containing '\_f\_\_'.

  However it should be avoided to use such names that `findSolution()` can confuse, e.g., ending with '_n_' or '_p_'.

## Install

```Shell
> npm i linear-program-solver
```

## Trouble shooting

#### Warning _'Compiler supporting C++-17 or newer required'_

You may see this message during installation, if your default C++ compiler is so. You can specify other compiler by setting CXX environment parameter before installation.

```Shell
> export CXX=/your/newer/c++/Compiler
> npm i linear-program-solver
```

#### Solution seems wrong.

Three possible reasons.

1. When the build conditions do not meet, it returns 'inviavel' regardless the given problem.
1. The given problem is grammatically wrong.
1. Bugs of the solver or wrapper.

During installation of this module, it compiles C++ code. Compiling this module requires N-API (node API) version 6 or greater, that comes with BigInt support. Node 10.20.0 has it. Node 11.15.0 seems not. See [the version matrix](https://nodejs.org/api/n-api.html#n_api_n_api_version_matrix).

In some cases, node-gyp you are running is old. Some Linux has a pretty old node-gyp that came with apt install. You can install the latest node-gyp by

```Shell
> npm i -g node-gyp
```

To check the grammar of the problem, you may try [Simplex-web](https://jeronimonunes.github.io/simplex-web/).

#### Failed to install to Windows

Sorry, install scripts are written for Unix (tested on Linux, macOS) and I don't have a Windows PC to test. But the main code would and should work on Windows. If you can volunteer an installation script for Windows, very welcome.

If you already have a C++ compiler, python, node-gyp and installed every necessary module, you can manually build it from command console like this way (sorry not tested),

```Shell
> cd c:\users\your\prefered\folder
> git clone https://github.com/kchinzei/linear-program-solver.git
> cd linear-program-solver
> git submodule update --init --recursive
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
