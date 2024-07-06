# Hey, emacs, this is -*- yaml -*-

- job-template:
    name: '{name}_{package}-source'
    project-type: matrix
    execution-strategy:
      sequential: '{obj:sequential}'
    scm:
      - git:
          url: '{git-url}'
          name: origin
          refspec: +refs/heads/*:refs/remotes/origin/*
          basedir: source
          branches: '{obj:branches}'
          wipe-workspace: false
    axes:
      - axis:
          type: user-defined
          name: distribution
          values: '{obj:distributions}'
    builders:
      - shell: |
          rm -f ./* || true
      - shell: |
          export distribution=debian$(distro-info -r --series=${{distribution}})
          export TIMESTAMP_FORMAT="%Y%m%d"
          export PRE_DCH_HOOK="/var/lib/jenkins/bin/maybe-gen-control"
          /usr/bin/generate-git-snapshot
#      - shell: |
#          mkdir -p report
#          /usr/bin/lintian-junit-report *.dsc > report/lintian.xml
    publishers:
      - archive:
          artifacts: '*.gz,*.gz.asc,*.bz2,*.bz2.asc,*.xz,*.xz.asc,*.deb,*.dsc,*.git,*.changes,lintian.txt'
      - trigger:
          project: '{name}_{package}-binaries'
#      - junit:
#          results: '**/lintian.xml'
#          keep-long-stdio: false
      - fingerprint:
          record-artifacts: true
    properties:
      - build-discarder:
          num-to-keep: 1
          artifact-num-to-keep: 1
      
- job-template:
    name: '{name}_{package}-binaries'
    project-type: matrix
    description: |
      <p>Build Debian binary packages of {package}.<br />Do not edit this job through the web, it is generated via jenkins-job-builder!</p>
      <p>Usage instructions how to remotely access and use the repository can be found in the <a href="https://packages.sury.org/{repo}/">{repo} repository</a></p>
      <p>Or if you really hate security or you just do not care about it:
      <pre>
      curl -L https://packages.sury.org/{repo}/README.txt | bash -x
      </pre>
    execution-strategy:
      combination-filter: |
        (architecture == "amd64" && node == "amd64") || (architecture == "i386" && node == "i386") || (architecture == "armhf" && node == "armhf") || (architecture == "arm64" && node == "arm64")
      sequential: '{obj:sequential}'
      touchstone:
        expr: 'architecture == "amd64"'
        result: 'stable'
    axes:
      - axis:
          type: user-defined
          name: architecture
          values: '{obj:architectures}'
      - axis:
          type: user-defined
          name: distribution
          values: '{obj:distributions}'
      - axis:
          type: slave
          name: node
          values:
           - amd64
           - i386
           - armhf
           - arm64
    builders:
      - copyartifact:
          project: '{name}_{package}-source/distribution=${{distribution}}'
          filter: '*'
          which-build: upstream-build
          fallback-to-last-successful: true
      - shell: |
          export POST_BUILD_HOOK=/usr/bin/jdg-debc
          export release=$distribution
          export EXTRAPACKAGES="apt-transport-https"
          export RELEASE_REPOSITORY=/srv/repository/{repo}
          export RELEASE_DISTRIBUTION=$distribution
          export DEB_BUILD_OPTIONS="{deb_build_options}"
          export DEB_BUILD_PROFILES="{deb_build_profiles}"
          if [ "$release" != "sid" ]; then export REPOSITORY_EXTRA="{repository_extra}"; fi
          export REPOSITORY_EXTRA_KEYS="{repository_extra_keys}"
          /usr/bin/build-and-provide-package
      - shell: |
          /var/lib/jenkins/bin/sync-repository "{repo}"
      - shell: |
          export REPOSITORY=/srv/repository/{repo}
          echo "Listing packages inside the {repo} repository:"
          /usr/bin/repository_checker --list-repos $distribution
#      - shell: |
#          mkdir -p report
#          /usr/bin/lintian-junit-report *.dsc > report/lintian.xml
    publishers:
      - archive:
          artifacts: '*.gz,*.gz.asc,*.bz2,*.bz2.asc,*.xz,*.xz.asc,*.deb,*.dsc,*.git,*.changes,lintian.txt'
#      - junit:
#          results: '**/lintian.xml'
#          keep-long-stdio: false
      - fingerprint:
          record-artifacts: true
#      - trigger:
#          project: '{name}_{package}-piuparts'
      - workspace-cleanup:
          dirmatch: false
    wrappers:
      - timestamps
    properties:
      - build-discarder:
          num-to-keep: 1
          artifact-num-to-keep: 1

- job-template:
    name: '{name}_{package}-piuparts'
    project-type: freestyle
    description: 'Installation and upgrade tests for {package} Debian packages.<br />Do not edit this job through the web, it is generated via jenkins-job-builder!'
    disabled: false
    parameters:
      - string:
          name: architecture
          default: '{architectures[0]}'
          description: "Architecture that should be used."
      - string:
          name: distribution
          default: '{distributions[0]}'
          description: "Distribution that should be used."
    builders:
      - copyartifact:
          project: '{name}_{package}-binaries/architecture=${{architecture}},distribution=${{distribution}}'
          filter: '*.deb'
          which-build: upstream-build
          fallback-to-last-successful: true
          flatten: true
          target: 'artifacts/'
      - shell: |
          # sadly piuparts always returns with exit code 1 :((
          sudo piuparts_wrapper ${{PWD}}/artifacts/*.deb || true
      - shell: |
          piuparts_tap piuparts.txt > piuparts.tap
    publishers:
      - tap:
          results: 'piuparts.tap'
      - archive:
          artifacts: 'piuparts.*'
      - fingerprint:
          record-artifacts: true
      - workspace-cleanup:
          dirmatch: false
    wrappers:
      - timestamps
    properties:
      - build-discarder:
          num-to-keep: 1
          artifact-num-to-keep: 1
