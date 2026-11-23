# @annouimo/c-exam-kit-preset (draft)

This npm package contains **C99 header/template data only**. It does not run code on installation, download assets at runtime, or replace the independent C Exam Kit project.

## Build locally

From `npm/accoding-preset/`:

```sh
npm pack --dry-run
npm pack
```

The `prepack` step copies `scaffold/presets/exam-io-c99/` from the repository into the published `preset/` directory after validating component paths.

Accoding Companion's external installer downloads this package into an explicitly versioned, user-owned data directory. Existing exercise snapshots are never changed just because a new version was downloaded.

Release is intentionally blocked by `private: true` until the package contents and integration tests are accepted.
