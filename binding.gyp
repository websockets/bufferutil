{
  'targets': [
    {
      'target_name': 'bufferutil',
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
      'sources': ['src/bufferutil.cc'],
      'xcode_settings': {
        'MACOSX_DEPLOYMENT_TARGET': '10.8',
        'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
        'CLANG_CXX_LIBRARY': 'libc++'
      }
    }
  ]
}
