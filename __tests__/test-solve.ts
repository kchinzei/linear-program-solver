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
  Test solver

  linear-program-solver
  Kiyo Chinzei
  https://github.com/kchinzei/linear-program-solver
*/

import { parse, Fpi, Fraction } from 'linear-program-parser';
import { simplex, findSolution } from '../src/index';

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
   'otima', 0, 0, 10, 0, 5, 40, 0, 0],

  // Probelm is x10ed. Should be same solution.
  ['max(-30a -40b +50c -50d) \n\
    st: \n\
        +10a +10b  +0c  +0d <= +50; \n\
        -10a  +0b -50c +50d <= -100; \n\
        +20a +10b +10c -10d <= +100; \n\
        -20a -10b -10c +10d <= -100; \n\
        a >= 0; \n\
        b >= 0; \n\
        c >= 0; \n\
        d >= 0; ',
   'otima', 0, 0, 10, 0, 50, 400, 0, 0],

  // Fractionl introduced. Problem is /100ed.
  ['max(3a -4b +5c -5d) \n\
    st: \n\
        +1/100*a +1/100*b +0/100*c +0/100*d <= +5/100; \n\
        -1/100*a +0/100*b -5/100*c +5/100*d <= -10/100; \n\
        +2/100*a +1/100*b +1/100*c -1/100*d <= +10/100; \n\
        -2/100*a -1/100*b -1/100*c +1/100*d <= -10/100; \n\
        a >= 0; \n\
        b >= 0; \n\
        c >= 0; \n\
        d >= 0; ',
   'otima', 0, 0, 10, 0, 1/20, 2/5, 0, 0],

  // Same condition, proportional objective function >> Should give same solution.
  ['max(3/100*a -4/100*b +5/100*c -5/100*d) \n\
    st: \n\
        +1/100*a +1/100*b +0/100*c +0/100*d <= +5/100; \n\
        -1/100*a +0/100*b -5/100*c +5/100*d <= -10/100; \n\
        +2/100*a +1/100*b +1/100*c -1/100*d <= +10/100; \n\
        -2/100*a -1/100*b -1/100*c +1/100*d <= -10/100; \n\
        a >= 0; \n\
        b >= 0; \n\
        c >= 0; \n\
        d >= 0; ',
   'otima', 0, 0, 10, 0, 1/20, 2/5, 0, 0],

])('', (problem, res, a, b, c, d, f_1, f_2, f_3, f_4) => {
  test(`${i++}. Examine answer given by https://jeronimonunes.github.io/simplex-web`, () => {
    const linearProgram = parse(problem);
    const fpi: Fpi = linearProgram.toFPI();
    const tableau: {a: Fraction[][]; b: Fraction[]; c: Fraction[]; vars: string[]} = fpi.toMatrix();
    const { result, solution, vars } = simplex(tableau.a, tableau.b, tableau.c, tableau.vars);

    expect(res).toBe(result);
    expect(findSolution('a', solution, vars)).toBeCloseTo(a);
    expect(findSolution('b', solution, vars)).toBeCloseTo(b);
    expect(findSolution('c', solution, vars)).toBeCloseTo(c);
    expect(findSolution('d', solution, vars)).toBeCloseTo(d);
  });
});






// non-negative constraints, unlimited solution
describe.each([
  // Problem and solution given by https://jeronimonunes.github.io/simplex-web/
  ['max(-3a -4b +5c -5d) \n\
    st: \n\
        +1a +1b +0c +0d <= +5; \n\
        -1a +0b -5c +5d <= -10; \n\
        +2a +1b +1/10*c -1d <= +10; \n\
        -2a -1b -1c +1d <= -10; \n\
        a >= 0; \n\
        b >= 0; \n\
        c >= 0; \n\
        d >= 0; ',
     'ilimitada', 0, 0, 100, 0, 5, 490, 0, 90]

])('', (problem, res, a, b, c, d, f_1, f_2, f_3, f_4) => {
  test(`${i++}. Examine answer given by https://jeronimonunes.github.io/simplex-web`, () => {
    const linearProgram = parse(problem);
    const fpi: Fpi = linearProgram.toFPI();
    const tableau: {a: Fraction[][]; b: Fraction[]; c: Fraction[]; vars: string[]} = fpi.toMatrix();
    const { result, solution, vars } = simplex(tableau.a, tableau.b, tableau.c, tableau.vars);

    expect(res).toBe(result);
    expect(findSolution('a', solution, vars)).toBeCloseTo(a);
    expect(findSolution('b', solution, vars)).toBeCloseTo(b);
    expect(findSolution('c', solution, vars)).toBeCloseTo(c);
    expect(findSolution('d', solution, vars)).toBeCloseTo(d);
  });
});







