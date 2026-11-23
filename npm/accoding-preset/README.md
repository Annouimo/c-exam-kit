# @annouimo/c-exam-kit-preset

This npm package contains **C99 component data and a versioned Chinese reference manual** for [Accoding Companion](https://github.com/Annouimo/accoding-companion).

## Build locally

From `npm/accoding-preset/`:

```sh
npm pack --dry-run
npm pack
```

The `prepack` step reads the maintained C99 headers in `scaffold/presets/exam-io-c99/`. It emits one component per definition and writes its transitive inputs as direct edges in `preset/manifest.json`. A component can be a function, a macro, a type, or a callback declaration. Identical definitions shared by source headers are emitted once. Conflicting definitions and dependency cycles fail the build.

The package carries the manifest and component fragments. Accoding Companion assembles the full IDE header once per workspace version and performs a separate dependency traversal for each submission.

The Companion launcher installs the package through npm. When a course or exercise adopts a preset version, Companion saves that version in the workspace.

The release workflow validates and publishes the packed package to npm.
