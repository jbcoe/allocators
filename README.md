# Notes, code samples and a presentation about allocators for C++

[![codecov][badge.codecov]][codecov] [![language][badge.language]][language]
[![license][badge.license]][license] [![issues][badge.issues]][issues]
[![pre-commit][badge.pre-commit]][pre-commit]

[badge.language]: https://img.shields.io/badge/language-C%2B%2B14-yellow.svg
[badge.codecov]: https://img.shields.io/codecov/c/github/jbcoe/allocators/master.svg?logo=codecov
[badge.license]: https://img.shields.io/badge/license-MIT-blue.svg
[badge.issues]: https://img.shields.io/github/issues/jbcoe/allocators.svg
[badge.pre-commit]: https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit
[codecov]: https://codecov.io/gh/jbcoe/allocators
[language]: https://en.wikipedia.org/wiki/C%2B%2B14
[license]: https://en.wikipedia.org/wiki/MIT_License
[issues]: http://github.com/jbcoe/allocators/issues
[pre-commit]: https://github.com/pre-commit/pre-commit
[value-types]: https://github.com/jbcoe/value_types

We've worked for some time on new [value types for C++][value-types]
and were invited to add allocator support by the C++ Standard Library
Evolution Working Group.

We were advised that allocator support could not be added later, it needed to be
part of the types from the beginning.

We set about learning about allocators and how to add them to our types. This
repository is a collection of notes, examples and some talks about what we
learned.

We spoke to many C++ experts on our journey. Wisdom is theirs, mistakes are our
own.
