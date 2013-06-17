{
  'targets': [
    {
      'target_name': 'c-pack',
      'type': 'static_library',
      'sources': [
        'pack.c'
      ],
      'include_dirs': [
        '.'
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '.'
        ]
      }
    }
  ]
}
