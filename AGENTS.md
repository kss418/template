# AGENTS.md (PS C++ templates repo)

## Goal
Maintain a clean, reliable competitive-programming template repository.
Prefer small, surgical diffs; avoid rewriting unrelated files.

## Snippets sync
- If you change any reusable template code, also update the corresponding editor snippets
  (e.g., VS Code user/project snippets) in the same change.
- Keep snippet triggers/prefixes and the expanded code consistent with the repository templates.
- Do not defer snippet updates; treat snippets as part of the template interface.

- README.md must contain only the algorithm/file-location guide and a brief
  snippets overview. Do not append API contracts, code examples, index/lifecycle
  rules, or adapter/HLD usage details, even inside the snippets section.
  Maintain those details in AGENTS.md or source comments.

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

## Style / conventions
- Use `class`, not `struct`, for algorithm, policy, adapter, node, and lazy type
  definitions when creating or modifying them. State access with explicit
  `public:`/`private:` sections and use explicit public inheritance where required.
  Preserve the public API when converting an existing struct.
- Add only time-complexity code comments by default. Add other code comments
  only when explicitly requested by the user.
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
- Bidirectional nodes provide `node reversed() const` by swapping cached directions.
  `_hld_path::query(u,v)` folds segments in u-to-v order and invokes this hook on upward
  segments. Nodes without the hook retain the commutative-aggregation contract.
- `_hld::it_path(u,v,f)` emits inclusive zero-based backend ranges `(l,r,rev)` in
  path order; `rev=1` means visit r down to l. Endpoints must be in one component.
  Vertex mode includes the LCA; edge mode stores each edge at its child and excludes
  the LCA/root dummy. Edge forward/backward values mean parent-to-child/child-to-parent.
  Vertex values should be direction-independent (use `lift`). Subtree queries keep
  backend DFS order rather than a path order.
- `_hld_path` updates apply the same tag in the backend's canonical orientation
  to each segment. For updates relative to u-to-v direction, use `it_path` and map
  tags using its rev flag before calling the public backend's update function.

## HLD decomposition and backends
- `_hld` is a non-template decomposition-only class. Constructors and clear use
  `(n,m=0,base=1)`, where m=0 selects vertices and m=1 selects child-stored edges.
  It owns no backend, values, aggregation policy, or query/update operations.
- After build, `index(v)` maps an external vertex to its zero-based DFS position;
  `vertex(i)` is its inverse. `parent(v)` uses -1 for roots and `depth(v)` starts
  at 0. Forest roots are the smallest active vertices, and directed input must
  be reachable from those roots. Input must be a forest.
- `subtree(v)` returns an inclusive `(l,r)` interval using the configured mode.
  Vertex mode includes v; edge mode excludes v's slot. An edge-mode leaf returns
  an empty interval with l>r. `it_path` retains endpoint order and the rev flag.
- `_hld_path<backend>` publicly extends `_hld`, retains its decomposition APIs,
  and owns public `seg` plus point/path/subtree queries and supported updates.
  Only the extension manages backend values and preserves each logical vertex's
  stored node across repeated build and supported input modifications. Edge values
  remain indexed by the logical child slot, as before; users own any reinterpretation
  needed when changing which vertex is the child of a particular edge.
- Use the extension's own clear/build; invoking base mutators through an upcast
  bypasses backend handling and is unsupported. Base-only build recomputes numbering;
  external backends attached manually to `_hld` must be remapped by their owner.
- `_hld_path::make(arr)` replaces stored nodes using external vertex indices:
  arr[v] belongs to vertex v, and base=1 skips arr[0]. It requires a fresh topology
  build, with UB before build or after modifying edges before rebuilding; add no
  lifecycle checks or automatic build. It preserves topology, n, mode, and base.
  Unlike array-only templates, HLD make does not resize the graph to match input:
  missing vertices get backend id(), excess input is ignored, and empty input
  resets values to identity. Supplied nodes are used verbatim, including root
  dummy slots in edge mode. Use the same remapping/loading logic during rebuild.
