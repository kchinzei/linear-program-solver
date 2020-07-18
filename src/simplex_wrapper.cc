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
  Kiyo Chinzei
  https://github.com/kchinzei/linear-program-solver
*/

#include <napi.h>
#include "BigInteger.hh"
#include "Fraction.hh"
#include "Simplex.hh"
#include "Tabloid.hh"

#define WRAPPER_FUNCTION_NAME "simplex_wrapper"

using namespace std;

/*
  Helper
 */
bool isFraction(Napi::Value val) {
    if (!val.IsObject())
        return false;
    Napi::Object obj = val.As<Napi::Object>();
    if (obj.Has("numerator") && obj.Has("denominator"))
        return true;
    else
        return false;
}

Fraction Obj2Fraction(Napi::Value val) {
    Napi::Object obj = val.As<Napi::Object>();
    bool lossless = false;

    int64_t n = obj.Get("numerator").As<Napi::BigInt>().Int64Value(&lossless);
    int64_t d = obj.Get("denominator").As<Napi::BigInt>().Int64Value(&lossless);

    BigInteger N = BigInteger((long)n);
    BigInteger D = BigInteger((long)d);
    Fraction f = Fraction(N, D);
    return f;
}

/*
  export function solve(a: Fraction[][], b:Fraction[], c: Fraction[], vars: string[])
  : { result: ( 'otima' | 'ilimitada' | 'inviavel' ), solution: number[], vars: string[] }
 */
/*
  Acknowledgement:
  Part of code translated from Main.cc, Io,cc in https://github.com/jeronimonunes/simplex
 */
Napi::Value Solve(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 4) {
    Napi::TypeError::New(env, "Wrong number of arguments: expecting 4").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!info[0].IsArray()) {
      Napi::TypeError::New(env, "Wrong arguments: expecting a[]").ThrowAsJavaScriptException();
      return env.Null();
  }
  if (!info[1].IsArray()) {
      Napi::TypeError::New(env, "Wrong arguments: expecting b[]").ThrowAsJavaScriptException();
      return env.Null();
  }
  if (!info[2].IsArray()) {
      Napi::TypeError::New(env, "Wrong arguments: expecting c[]").ThrowAsJavaScriptException();
      return env.Null();
  }
  if (!info[3].IsArray()) {
      Napi::TypeError::New(env, "Wrong arguments: expecting vars[]").ThrowAsJavaScriptException();
      return env.Null();
  }

  Napi::Array a = info[0].As<Napi::Array>();
  Napi::Array b = info[1].As<Napi::Array>();
  Napi::Array c = info[2].As<Napi::Array>();
  Napi::Array vars = info[3].As<Napi::Array>();

  // Consistency check
  Napi::Value aTmp = a[(uint32_t)0];
  if (!aTmp.IsArray()) {
      Napi::TypeError::New(env, "Wrong arguments: expecting a[][]").ThrowAsJavaScriptException();
      return env.Null();
  }
  Napi::Array aa = aTmp.As<Napi::Array>();

  uint32_t asiz = a.Length();
  uint32_t bsiz = b.Length();
  uint32_t csiz = c.Length();
  uint32_t vsiz = vars.Length();
  uint32_t aasiz = aa.Length();

  if (asiz != bsiz) {
      Napi::TypeError::New(env, "Wrong arguments: expecting a.length === b.length").ThrowAsJavaScriptException();
      return env.Null();
  }
  if (aasiz != csiz) {
      Napi::TypeError::New(env, "Wrong arguments: expecting a[0].length === c.length").ThrowAsJavaScriptException();
      return env.Null();
  }
  if (csiz != vsiz) {
      Napi::TypeError::New(env, "Wrong arguments: expecting c.length === vars.length").ThrowAsJavaScriptException();
      return env.Null();
  }
  if (!isFraction(aa[(uint32_t)0])) {
      Napi::TypeError::New(env, "Wrong arguments: expecting a: Fraction[][]").ThrowAsJavaScriptException();
      return env.Null();
  }
  if (!isFraction(b[(uint32_t)0])) {
      Napi::TypeError::New(env, "Wrong arguments: expecting b: Fraction[]").ThrowAsJavaScriptException();
      return env.Null();
  }
  if (!isFraction(c[(uint32_t)0])) {
      Napi::TypeError::New(env, "Wrong arguments: expecting c: Fraction[]").ThrowAsJavaScriptException();
      return env.Null();
  }
  Napi::Value vars0 = vars[(uint32_t)0];
  if (!vars0.IsString()) {
      Napi::TypeError::New(env, "Wrong arguments: expecting vars: string[]").ThrowAsJavaScriptException();
      return env.Null();
  }

  // Prepare the tablau
  Matrix A;
  Vector B;
  Vector C;
  Vector certificate;
  Matrix certificateMatrix;
  Fraction v;

  uint32_t i,j;

  for (i=0; i< bsiz; i++) {
      Vector certificateLine;
      for (j=0; j<bsiz; j++) {
          Fraction eij = Fraction(i == j? 1:0);
          certificateLine.push_back(eij);
      }
      certificateMatrix.push_back(certificateLine);
      certificate.push_back(Fraction(0));
  }
  
  for (i=0; i< bsiz; i++) {
      Vector restrictionLine;
      Napi::Value aiv = a[i];
      Napi::Array ai = aiv.As<Napi::Array>();
      for (j=0; j<csiz; j++) {
          restrictionLine.push_back(Obj2Fraction(ai[j]));
      }
      A.push_back(restrictionLine);
      B.push_back(Obj2Fraction(b[i]));
  }

  for (j=0; j<csiz; j++)
      C.push_back(Obj2Fraction(c[j]));

  v = Fraction(0);

  // Solve it
  Tabloid t = Tabloid(certificate, certificateMatrix, A, B, C, v, Base());
  Result result = runSimplex(t);
  std::string rStr;
  
  switch (result.type) {
  case ResultType::ILIMITED:
      rStr = "ilimitada";
      cerr << rStr << endl;
      cerr << result.solution << endl;
      cerr << result.certificate << endl;
      break;
  case ResultType::INFEASIBLE:
      rStr = "inviavel";
      cerr << rStr << endl;
      cerr << result.certificate << endl;
      break;
  case ResultType::LIMITED:
      rStr = "otima";
      cerr << rStr << endl;
      cerr << result.value << endl;
      cerr << result.solution << endl;
      cerr << result.certificate << endl;
      break;
  }

  // Decode the result into resultObj
  Napi::Array solution = Napi::Array::New(env, result.solution.size());
  for (i=0; i<result.solution.size(); i++) {
      Fraction f = result.solution[i];
      Napi::Number v = Napi::Number::New(env, f.toDouble());
      solution[i] = v;
  }
  
  Napi::Object resultObj = Napi::Object::New(env);
  resultObj.Set("result", Napi::String::New(env, rStr));
  resultObj.Set("solution", solution);
  resultObj.Set("vars", vars);
  
  return resultObj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, WRAPPER_FUNCTION_NAME),
              Napi::Function::New(env, Solve));
  return exports;
}

NODE_API_MODULE(simplex_wrapper, Init)