// non-negative constraints, infeasible proble (= no solution)
describe.each([
  // Problem and solution given by https://jeronimonunes.github.io/simplex-web/
  ['max(-3a -4b +5c -5d) \n\
    st: \n\
        +1a +1b +0c +0d <= +5; \n\
        -1a +0b -5c +5d <= -10; \n\
        +2a +1b +1c -1d <= +10; \n\
        -2a -1b +1c +1d <= -10; \n\
        a >= 0; \n\
        b >= 0; \n\
        c >= 0; \n\
        d >= 0; ',
   'inviavel'],

])('', (problem, res) => {
  test(`${i++}. Examine answer given by https://jeronimonunes.github.io/simplex-web`, () => {
    const linearProgram = parse(problem);
    const fpi: Fpi = linearProgram.toFPI();
    const tableau: {a: Fraction[][]; b: Fraction[]; c: Fraction[]; vars: string[]} = fpi.toMatrix();
    // @ts-ignore: TS6133
    const { result, solution, vars } = simplex(tableau.a, tableau.b, tableau.c, tableau.vars);

    expect(res).toBe(result);
  });
});





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
        b >= -4; \n\
        c >= 0; ',
   'otima', 0, -4, 14, 0],

  // Same condition, proportional objective function >> Should give same solution.
  ['max(3/100*a -4/100*b +5/100*c -5/100*d) \n\
    st: \n\
        +1/100*a +1/100*b +0/100*c +0/100*d <= +5/100; \n\
        -1/100*a +0/100*b -5/100*c +5/100*d <= -10/100; \n\
        +2/100*a +1/100*b +1/100*c -1/100*d <= +10/100; \n\
        -2/100*a -1/100*b -1/100*c +1/100*d <= -10/100; \n\
        a >= 0; \n\
        b >= -4/100; \n\
        c >= 0; ',
   'otima', 0, -0.04, 10.04, 0]

])('', (problem, res, a, b, c, d) => {
  test(`${i++}. Examine answer given by https://jeronimonunes.github.io/simplex-web`, () => {
    const linearProgram = parse(problem);
    const fpi: Fpi = linearProgram.toFPI();
    const tableau: {a: Fraction[][]; b: Fraction[]; c: Fraction[]; vars: string[]} = fpi.toMatrix();
    const { result, solution, vars } = simplex(tableau.a, tableau.b, tableau.c, tableau.vars);

    expect(res).toBe(result);
    expect(findSolution('a', solution, vars)).toBeCloseTo(a);
    expect(findSolution('b', solution, vars)).toBeCloseTo(b);
    expect(findSolution('c', solution, vars)).toBeCloseTo(c);
    expect(findSolution('d', solution, vars)).toBeCloseTo(d);
  });
});




test(`${i++}. findSolution() assumes that solution and vars match`, () => {
  const solution = [0, 0, 10, 0, 5, 40, 0, 0];
  const vars = ['a', ' b', ' c', ' d', ' f_1', ' f_2', ' f_3', ' f_4'];

  for (let j=0; j<solution.length; j++) {
    expect(findSolution(vars[j], solution, vars)).toBe(solution[j]);
  }
});

test(`${i++}. findSolution() can find solution even when vars are not in the order human expects`, () => {
  const solution = [0, 14, 9, 60, 0, 0, 0, 10, 0, 0, 0, 4];
  const vars = ['a', 'c', ' f_1', 'f_2', 'f_3', 'f_4', 'f_5', 'f_6', 'dp', 'dn', 'bp', 'bn'];
  const orderedSolution = [0, -4, 14, 0];
  const orderedVars = ['a', 'b', 'c', 'd'];

  for (let j=0; j<orderedSolution.length; j++) {
    expect(findSolution(orderedVars[j], solution, vars)).toBe(orderedSolution[j]);
  }
});

test(`${i++}. findSolution() returns NaN when solution and vars unmatch`, () => {
  const solution = [0, 0, 10, 0, 5, 40, 0, 0];
  const vars = ['a', ' b', ' c', ' d', ' f_1', ' f_2', ' f_3']; // Wrong: one shorter than solution[]

  expect(findSolution('a', solution, vars)).toBe(NaN);
});

test(`${i++}. findSolution() returns NaN when variable is not found`, () => {
  const solution = [0, 0, 10, 0, 5, 40, 0, 0];
  const vars = ['a', ' b', ' c', ' d', ' f_1', ' f_2', ' f_3', 'f_4'];

  expect(findSolution('x', solution, vars)).toBe(NaN); // x is not in vars
});
