# Introduction

The team behind the [MoveTK](https://github.com/heremaps/MoveTK) gratefully accepts contributions via
[pull requests](https://help.github.com/articles/about-pull-requests/) filed against the
[GitHub project](https://github.com/heremaps/MoveTK/pulls).

If you not familiar with how to contribute via pull requests to a GitHub repository see [Contributing to a Project](https://git-scm.com/book/en/v2/GitHub-Contributing-to-a-Project).

## Signing each Commit

As part of filing a pull request we ask you to sign off the
[Developer Certificate of Origin](https://developercertificate.org/) (DCO) in each commit.
Any Pull Request with commits that are not signed off will be reject by the
[DCO check](https://probot.github.io/apps/dco/).

A DCO is lightweight way for a contributor to confirm that you wrote or otherwise have the right
to submit code or documentation to a project. Simply add `Signed-off-by` as shown in the example below
to indicate that you agree with the DCO.

An example signed commit message:

```
    README.md: Fix minor spelling mistake

    Signed-off-by: John Doe <john.doe@example.com>
```

Git has the `-s` flag that can sign a commit for you, see example below:

`$ git commit -s -m 'README.md: Fix minor spelling mistake'`

`$ git push origin [name_of_your_new_branch]`

## Development

##### File Names

- End C++ files with `.cpp` and header files with `.h`.
- Name header files in CamelCase. For example, `MyClassName.h`.
- Name code files in snake_case. For example, `my_source.cpp` .
- Begin test files with `test_`. All test files should be placed under the tests folder.

### File Headers

Every `.cpp`, `.h`, and `.inl` file must contain the following header at the very top:

```cpp
/*
 * Copyright (C) 2020 HERE Europe B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 * License-Filename: LICENSE
 */
```

### Style Guidelines for Commit Messages

We strive to maintain a high software quality standard and strongly encourage contributions to keep up with the the guidelines.
In general, we share the view on how a commit message should be written with the [Git project itself](https://github.com/git/git/blob/master/Documentation/SubmittingPatches).

When you create a commit message, follow these guidelines:

- [Make separate commits for logically separate changes](https://github.com/git/git/blob/e6932248fcb41fb94a0be484050881e03c7eb298/Documentation/SubmittingPatches#L43). For example, simple formatting changes that do not affect software behavior usually do not belong in the same commit as changes to program logic.
- [Describe your changes well](https://github.com/git/git/blob/e6932248fcb41fb94a0be484050881e03c7eb298/Documentation/SubmittingPatches#L101). Do not just repeat in prose what is "obvious" from the code, but provide a rationale explaining why you believe your change is necessary.
- [Describe your changes in the imperative mood and present tense](https://github.com/git/git/blob/e6932248fcb41fb94a0be484050881e03c7eb298/Documentation/SubmittingPatches#L133). Instead of writing "Fixes an issue with encoding" prefer "Fix an encoding issue". Think about it like the commit only does something if it is applied. Such approach usually results in more concise commit messages.
- [We are picky about whitespaces](https://github.com/git/git/blob/e6932248fcb41fb94a0be484050881e03c7eb298/Documentation/SubmittingPatches#L95). Trailing whitespace and duplicate blank lines are simply an annoyance, and most Git tools flag them red in the diff anyway.
- Limit the first line to 72 characters or less.
- If you ever wondered how a nearly "perfect" commit message looks like, have a look at one of [Jeff King's commits](https://github.com/git/git/commits?author=peff) in the Git project for references and ideas.
- When you address a review comments in a pull request, please fix the issue in the commit where it appears, not in a new commit on top of the pull request history. While this requires force-pushing of the new iteration of you pull request branch, it has several advantages:
  - Reviewers that go through (larger) pull requests commit by commit are always up-to-date with the latest fixes instead of coming across a commit that addresses one comment or possibly also their remarks only at the end.
  - It maintains a cleaner history without distracting commits like "Address review comments".
  - As a result, tools like [git-bisect](https://git-scm.com/docs/git-bisect) can operate in a more meaningful way.
  - When you fix up commits, you can also change your commit messages, which is not possible if you just add new commits.
- If you are unfamiliar with how to fix up the existing commits, please read about [rewriting history](https://git-scm.com/book/id/v2/Git-Tools-Rewriting-History) and `git rebase --interactive` in particular.
- To resolve conflicts, rebase pull request branches onto their target branch instead of merging the target branch into the pull request branch—such approach results in a cleaner history without "criss-cross" merges.

## Contributors

*Aniket Mitra* and *Onur Derin* have been responsible for the design and implementation of the core components of the library. 

We also like to thank the following contributors:

 1.  *Wouter Jongeling* - Model Based Segmentation Algorithm
 2.  *Ping Zhou* - Dockerization of MoveTK
 3.  *Mees van de Kerkhof* - Trajectory Outlier Detection Algorithms
 4.  *Bram Custers* - Trajectory Statistics Algorithms   
 5.  *Matei Stroila* - Dockerization with JupyterLab
 6.  *Sian Teesdale* - Testing of the Monotone Segmentation Algorithm

