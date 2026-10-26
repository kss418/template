## 알고리즘 위치 안내

- `Automata.cpp`
  - Trie (`_trie`)
  - XOR Trie (`_xor_trie`)
  - Aho–Corasick (`_ac`)
- `Combinatorics.cpp`
  - Combination Table (`_comb`)
  - Lucas's Theorem (`_lucas`)
  - CRT-based Combination (`_comb_crt`)
- `DSU.cpp`
  - Union-Find (`_uf`)
  - Weighted Union-Find (`_wuf`)
  - Union-Find Set (`_ufset`)
- `FFT.cpp`
  - FFT (`_fft`)
- `Flow.cpp`
  - Dinic (`_flow`)
  - MCMF (`_mcmf`)
- `Geometry.cpp`
  - CCW (`_ccw`)
  - Angle Sort (`_angle_sort`)
  - Segment Intersection (`_li`)
  - Fraction (`_frac`)
  - Line Equation/Intersection (`_line`)
  - Point-in-Polygon (`_inter`)
  - Convex hull (`_ch`)
  - Rotating Calipers (`_rc`)
  - Polygon Area (`_area`)
  - Point/Vector Operations (`_dot`)
  - Modular Matrix Operations (`_mod_matrix`)
  - GF(2) Linear Algebra (`_gf2`)
  - XOR Basis (`_xor_basis`)
- `Graph.cpp`
  - LCA (`_lca`)
  - Virtual Tree (`_vtree`)
  - Binary Lifting Table (`_st`)
  - MST (`_mst`)
  - SCC (`_scc`)
  - 2-SAT (`_2sat`)
  - Bipartite Matching (`_bm`)
  - Hungarian Algorithm (`_hungarian`)
  - BCC (`_bcc`)
  - 3/4-cycle Search (`_3cycle`, `_4cycle`)
  - Compressed Edge List (`_clist`)
- `Hash.cpp`
  - Rolling Hash (Prefix) (`_hash`)
  - Deque Rolling Hash (`_deque_hash`)
- `Number_Theory.cpp`
  - Modint (`_mint`)
  - Sieve of Eratosthenes (`_es`)
  - Extended Euclidean (`_gcd`)
  - CRT (`_crt`)
  - Discrete Logarithm (`_dlog`)
  - Pollard Rho (`_pr`)
  - Linear Sieve/φ, μ (`_ls`)
  - Prime Tower (`_pt`)
- `Others.cpp`
  - Binary Search (`_bs`)
  - LCS (`_lcs`)
  - Mo’s Algorithm (`_mos`)
  - Ternary Search (`_ts`)
  - Convex Hull Trick (`_cht`)
  - Parallel Binary Search (`_pbs`)
- `Segtree.cpp`
  - Segment Tree (`_seg`)
  - Lazy Propagation (`_prop`)
  - HLD (`_hld`)
  - Merge Sort Tree (`_mt`)
  - Fenwick Tree (`_fw`)
  - 2D Fenwick Tree (`_fw2d`)
  - Persistent Segment Tree (`_pst`)
  - Bidirectional Adapters (`bidirectional`, `bidirectional_action`)
- `Shortest_Path.cpp`
  - Dijkstra (`_dij`)
  - Floyd–Warshall (`_floyd`)
  - SPFA (`_spfa`)
- `String.cpp`
  - KMP (`_kmp`)
  - Suffix Array + LCP (`_sa`)
  - Manacher (`_mnc`)
  - Z Algorithm (`_z`)

## snippets/

`snippets/`에는 VS Code용 code snippets이 들어있습니다. 각 알고리즘명에 대응하는
`.code-snippets` 파일이 존재하며 위 템플릿 파일들과 1:1로 매칭됩니다.

- `>>monoid`: 기본 합 monoid policy만 삽입합니다.
- `>>action`: 기본 구간 합·덧셈 action policy만 삽입합니다.
- `>>bidirectional`: 정·역방향 집계를 저장하는 공통 어댑터를 삽입합니다.
- `>>bidirectional_action`: lazy 어댑터를 삽입합니다. 먼저 `>>bidirectional`과 원본 action policy가 필요합니다.

`>>seg`와 `>>prop`는 각각 기본 monoid/action을 포함하므로 독립 policy 스니펫과 중복 선언하지 않습니다.
양방향 어댑터의 `lift(value)`는 양쪽 값을 같게 만들고, 방향별 값이 다르면 `{forward, backward}`를 직접 넣습니다.
일반 lazy 어댑터는 양쪽에 같은 태그를 적용하며, 방향별 갱신이 다르면 `apply`를 확장합니다.
HLD에 양방향 backend를 연결하면 `query(u,v).forward`는 u→v, `.backward`는 반대 경로를 집계합니다.
`it_path(u,v,f)`는 경로 순서대로 `(l,r,rev)`를 전달하며 l/r은 0-based 닫힌 구간입니다.
