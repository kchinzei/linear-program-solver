/*
The MIT License (MIT)

Copyright (c) Kiyo Chinzei (kchinzei@gmail.com)

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
*/

/*
  linear-program-solver

  JS/TS Wrapper for simplex (C++) by jeronimonunes (https://github.com/jeronimonunes/simplex)
  Kiyo Chinzei
  https://github.com/kchinzei/linear-program-solver
*/


import { Fraction } from 'linear-program-parser';
// eslint-disable-next-line @typescript-eslint/no-unsafe-assignment, @typescript-eslint/no-unsafe-call, @typescript-eslint/no-var-requires
const simplexWrapper = require('bindings')('simplex_wrapper');

// eslint-disable-next-line @typescript-eslint/no-unsafe-assignment, @typescript-eslint/no-unsafe-member-access
export const simplex: (a: Fraction[][], b:Fraction[], c: Fraction[], vars: string[]) => { result: ( 'otima' | 'ilimitada' | 'inviavel' ); solution: number[]; vars: string[] } = simplexWrapper.simplex_wrapper;

/*
  Find solution in vars by variableName.
  variableName can be 'a', 'f_1', ...
  When the non-negative constraint is not given to variable 'a', variable is replaced by 'ap' and 'an'.
*/
export function findSolution(variableName: string, solution: number[], vars: string[]): number {
  let iFound = -1;
  const iMax = solution.length;

  if (solution.length !== vars.length)
    return NaN;

  for (let i=0; i < iMax; i++) {
    if (vars[i] === variableName)
      iFound = i;
  }
  if (iFound !== -1)
    return solution[iFound];

  const varP = variableName + 'p';
  const varN = variableName + 'n';
  let iFoundP = -1;
  let iFoundN = -1;

  for (let i=0; i < iMax; i++) {
    if (vars[i] === varP)
      iFoundP = i;
    else if (vars[i] === varN)
      iFoundN = i;
  }
  if (iFoundP !== -1 && iFoundN !== -1)
    return solution[iFoundP] - solution[iFoundN];

  return NaN;
}
