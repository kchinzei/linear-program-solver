{
  'targets': [
    {
      'target_name': 'simplex_wrapper',
        'sources': [
          'src/simplex_wrapper.cc',
          'simplex/src/Base.cc',
          'simplex/src/Matrix.cc',
          'simplex/src/Fraction.cc',
          'simplex/src/Simplex.cc',
          'simplex/src/Vector.cc',
          'simplex/src/Tabloid.cc',
          'simplex/bigint/BigUnsigned.cc',
          'simplex/bigint/BigInteger.cc',
          'simplex/bigint/BigIntegerAlgorithms.cc',
          'simplex/bigint/BigUnsignedInABase.cc',
          'simplex/bigint/BigIntegerUtils.cc',
        ],
      'cflags_cc+': [
        '-std=c++1z',
        '-std=c++17',
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [
        '-fno-exceptions',
        '-fno-rtti',
        '-std=c++0x',
        '-std=c++1y',
        '-std=c++11',
        '-std=c++14',
        '-std=gnu++0x',
        '-std=gnu++1y',
        '-std=gnu++11',
        '-std=gnu++14',
      ],
      'include_dirs': [
         "<!@(node -p \"require('node-addon-api').include\")",
         "simplex/src",
         "simplex/bigint",
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      'conditions': [
         ['OS == "mac"', {
           'cflags+': [ '-fvisibility=hidden' ],
           'xcode_settings': {
             'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
             'OTHER_CFLAGS': [
               '-std=c++17',
               '-stdlib=libc++',
               '-fvisibility=hidden',
             ],
           },
         }],
         ['OS == "win"', {
           # windows settings
         }],
      ],
    },
  ]
}
