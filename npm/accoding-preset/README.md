# @annouimo/c-exam-kit-preset

This npm package contains **C99 component data and a versioned Chinese reference manual** for [Accoding Companion](https://github.com/Annouimo/accoding-companion).

## Build locally

From `npm/accoding-preset/`:

```sh
npm pack --dry-run
npm pack
```

The `prepack` step reads the maintained C headers and adjacent documentation in `scaffold/presets/exam-io-c99/`. It writes each declaration, its C code and its direct dependencies into `preset/manifest.json` (schema 2). Missing dependencies, duplicate declarations and dependency cycles fail the build.

The same headers generate `reference.json` for the Companion TUI. Companion generates the full IDE header once per workspace version and selects only referenced declarations and their dependencies for each submission.

The Companion launcher installs the package through npm. When a course or exercise adopts a preset version, Companion saves that version in the workspace.

The release workflow validates and publishes the packed package to npm.
