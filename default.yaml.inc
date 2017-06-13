# Hey, emacs, this is -*- yaml -*-

- defaults: &defaults
    name: 'defaults'
    repo: '{name}'
    distributions: !!python/tuple [jessie, stretch]
    architectures: !!python/tuple [amd64, i386, armhf, arm64]
    deb_build_options: nocheck
    deb_build_profiles: nocheck
    branches: !!python/tuple [master]
    sequential: true
    repository_extra: ''
    repository_extra_keys: ''
    jobs:
      - '{name}_{package}-source'
      - '{name}_{package}-binaries'
      - '{name}_{package}-piuparts'
