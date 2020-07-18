# Linear Program Solver

<!--
[![npm version](https://badge.fury.io/js/raspi-pca9685-pwm.svg)](https://badge.fury.io/js/raspi-pca9685-pwm)
[![Build Status](https://travis-ci.org/kchinzei/raspi-pca9685-pwm.svg?branch=fakemorph)](https://travis-ci.org/kchinzei/raspi-pca9685-pwm)
[![Coverage Status](https://coveralls.io/repos/github/kchinzei/raspi-pca9685-pwm/badge.svg?branch=fakemorph)](https://coveralls.io/github/kchinzei/raspi-pca9685-pwm?branch=fakemorph)
-->

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

JS/TS wrapper for [simplex by jeronimonunes](https://github.com/jeronimonunes/simplex)

## Requirements

- C++ compiler with C++-17 support.
- [linear-program-parser 1.0.11 or newer](https://www.npmjs.com/package/linear-program-parser).
- [jeronimonunes/simplex](https://github.com/jeronimonunes/simplex).
- [bigint](https://github.com/jeronimonunes/bigint).
- Node 11.15.0 or newer.

## Description

Linear-program-solver is a JavaScript / TypeScript wrapper to [simplex C++ engine by jeronimonunes](https://github.com/jeronimonunes/simplex).
It's intended to be used together with [linear-program-parser](https://www.npmjs.com/package/linear-program-parser) linear program parser.
There are several linear program solvers found in npm.
[Linear-program-parser](https://www.npmjs.com/package/linear-program-parser) is relatively new, written in TypeScript and personally easy to use among them.
But this is a parser to generate a standard form.
You need to provide it [simplex](https://github.com/jeronimonunes/simplex) with nicely arranging the standard form to a simplex tableau.
Linear program solver can accept the output of [linear-program-parser](https://www.npmjs.com/package/linear-program-parser) directly.

## Code snippets

```TypeScript
import { parse, Fpi } from 'linear-program-parser';
import { simplex, findSolution } from 'linear-program-solver';

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

const fpi = linearProgram.toFPI();
const { result, solution, vars } = simplex(fpi.toMatrix());
const a: number = findSolution('a', solution, vars);
...
```

##### function simplex(arg: { a: Fraction[][]; b: Fraction[]; c: Fraction[]; vars: string[] }): { result: ( 'otima' | 'ilimitada' | 'inviavel' ); solution: number[]; vars: string[] }

Solves standard form matrix and vectors provided by Fpi.
Input arguments are those of the output of Fpi.toMatrix().

_Result_ is either of 'otima', 'ilimitada', 'inviavel' (in Portuguese), which mean

- _great, a feasible solution found in limited iterations_,
- _okey, a solution found but iteration didn't converge_,
- _sorry, it's infeasible to solve_.

_Solution_ and _vars_ are in pair. _Vars_ is an array of given variables to solve and slack variables (_f_1_ ... ).
If a given variable is not constrained nonnegative, it is replaced by two nonnegative variables.
If your variable is _x_, it will be _xp_ and _xn_.

##### function findSolution(variableName: string, solution: number[], vars: string[]): number

Obtain a solution that corresponds to _variableName_. _Solution_ and _vars_ are those given by simplex().
If variable is replaced to positive and negative parts (e.g., _xp_ and _xn_ such that _x = xp - xn_),
findSolution() will find both parts and returns the final solution (in above example, _x_).
If _variableName_ is not in _vars_, it returns NaN.

## Install

```Shell
npm i linear-program-solver
```

<!--
During installation, it checks if your C++ compiler supports C++-17.
If you see an error message like

```Shell
> npm i linear-program-solver
  ...
  Build error: C++ compiler that supports c++-17 or newer required.
  ...
```

You need to manage to get one. For Raspberry pi, it temporarily downloads gcc version 9.1 binary.
-->

## Credits

- [linear-program-parser](https://www.npmjs.com/package/linear-program-parser) and
  [simplex](https://github.com/jeronimonunes/simplex) by jeronimonunes.
- [bigint](https://github.com/jeronimonunes/bigint) originally by Matt McCutchen with modification by jeronimonunes.

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
