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

위치·정점 범위를 받는 자료구조는 생성자와 `clear`에 `bool base = 1`을 받습니다. 선택한 base는 객체 내부에 저장합니다.
`n`은 마지막 인덱스이며, 실제 범위는 `base=1`이면 `[1,n]`, `base=0`이면 `[0,n]`입니다.
기존 추가 인자가 있는 경우 `base`는 마지막에 옵니다. `make(arr)`는 base를 받지 않고 저장된 값을 유지합니다.

```cpp
_seg<> seg(n);                 // 1..n
_seg<> seg0(n, 0);             // 0..n
_hld<_seg<>> hld(n, 0, 0);     // vertex mode, 0..n
_mcmf<> flow(n, true, 0);      // use_spfa=true, vertices 0..n
_bm matching(a, b, 0);         // left 0..a, right 0..b
_fw2d<> grid(n, m, 0);         // rows 0..n, columns 0..m
```

- `span`으로 make하는 배열은 **배열 첨자와 외부 인덱스가 일치**합니다. 기본 `base=1`이면 `arr[0]`은 건너뛰며 마지막 인덱스는 `arr.size()-1`입니다.
- 기본 `n=0, base=1`은 원소가 없는 상태입니다. `n=0, base=0`은 0번 원소 하나입니다.
- `_vtree::build()`은 선택한 base를 기본 루트로 사용합니다. 루트는 명시해서 바꿀 수도 있습니다.
- `_2sat`의 양수 리터럴은 변수 번호, 음수 리터럴은 `번호 + (n-base+1)`입니다. `pos/neg/op`를 사용하면 base에 맞게 변환됩니다.
- `_ufset`의 이전/다음 원소가 없으면 각각 `base-1`, `n+1` 경계 인덱스를 반환합니다.
- 행렬의 행·열 개수, 수론의 수치 상한, 해시 용량은 base 대상이 아닙니다.
- 이전에 기본 생성자로 0번을 사용하던 코드는 마지막 인자에 `0`을 추가해야 합니다.

검증: GNU C++20 컴파일러가 PATH에 있을 때 `python tests/run_base_index.py`.


## build / make 계약

- `build(...)`는 보존한 현재 입력으로 계산 상태와 결과를 처음부터 다시 계산합니다. 반복 호출은 이전 실행의 증가분이 아닌 전체 결과를 만듭니다.
- 배열·문자열 등 전체 입력을 교체하고 준비하는 함수는 `make(...)`입니다. seg/lazy/merge-sort-tree/Fenwick은 `make(arr)`를 사용하고, 문자열 클래스도 `make(...)`를 사용합니다. `span`은 외부 인덱스에 맞추며 base는 생성자나 `clear`에서 선택한 값을 유지합니다.
- 기존 `init(...)` 및 2-SAT의 `solve()`는 `build(...)`로 변경했습니다. 매칭은 `build()`가 전체 최대 매칭 크기를 반환하고 `ret()`은 계산된 크기를 조회합니다.
- 전처리가 필요한 구조의 결과를 최초 `build` 전이나 입력 수정 후 재빌드 전에 조회하는 것은 **UB**입니다. 이를 위한 `built` assert나 자동 재계산은 제공하지 않습니다. 인덱스·수학적 전제조건에 대한 기존 assert는 별개입니다.
- seg/DSU/PST 등 즉시 갱신형 자료구조의 점 갱신 후 조회는 계속 유효합니다. `make`는 입력 교체와 필요한 준비를 함께 수행하므로 호출 직후 조회할 수 있습니다.
- `_vtree::build(root)`는 원본 트리를 전처리하고, `make(nodes)`는 선택 정점의 가상 트리를 구성합니다.
- HLD는 원본 트리를 유지하고 재빌드 시 정점 값을 새 DFS 순서로 옮깁니다. backend는 `make(span)` 또는 점 `set(index, node)`를 제공해야 합니다. HLD가 backend를 0-based로 초기화하며, `make`는 그 설정을 유지해야 합니다.
- 최대 유량과 MCMF의 `build(s,t,limit)`는 원래 용량에서 다시 계산합니다. 이전 잔여 그래프에서 이어서 보내려면 `augment(s,t,limit)`를 사용합니다. `augment`의 반환값은 이번 호출의 증가분입니다.
- Mo는 매 build에서 현재 구간과 답 상태를 초기화합니다. policy가 외부/static 작업 상태를 갖는다면 `static void reset()`을 제공하여 매 build 시작에 초기화해야 합니다.

```cpp
_seg<> seg(0, 0);           // base는 객체에서 설정
seg.make(arr);             // 저장된 base=0 유지

_dij<> dij(n);
dij.addsol(1, n, 5);
dij.build(1);
dij.addsol(1, n, 2);
dij.build(1);               // 수정한 입력으로 다시 계산

_flow<> flow(n);
flow.addsol(1, n, 5);
flow.build(1, n);           // 5
flow.build(1, n);           // 다시 5
flow.augment(1, n);         // 이미 포화되어 추가 유량 0
```

전체 회귀 검사: `python tests/run_tests.py` (GNU C++20).


## BCC / cycle 계산과 조회

BCC는 `build()` 한 번으로 단절점, 단절선, BCC를 모두 계산합니다.
`point()`, `line()`, `ret()`은 저장한 목록의 `const&`를 O(1)에 반환하며 재계산하지 않습니다.
반환한 결과 내용은 다음 `build()`/`clear()`에서 바뀝니다. 이전 결과를 보관하려면 복사합니다.

```cpp
_bcc g(n);
g.add(1, 2);
g.build();
const auto& points = g.point();
const auto& bridges = g.line();
const auto& blocks = g.ret();
```

`_3cycle`, `_4cycle`은 `build(mod = 0)`에서 개수를 계산하고 반환합니다.
`ret() const`는 저장한 개수를 O(1)에 반환합니다. 기존 `ret(mod)`는 `build(mod)`로 옮겼습니다.
`mod=0`은 나머지를 취하지 않으며 결과가 `ll` 범위에 들어와야 합니다.

```cpp
_3cycle cycles(n);
cycles.add(1, 2); cycles.add(2, 3); cycles.add(3, 1);
cycles.build(1'000'000'007);
cout << cycles.ret();
```

입력 수정 후에는 다시 `build()`해야 합니다. 그 전의 조회는 계약상 UB이며 검사하지 않습니다.
같은 입력으로 `build()`를 다시 호출하면 처음부터 재계산합니다.
