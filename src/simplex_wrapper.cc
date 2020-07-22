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
#define USAGE ": { a: Fraction[][]; b: Fraction[]; c: Fraction[]; vars: string[] } => \n { result: ( 'otima' | 'ilimitada' | 'inviavel' ), solution: number[], vars: string[] }"

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

    // Note: Napi::BigInt is available in N-API ver 5 or above.
    // cf: node_modules/node-addon-api/napi.h
    // cf: https://nodejs.org/api/n-api.html#n_api_n_api_version_matrix
#if NAPI_VERSION > 5
    bool lossless = false;
    int64_t n = obj.Get("numerator").As<Napi::BigInt>().Int64Value(&lossless);
    int64_t d = obj.Get("denominator").As<Napi::BigInt>().Int64Value(&lossless);
#else
    // dummy code
    int64_t n = 0;
    int64_t d = 0;
#endif  // NAPI_VERSION > 5

    // FIXME: coeration from int64_t to long is not perservative. But BigInteger does not accept long long.
    BigInteger N = BigInteger((long)n);
    BigInteger D = BigInteger((long)d);
    Fraction f = Fraction(N, D);
    return f;
}

Napi::Value Usage(Napi::Env env, string msg) {
    Napi::TypeError::New(env, msg + "\nUsage: " + WRAPPER_FUNCTION_NAME + USAGE).ThrowAsJavaScriptException();
    return env.Null();
}

/*
  export function solve(a: Fraction[][], b:Fraction[], c: Fraction[], vars: string[])
  : { result: ( 'otima' | 'ilimitada' | 'inviavel' ), solution: number[], vars: string[] }
 */
/*
  Acknowledgement:
  Part of code translated from Main.cc, Io,cc in https://github.com/jeronimonunes/simplex
*/
/*
  Because some part of simplex source is written by C++-17, some old environment won't compile.
  Since typescript does not allow conditional import, we provide a binary that works as a dummy
  even when compiler cannot do.
 */
Napi::Value Solve(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  // When no argument, it returns if it actually works.
  if (info.Length() == 0) {
      bool ok;
#if __cplusplus >= 201703L && NAPI_VERSION > 5
      ok = true;
#else
      ok = false;
#endif
      Napi::Boolean resultObj = Napi::Boolean::New(env, ok);
      return resultObj;
  }

  // Main work.
  if (info.Length() > 1) {
      return Usage(env, "Wrong number of arguments: expecting 1");
  if (!info[0].IsObject())
      return Usage(env, "Wrong argument.");
  }

#if __cplusplus >= 201703L && NAPI_VERSION > 5

  Napi::Object obj = info[0].As<Napi::Object>();
  if (!obj.Has("a") || !obj.Has("b") || !obj.Has("c") || !obj.Has("vars"))
      return Usage(env, "Wrong argument.");
  Napi::Value va = obj.Get("a");
  Napi::Value vb = obj.Get("b");
  Napi::Value vc = obj.Get("c");
  Napi::Value vv = obj.Get("vars");
  if (!va.IsArray() || !vb.IsArray() || !vc.IsArray() || !vv.IsArray())
      return Usage(env, "Wrong arguments: expecting array");

  Napi::Array a = va.As<Napi::Array>();
  Napi::Array b = vb.As<Napi::Array>();
  Napi::Array c = vc.As<Napi::Array>();
  Napi::Array vars = vv.As<Napi::Array>();

  // Consistency check
  // FIXME: why 'a[0].IsArray()' got error?
  Napi::Value aTmp = a[(uint32_t)0];
  if (!aTmp.IsArray())
      return Usage(env, "Wrong arguments: expecting a[][]");

  uint32_t asiz = a.Length();
  uint32_t bsiz = b.Length();
  uint32_t csiz = c.Length();
  uint32_t vsiz = vars.Length();
  Napi::Array aa = aTmp.As<Napi::Array>();
  uint32_t aasiz = aa.Length();

  if (asiz != bsiz)
      return Usage(env, "Wrong arguments: expecting a.length === b.length");
  if (aasiz != csiz)
      return Usage(env, "Wrong arguments: expecting a[0].length === c.length");
  if (csiz != vsiz)
      return Usage(env, "Wrong arguments: expecting c.length === vars.length");
  if (!isFraction(aa[(uint32_t)0]))
      return Usage(env, "Wrong arguments: expecting a: Fraction[][] of 'linear-program-parser', NOT that of 'fraction.js'.");
  if (!isFraction(b[(uint32_t)0]))
      return Usage(env, "Wrong arguments: expecting b: Fraction[] of 'linear-program-parser', NOT that of 'fraction.js'.");
  if (!isFraction(c[(uint32_t)0]))
      return Usage(env, "Wrong arguments: expecting c: Fraction[] of 'linear-program-parser', NOT that of 'fraction.js'.");
  Napi::Value vars0 = vars[(uint32_t)0];
  if (!vars0.IsString())
      return Usage(env, "Wrong arguments: expecting vars: string[]");

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
      // FIXME: how to avoid 'a[i][j].As<>()' got error?
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
      // cerr << rStr << endl;
      // cerr << result.solution << endl;
      // cerr << result.certificate << endl;
      break;
  case ResultType::INFEASIBLE:
      rStr = "inviavel";
      // cerr << rStr << endl;
      // cerr << result.certificate << endl;
      break;
  case ResultType::LIMITED:
      rStr = "otima";
      // cerr << rStr << endl;
      // cerr << result.value << endl;
      // cerr << result.solution << endl;
      // cerr << result.certificate << endl;
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

#else // __cplusplus >= 201703L

  Napi::Object resultObj = Napi::Object::New(env);
  resultObj.Set("result", Napi::String::New(env, "inviavel"));
  resultObj.Set("solution", Napi::Array());
  resultObj.Set("vars", Napi::Array());

#endif // __cplusplus >= 201703L

  return resultObj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, WRAPPER_FUNCTION_NAME),
              Napi::Function::New(env, Solve));
  return exports;
}

NODE_API_MODULE(simplex_wrapper, Init)
