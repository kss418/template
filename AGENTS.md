# AGENTS.md (PS C++ templates repo)

## Goal
Maintain a clean, reliable competitive-programming template repository.
Prefer small, surgical diffs; avoid rewriting unrelated files.

## Snippets sync
- If you change any reusable template code, also update the corresponding editor snippets
  (e.g., VS Code user/project snippets) in the same change.
- Keep snippet triggers/prefixes and the expanded code consistent with the repository templates.
- Do not defer snippet updates; treat snippets as part of the template interface.

## Index base contract
- For templates whose constructor takes a positional or vertex range, `n` is the
  maximum external index, not the number of elements. Use `bool base = 1` in
  constructors and initialization APIs; spell the default as `1`, not `true`.
- The active range is inclusive: `base=1` means `[1,n]`, and `base=0` means `[0,n]`.
  Compute element, vertex, component, and required-edge counts from `n-base+1`.
  For two-dimensional ranges, apply the same base to both axes.
- Append `base` after existing optional parameters so their positions are preserved.
  Forward the selected base through `clear`, `build`, preprocessing, and wrappers.
  Internal helpers may use their own indexing, but configure and translate it explicitly.
- `n=0, base=1` is empty; `n=0, base=0` contains index 0. Include both cases when
  changing range initialization or boundary behavior.
- Array `span` inputs remain index-aligned: `arr[i]` represents external index `i`.
  With `base=1`, skip `arr[0]`; a nonempty span has maximum index `arr.size()-1`.
  Do not reinterpret the span as a packed list starting at the selected base.
- Iterate over all active indices, including 0 when `base=0`. Treat vertex 0 as
  a real vertex, not an absent parent or dummy vertex. Use `-1`, root self-links,
  or explicit external/internal index conversion for sentinels.
- Keep algorithm-specific mappings consistent: `_2sat` uses `n-base+1` as its
  literal offset; `_ufset` returns boundary sentinels `base-1` and `n+1`.
  Hungarian's internal dummy index must remain separate from external index 0.
- Matrix dimensions, number-theory numeric bounds, hash capacities, and
  input-derived string indices retain their own meaning; do not add range-base
  parameters to them merely because a parameter is named `n`.
- New and refactored range templates must follow this contract. Update the
  matching snippets and README examples when their index behavior changes.
  For index-behavior changes, run `python tests/run_base_index.py` with a GNU C++20
  compiler and keep coverage for both bases, inclusive endpoints, counts, and sentinels.

## Repo sync (web tasks)
- At the start of a task, run:
  - git fetch --all --prune
- If working on the default branch:
  - ensure `git status --porcelain` is empty
  - git pull --ff-only
- Never use `git reset --hard` unless explicitly instructed.

- ## Style / conventions
- C++17/20/23 as used in the repo (follow existing).
- Keep headers minimal; prefer `#include <bits/stdc++.h>` if the repo standard uses it.
- Use consistent type aliases/macros as already present.
- No extra I/O prompts; code should be contest-ready.
