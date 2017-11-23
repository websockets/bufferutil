{
  'targets': [
    {
      'target_name': 'bufferutil',
      'xcode_settings': { 
        'CLANG_CXX_LIBRARY': 'libc++',
        'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
        'MACOSX_DEPLOYMENT_TARGET': '10.8'
      },
      'include_dirs': ['<!@(node -p "require(\'node-addon-api\').include")'],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
      'sources': [ 'src/bufferutil.cc' ]
    }
  ]
}
