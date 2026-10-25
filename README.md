# PS/CP C++ 템플릿

PS/CP(Problem Solving / Competitive Programming)용 C++ 템플릿 모음입니다.
대회에서 바로 사용할 수 있도록 알고리즘별 구현을 파일 단위로 정리해 두었습니다.

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


## 인덱스 base

위치·정점 범위를 받는 자료구조는 생성자와 초기화 함수에 `bool base = 1`을 받습니다.
`n`은 마지막 인덱스이며, 실제 범위는 `base=1`이면 `[1,n]`, `base=0`이면 `[0,n]`입니다.
기존 추가 인자가 있는 경우 `base`는 마지막에 옵니다.

```cpp
_seg<> seg(n);                 // 1..n
_seg<> seg0(n, 0);             // 0..n
_hld<_seg<>> hld(n, 0, 0);     // vertex mode, 0..n
_mcmf<> flow(n, true, 0);      // use_spfa=true, vertices 0..n
_bm matching(a, b, 0);         // left 0..a, right 0..b
_fw2d<> grid(n, m, 0);         // rows 0..n, columns 0..m
```

- `span`으로 build하는 배열은 **배열 첨자와 외부 인덱스가 일치**합니다. 기본 `base=1`이면 `arr[0]`은 건너뛰며 마지막 인덱스는 `arr.size()-1`입니다.
- 기본 `n=0, base=1`은 원소가 없는 상태입니다. `n=0, base=0`은 0번 원소 하나입니다.
- `_vtree::init()`은 선택한 base를 기본 루트로 사용합니다. 루트는 명시해서 바꿀 수도 있습니다.
- `_2sat`의 양수 리터럴은 변수 번호, 음수 리터럴은 `번호 + (n-base+1)`입니다. `pos/neg/op`를 사용하면 base에 맞게 변환됩니다.
- `_ufset`의 이전/다음 원소가 없으면 각각 `base-1`, `n+1` 경계 인덱스를 반환합니다.
- 행렬의 행·열 개수, 수론의 수치 상한, 해시 용량은 base 대상이 아닙니다.
- 이전에 기본 생성자로 0번을 사용하던 코드는 마지막 인자에 `0`을 추가해야 합니다.

검증: GNU C++20 컴파일러가 PATH에 있을 때 `python tests/run_base_index.py`.
