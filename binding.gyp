{
  'targets': [
    {
      'target_name': 'bufferutil',
      'include_dirs': ["<!(node -e \"require('nan')\")"],
      'sources': [ 'src/bufferutil.cc' ]
    }
  ]
}
