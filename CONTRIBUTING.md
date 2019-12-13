# Contributing guide

[//]: # (automatically generated from https://github.com/metwork-framework/resources/blob/master/cookiecutter/%7B%7Bcookiecutter.repo%7D%7D/CONTRIBUTING.md)

**WARNING: work in progress**





## Version numbering 

We follow the [semantic versionning specification](https://semver.org/). 

### Summary (see above specification for more details)

Given a version number `MAJOR.MINOR.PATCH`, we increment the:

- `MAJOR` version when we make incompatible API changes,
- `MINOR` version when we add functionality in a backwards-compatible manner, and
- `PATCH` version when we make backwards-compatible bug fixes.

## Commit Message Guidelines

Inspired by Angular project and [conventional commits initiative](https://www.conventionalcommits.org), 
we have very precise rules over how our git commit messages can be formatted. This leads to more readable messages that are 
easy to follow when looking through the project history. But also, we use the git commit messages to generate the project 
changelog.

So we follow the [conventional commits initiative](https://www.conventionalcommits.org) specification.

### Summary (see above specification for more details)

Each commit message consists of a `header`, a `body` and a `footer`. The `header` has a special format that includes a `type`, 
a `scope` and a `description`. The commit message should be structured as follows:

```
<type>[optional scope]: <description>
<BLANK LINE>
[optional body]
<BLANK LINE>
[optional footer]
```

The commit message contains the following structural elements, to communicate intent to the consumers of the project:

 - `fix`: a commit of the type fix patches a bug in your codebase (this correlates with `PATCH` in semantic versioning).
 - `feat`: a commit of the type feat introduces a new feature to the codebase (this correlates with `MINOR` in semantic versioning).
 - `BREAKING CHANGE`: a commit that has the text `BREAKING CHANGE:` at the beginning of its optional body or footer section 
    introduces a breaking API change (correlating with `MAJOR` in semantic versioning). 
    A breaking change can be part of commits of any type. e.g., a `fix:`, `feat:` & `chore:` types would all be valid, 
    in addition to any other type.
    Others: commit types other than fix: and feat: are allowed, for example commitlint-config-conventional (based on the the Angular convention) recommends chore:, docs:, style:, refactor:, perf:, test:, and others. We also recommend improvement for commits that improve a current implementation without adding a new feature or fixing a bug. Notice these types are not mandated by the conventional commits specification, and have no implicit effect in semantic versioning (unless they include a BREAKING CHANGE, which is NOT recommended).
    A scope may be provided to a commit’s type, to provide additional contextual information and is contained within parenthesis, e.g., feat(parser): add ability to parse arrays.

### Examples

#### Commit message with description and breaking change in body

```
feat: allow provided config object to extend other configs

BREAKING CHANGE: `extends` key in config file is now used for extending other config files
```

#### Commit message with no body

```
docs: correct spelling of CHANGELOG
```

#### Commit message with scope

```
feat(lang): added polish language
```

#### Commit message for a fix using an (optional) issue number.

```
fix: minor typos in code

see the issue for details on the typos fixed

fixes issue #12
```

### Revert

If the commit reverts a previous commit, it should begin with `revert:`, followed by the header of the reverted commit. 
In the body it should say: `This reverts commit <hash>.`, where the `hash` is the SHA of the commit being reverted.

### Type

Must be one of the following:

- `build`: Changes that affect the build or CI system (`chore` is also accepted for compatibility)
- `docs`: Documentation only changes
- `feat`: A new feature
- `fix`: A bug fix
- `perf`: A code change that improves performance
- `refactor`: A code change that neither fixes a bug nor adds a feature
- `style`: Changes that do not affect the meaning of the code (white-space, formatting, missing semi-colons, etc)
- `test`: Adding missing tests or correcting existing tests

### Scope

The scope is not used for the moment. Please don't use scopes in commit messages.

### Description

The description contains a succinct description of the change:

```
    use the imperative, present tense: "change" not "changed" nor "changes"
    don't capitalize the first letter
    no dot (.) at the end
```

### Body

Just as in the subject, use the imperative, present tense: "change" not "changed" nor "changes". The body should include 
the motivation for the change and contrast this with previous behavior.

### Footer

The footer should contain any information about `Breaking Changes` and is also the place to reference GitHub issues 
that this commit Closes.

`Breaking Changes` should start with the word `BREAKING CHANGE:` with a space or two newlines. The rest of the commit 
message is then used for this.





## Pull-requests and issues labels

We use a consistent labelling scheme inspired by [sensible-github-labels](https://github.com/Relequestual/sensible-github-labels).

### Type

- `Type: Bug`: it's about a bug
- `Type: Enhancement`: it's about a new feature
- `Type: Question`: it's just a question
- `Type: Maintenance`: it's about a better way to implement an existing feature (refactor, performances improvement...)

### Priority

- `Priority: Critical`: This should be dealt with ASAP. Not fixing this issue would be a serious error.
- `Priority: High`: After critical issues are fixed, these should be dealt with before any further issues.
- `Priority: Medium`: (implicit, does not exist as a label) This issue may be useful, and needs some attention.
- `Priority: Low` : This issue can probably be picked up by anyone looking to contribute to the project, as an entry fix.

### Status

- `Status: Pending`: The issue is new, this is the triage status.
- `Status: Closed`: The issue/pr is closed (because the corresponding change is merged or because the corresponding change was abandoned/rejected)
- `Status: Accepted`: It's clear what the subject of the issue is about, what the resolution should be, and we want this :-)
- `Status: Blocked`: There is another issue that needs to be resolved first, or a specific person is required to comment or reply to progress. There may also be some external blocker.
- `Status: In Progress`: This issue is being worked on, and has someone assigned.
- `Status: Review Needed`: The PR must be reviewed by a team member.
- `Status: Revision Needed`: Submitter of PR needs to revise the PR related to the issue.

### Labels management by `MetworkBot`

We have a bot to do some automatic labelling:

- [x] When a pr is opended, it adds the `Status: Pending` label 
- [x] When an issue is opened, it adds the `Status: Pending` label (if no other `Status: *` label was given initialy)
- [x] When a pr is closed, it removes every `Status: *` labels and adds `Status: Closed`
- [x] When an issue is closed, it removes every `Status: *` labels and adds `Status: Closed`
- [x] When a pr is reopened, it removes the `Status: Closed` label and adds `Status: Review Needed`
- [x] When an issue is reopened, it removes the `Status: Closed` label and adds `Status: Pending`
- [ ] When a new `Priority: *` label is set, old `Priority: *` labels are removed (if necessary)
- [ ] When a new `Status: *` label is set, old `Status: *` labels are removed (if necessary)
- [x] When a pr is not "good" (because of bad statuses for example), the label `Status: Revision Needed` is set
- [x] When a pr is "good" (statuses all green), the label `Status: Review Needed` is set





## Code of Conduct

The MetWork community must follow the Code of Conduct described in [this document](CODE_OF_CONDUCT.md).


