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
  Test solver (async)

  linear-program-solver
  Kiyo Chinzei
  https://github.com/kchinzei/linear-program-solver
*/

import { Fraction } from 'linear-program-parser';
import { simplexAsync, solveAsync, findSolution, simplexIsOK, SimplexSolution, SimplexTableau } from '../src/index';

let i=0;

/*
  Solutions obtained from descrete run of linear-program-parser and simplex.
*/

// non-negative constraints, limited solution
describe.each([
  // Problem and solution given by https://jeronimonunes.github.io/simplex-web/
  ['max(-3a -4b +5c -5d) \n\
    st: \n\
        +1a +1b +0c +0d <= +5; \n\
        -1a +0b -5c +5d <= -10; \n\
        +2a +1b +1c -1d <= +10; \n\
        -2a -1b -1c +1d <= -10; \n\
        a >= 0; \n\
        b >= 0; \n\
        c >= 0; \n\
        d >= 0; ',
   'otima', 0, 0, 10, 0, 5, 40, 0, 0]
])('', (problem, resultTxt, a, b, c, d, f_1, f_2, f_3, f_4) => {
  test(`${i++}. `, async () => {
    const res: SimplexSolution = await solveAsync(problem);

    if (simplexIsOK()) {
      expect(res.result).toBe(resultTxt);
      expect(findSolution('a', res.solution, res.vars)).toBeCloseTo(a);
      expect(findSolution('b', res.solution, res.vars)).toBeCloseTo(b);
      expect(findSolution('c', res.solution, res.vars)).toBeCloseTo(c);
      expect(findSolution('d', res.solution, res.vars)).toBeCloseTo(d);
    } else {
      expect(res.result).toBe('inviavel');
    }
  });
});




describe.each([
  // Syntax error in problem. (missing '-' at -5d)
  ['max(-3a -4b +5c 5d) \n\
    st: \n\
        +1a +1b +0c +0d <= +5; \n\
        -1a +0b -5c +5d <= -10; \n\
        +2a +1b +1c -1d <= +10; \n\
        -2a -1b -1c +1d <= -10; \n\
        a >= 0; \n\
        b >= 0; \n\
        c >= 0; \n\
        d >= 0; ',
   'SyntaxError: Expected \")\", \"*\", \"+\", \"-\", or \"/\" but \"5\" found.'
  ],
])('', (problem, errMsg) => {
  test(`${i++}. Syntax error in problem`, async () => {
    expect.assertions(1);
    try {
      await solveAsync(problem);
    } catch (e) {
      expect(e.toString()).toMatch(errMsg);
    }
  });
});



// Error in SimplexTableau
test(`${i++}. Erroneous SimplexTableau should arise reject`, async () => {
  expect.assertions(1);

  const a: Fraction[][] = [];
  const b: Fraction[] = [];
  const c: Fraction[] = [];
  const vars: string[] = [];
  const tableau: SimplexTableau<Fraction> = { a, b, c, vars };
    try {
      await simplexAsync(tableau);
    } catch (e) {
      expect(e.toString()).toMatch('');
    }
  });
