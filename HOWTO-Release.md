
# How to release

This is a reminder for whoever maintains the repository and wants to produce a release.


In src/common/version.c update the version:

```
#define VER_MAJOR       2U
#define VER_MINOR       19U
```

now push your changes

```
$ git push origin
```


When everything is done and compiles, tag the release.
CAUTION: the tag _must_ start with V

```
$ git tag -a "V2.19-test-20260926" -m "testing PR #2988"
$ git push origin "V2.19-test-20260926"
$ git tag
V2.12.0
V2.13.0
V2.13.0rc1
V2.13.0rc2
V2.13.0rc4
V2.13.1
V2.13.2
V2.13.3
V2.14
V2.15
V2.16
V2.17
V2.18
V2.19
V2.19-test-20260926
```


To delete a tag incase of a mistake:

```
$ git tag -d "V2.19-test-20260926"
$ git push origin :refs/tags/V2.19-test-20260926
$ git fetch --tags
$ git tag
V2.12.0
V2.13.0
V2.13.0rc1
V2.13.0rc2
V2.13.0rc4
V2.13.1
V2.13.2
V2.13.3
V2.14
V2.15
V2.16
V2.17
V2.18
V2.19
```
