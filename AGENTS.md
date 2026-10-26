# AGENTS.md (PS C++ templates repo)

## Goal
Maintain a clean, reliable competitive-programming template repository.
Prefer small, surgical diffs; avoid rewriting unrelated files.

## Snippets sync
- If you change any reusable template code, also update the corresponding editor snippets
  (e.g., VS Code user/project snippets) in the same change.
- Keep snippet triggers/prefixes and the expanded code consistent with the repository templates.
- Do not defer snippet updates; treat snippets as part of the template interface.

- Keep README.md limited to the algorithm-location guide and snippets section;
  maintain API conventions in AGENTS.md or source comments.

## Index base contract
- For templates whose constructor takes a positional or vertex range, `n` is the
  maximum external index, not the number of elements. Use `bool base = 1` in
  constructors and range-resetting `clear` APIs; spell the default as `1`, not `true`.
- The active range is inclusive: `base=1` means `[1,n]`, and `base=0` means `[0,n]`.
  Compute element, vertex, component, and required-edge counts from `n-base+1`.
  For two-dimensional ranges, apply the same base to both axes.
- Append `base` after existing optional parameters so their positions are preserved.
  Store the selected base in the object. `make(arr)` must not accept a base parameter:
  it uses and preserves the stored base, including empty input and resizing.
  Only constructors and `clear` select a new base; calculation-only `build` also preserves it.
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

## Computation lifecycle contract
- Use `build(...)` for preprocessing/calculation. Every call recomputes derived state
  from the retained current input; do not silently continue previous calculations.
- Use `make(...)` for whole-input replacement (arrays, strings, or a new virtual-tree
  node set). A make operation may finish the preparation needed for immediate queries.
  Keep point `set`/`update` and incremental input `add`/`insert` names.
  Bulk `make(arr)` reuses the object's configured base; do not add a base argument.
- Queries requiring preprocessing before the first build, or after input changes and
  before rebuilding, are UB by contract. Do not enforce lifecycle validity with asserts,
  exceptions, automatic rebuilds, or runtime checks. Bounds/algebra preconditions are separate.
- Precomputed-result getters only read stored results. Do not run preprocessing,
  recalculate answers, or lazily rebuild from getters. Array/tree query aggregation
  keeps its normal algorithmic cost. Store BCC results during `build()`; cycle
  counters use `build(mod = 0)` and expose the stored answer through `ret() const`.
- Preserve input across builds; clear derived visitation, aggregate counts, heaps,
  matches, potentials, residual capacities and other work state before recalculation.
  Input and derived data sharing storage must be separated or reconstructible.
- Build returns the full result or status. Explicit incremental execution uses a
  separate name such as `augment`, whose result is the current call's increase.
- Online data structures keep immediate-query semantics after supported updates.
  HLD rebuilds preserve logical vertex values while changing DFS storage order.
- When Mo policy owns external/static work state, call its optional `reset()` at each
  build; policy authors must supply this hook when their state needs it.
- Test repeated build and modify-build-query sequences, including both index bases;
  never exercise UB query sequences as if they had a defined result. Keep snippets
  and README examples synchronized with API renames.

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


## Identity, initial leaves, and hashes
- `_seg`, `_prop`, `_pst`, `_fw`, and `_fw2d` require `policy::id()` for the
  aggregation identity. Do not infer it from `node()`.
- `_seg` and `_prop` also require `policy::leaf()` for each real initial leaf
  in `[base,n]`. Padding and query accumulators use `id()`, not `leaf()`.
  The default lazy policy has identity `{0,0}` and initial leaf `{0,1}`.
- Built-in monoid/group/action nodes and action lazy tags have no default
  constructors. Initialize through `policy::id()`, `policy::leaf()`, and
  `policy::lz_id()`; value-taking constructors remain available. For example,
  use `vector<policy::node>(n + 1, policy::leaf())` for an input array.
- `_prop` requires `static lazy lz_id()` for the no-op tag. Do not infer the
  lazy identity from `lazy()`. Custom action policies must provide this hook.
- Lazy tags must support `operator==`. `_prop` checks `tag == policy::lz_id()`
  directly; there is no `policy::is_lz_id()` hook. Use a canonical tag representation
  or custom equality when inactive fields should be ignored. Equality with the
  identity must never classify an effective update as a no-op.
- `make(arr)` uses the caller's nodes verbatim and retains the stored base.
  PST null roots and Fenwick reset state retain identity semantics; they do
  not materialize the dense-tree `leaf()` value.
- Default prefix/deque hashes use the same key pair `{911382323,972663749}`
  and modulus pair `{1000000007,1000000009}`. Custom settings remain supported.
  Only compare raw hashes produced with identical settings and equal lengths.
  `same` checks lengths, while `ret` returns only the hash value. Hash collisions
  remain possible. Keep both hash implementations and their snippets aligned.


## Policy snippets and bidirectional adapters
- Keep standalone monoid/action snippets synchronized with their definitions in
  Segtree.cpp and with the bundled seg/prop snippets. Do not change existing triggers.
- `bidirectional<P>` stores forward/backward aggregates; reverse combination is
  `P::op(right.backward, left.backward)`. `lift(value)` duplicates a leaf value;
  explicitly supply both values for direction-dependent leaves.
- `bidirectional_action<P>` reuses the same tag/compose in both directions.
  Override `apply` when the problem requires direction-dependent updates.
  Its standalone snippet depends on the common bidirectional snippet.
- Adapters provide aggregate storage only. Do not claim that HLD traversal order
  is supported until its path decomposition/query logic has been implemented.
