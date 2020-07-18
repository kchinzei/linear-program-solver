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
[Linear-program-parser](https://www.npmjs.com/package/linear-program-parser) is the newest and personally easy to use among them.
But this is a parser to generate a standard form.
You need to provide it [simplex](https://github.com/jeronimonunes/simplex) with nicely arrange the standard form to a simplex tableau.
Linear prorgram solver can accept the output of [linear-program-parser](https://www.npmjs.com/package/linear-program-parser) directly.

## Code snippets

```TypeScript

import { parse, Fpi } from 'linear-program-parser';
import { simplex } from 'linear-program-solver';

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

const { result, solution, vars } = simplex.solve(fpi.toMatrix());

// const result: ( 'otima' | 'ilimitada' | 'inviavel' );
// const solution: number[];
// const vars: string[];
```

_Result_ is either of 'otima', 'ilimitada', 'inviavel' (in Portuguese), which mean 'great, a feasible solution found in limited iterations', 'okey, a solution found but iteration didn't finish within limited times', 'sorry, it's infeasible to solve'.

_Solution_ and _vars_ are in pair. _Vars_ is an array of given variables to solve and slack variables (_f_1_ ... ). If a given variable is not constrained nonnegative, it is replaced by two nonnegative variables. If your variable is _x_, it will be _xp_ and _xn_.

## Install

```Shell
npm i linear-program-solver
```

During installation, it checks if your C++ compiler supports C++-17.
If you see an error message like

```Shell
> npm i linear-program-solver
  ...
  Build error: C++ compiler that supports c++-17 or newer required.
  ...
```

You need to manage to get one. For Raspberry pi, it temporarily downloads gcc version 9.1 binary.