- Backend storage remains zero-based. Prefer clear(n,0), falling back to clear(n)
  for backends that inherently use zero-based indices. Rebuild uses make(arr) when
  available, otherwise clear plus point set; it always uses backend id() for padding.
  Lazy updates and direction reversal retain their existing capability requirements.
- `>>hld` contains only the decomposition class. `>>hld_path` contains the backend
  adapter and is_action concept and requires `>>hld` and the chosen backend snippet.

## LCA path aggregation
- `_lca` is a non-template topology-only class. Use `_lca tree(n,base)` for
  LCA/distance/ancestor/jump/component queries and parent/child traversal order.
  It takes unweighted `add(a,b)` and `addsol(a,b)` input and has no path query.
- `_lca_path<policy>` publicly extends `_lca` and retains all its read APIs.
  Use the extension's own `add`, `addsol`, `clear`, and `build`; mutating through
  a base-class reference bypasses retained weights and is unsupported.
  Do not add a `use_node` switch: the extension always builds aggregation tables.
- Path policies provide `cost`, `node`, `lift(cost)`, associative `merge(left,right)`,
  and `id()` for the two-sided identity; never infer identity from `node()`.
  The built-in node has no default constructor; its cost stores `ll v` with
  default value 0, and the default aggregation is maximum.
- `query(a,b)` folds edge values in a-to-b order, excluding the LCA vertex.
  It stores both traversal orders internally, so policies need no reversal hook
  or bidirectional adapter. Equal endpoints and disconnected endpoints return identity.
- `add(a,b,cost)` assigns the same value in both directions. The overload
  `add(a,b,ab,ba)` assigns a-to-b and b-to-a costs independently, regardless of root.
  `addsol` accepts the same cost forms but stores only the a-to-b adjacency;
  its reverse cost is still available for upward path queries. Directed forest
  input must be reachable from each component's smallest active vertex, which is
  selected as root during build. `ret`, `dist`, `ancestor`, and `jump` retain their APIs.
- Both aggregation orders are rebuilt from retained edges on every `build()`.
  Store upward/downward values together in one internal path type/table.
  Complexity is O(n log n) preprocessing/storage and O(log n) per path query,
  assuming constant-time policy values and operations.
- Keep `>>lca` topology-only. `>>lca_path` contains the default path policy and
  aggregation extension and requires expanding `>>lca` first.

## SCC and condensation DAG
- `_scc` computes component IDs/groups and exposes component/group queries plus
  `it_cid`, `it_all`, and `it_group`. Keep DAG state and operations out of this
  base class; 2-SAT continues to use `_scc` alone with unchanged SCC numbering.
- `_scc_dag` publicly extends `_scc`. Its build recomputes SCCs and then caches
  deduplicated condensation edges in both directions, indegrees/outdegrees, and
  Kahn traversal orders for both orientations. Exclude intra-component/self edges;
  parallel original edges produce only one edge per ordered component pair.
- External vertices follow `[base,n]` with default `bool base = 1`; component IDs
  and all DAG vector indices are always zero-based `[0,comp_cnt()-1]`.
  `dag()`, `ind()`, and `outd()` return const references to cached forward adjacency,
  indegree, and outdegree vectors in O(1). Their contents are replaced by build/clear.
- `it_dag(f)` traverses cached forward edges. `it_top<REV=true>(merge,done)` uses
  the cached Kahn order and selected adjacency, with done(cur) before its outgoing
  merge(cur,nxt) calls. REV=false follows forward edges; the default REV=true follows
  reversed edges. Preserve the previous FIFO Kahn tie order in both directions.
  Neither traversal reconstructs a graph or recomputes degrees/topological order;
  each takes O(k+e), where k is the component count and e the condensed edge count.
- After addsol, rebuild through the extension before querying any derived result.
  Base clear/build invoked through an upcast bypasses DAG caches and is unsupported.
  Queries before build or after modifying input before rebuilding remain UB;
  do not add lifecycle guards or automatic rebuilds.
- `>>scc` contains only the base class. `>>scc_dag` requires `>>scc`; `>>2sat`
  continues to require only `>>scc` and must not depend on the DAG extension.
