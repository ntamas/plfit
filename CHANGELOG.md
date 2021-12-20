# plfit library changelog

## [Unreleased]

## [0.9.2]

### Changed

* Header files are now installed in the `plfit` subfolder of the designated
  include directory.

### Fixed

* Expected output from tests cases is now printed on `stdout`, not `stderr`

## [0.9.1]

### Fixed

* Fixed a bug in `plfit_discrete()` that returned a misleading error message
  when some of the samples in `xs` was less than 1 and the remaining samples
  did not contain at least 3 unique values.

## [0.9.0]

### Added

* This version now compiles on Windows as well as Linux and macOS.

### Fixed

* Fixed potential naming conflicts with other libraries by prefixing exported
  symbols with `plfit_` where appropriate, thanks to @jgmbenoit

* Lots of enhancements to facilitate inclusion in Debian Linux as a separate
  package, thanks to @jgmbenoit

* Tidied up CMakeLists.txt file

* Fixed a bug with the Walker alias sampler that sometimes returned incorrect
  samples on Windows.

## [0.8.2]

First version with a changelog entry.

[Unreleased]: https://github.com/ntamas/plfit/compare/0.9.2..HEAD
[0.9.2]: https://github.com/ntamas/plfit/compare/0.9.1...0.9.2
[0.9.1]: https://github.com/ntamas/plfit/compare/0.9.0...0.9.1
[0.9.0]: https://github.com/ntamas/plfit/compare/0.8.2...0.9.0
[0.8.2]: https://github.com/ntamas/plfit/releases/tag/0.8.2

