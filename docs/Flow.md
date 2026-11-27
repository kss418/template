# Flow.cpp

## flow

`_flow<T = ll>`은 Dinic으로 최대 유량을 구한다. 용량과 유량 타입 `T`는 부호 있는 정수 타입이다. 기본 용량과 유량 한도는 `make_inf()`가 만든 `_flow<T>::INF`이며, 각 바이트가 `0x3f`인 값이다. [소스](../Flow.cpp#L14) · [VS Code 스니펫 `>>flow`](../snippets/flow.code-snippets)

### `_flow` API

`n`은 최대 정점 번호이고 정점 범위는 `[base, n]`이다. `V`는 정점 수, `E`는 추가한 방향 간선 수다. 공간은 `O(1 + V + E)`다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_flow(int n = 0, bool base = 1)` | 간선이 없는 그래프를 생성한다. | `O(1 + V)` |
| `clear(int n, bool base = 1)` | 기존 간선을 지우고 새 정점 범위로 초기화한다. | 기존 그래프와 새 정점 수에 선형 |
| `reset()` | 간선과 용량을 유지하고 모든 유량과 저장된 결과를 `0`으로 돌린다. | `O(V + E)` |
| `addsol(int st, int en, T c = INF)` | 용량 `c >= 0`인 `st → en` 간선을 추가하고 `state`를 반환한다. | `O(1)` amortized |
| `add(int st, int en, T c = INF)` | 양방향에 용량 `c`인 간선을 하나씩 추가하고 `{정방향 state, 반대 방향 state}`를 반환한다. 각 방향은 별도의 간선이다. | `O(1)` amortized |
| `build(int st, int en, T f = INF)` | 유량을 초기화한 뒤 `st`에서 `en`으로 최대 `f`만큼 흘린다. 반환 타입은 `void`이며 `st != en`, `f >= 0`이어야 한다. | `O(1 + V + V²E)` |
| `ret()` | 마지막 `build()`가 구한 유량을 `T`로 반환한다. 간선이나 용량을 바꾸면 다시 빌드한 뒤 조회한다. | `O(1)` |
| `min_cut(int st)` | 현재 잔여망에서 `st`로부터 도달 가능한 정점을 `vector<bool>`로 반환한다. `build(st, en)` 후 `en`에 도달할 수 없으면 `true`인 쪽이 최소 컷의 소스 쪽이다. | `O(V + E)` |
| `reachable(int st, int en)` | 현재 잔여망에서 `st`가 `en`에 도달할 수 있는지 확인한다. | `O(V + E)` |
| `it_idx(int idx, const F& f)` | `idx`에서 나가는 추가 간선마다 삽입 순서대로 `f(state h)`를 호출한다. | `O(1 + outdegree(idx))` + 콜백 비용 |
| `it_all(const F& f)` | 정점 번호 순으로 각 정점의 추가 간선을 순회한다. | `O(V + E)` + 콜백 비용 |
| `get_edge(const state& h)` | `h`가 가리키는 간선을 `const edge&`로 반환한다. | `O(1)` |
| `get_rev(const state& h)` | `h`에 대응하는 잔여망의 역간선을 `const edge&`로 반환한다. | `O(1)` |

순회는 사용자가 추가한 간선을 대상으로 하며 용량 `0`인 간선도 포함한다. 순회 중에는 그래프의 간선 구성을 유지한다. `edge`의 `nxt`는 도착 정점, `rev`는 역간선의 인덱스, `c`는 잔여 용량, `cap`은 설정 용량이다. `get_edge()`와 `get_rev()`의 참조는 간선 추가나 `clear()`로 무효화될 수 있다.

### `state` API

`state`는 간선 핸들이다. 간선 추가, `reset()`, `build()` 후에도 같은 간선을 가리킨다. `clear()` 후에는 새 핸들을 얻는다. 핸들은 생성한 그래프에서 사용하고, 원본 그래프가 살아 있고 이동되지 않은 동안 유효하다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `from()` | 출발 정점을 반환한다. | `O(1)` |
| `to()` | 도착 정점을 반환한다. | `O(1)` |
| `h == other` | 같은 그래프의 같은 간선을 가리키면 `true`다. | `O(1)` |
| `get_flow()` | 현재 유량인 `cap - c`를 반환한다. | `O(1)` |
| `set_flow(T f)` | 유량을 `f`로 바꾸고 양쪽 잔여 용량을 갱신한다. `0 <= f <= get_cap()`이어야 한다. | `O(1)` |
| `add_flow(T delta)` | 현재 유량에 `delta`를 더한다. 변경 후 유량도 용량 범위 안에 있어야 한다. | `O(1)` |
| `get_cap()` | 설정 용량을 반환한다. | `O(1)` |
| `set_cap(T cap)` | 현재 유량을 유지하면서 용량을 바꾼다. `cap >= get_flow()`여야 한다. | `O(1)` |
| `add_cap(T delta)` | 현재 용량에 `delta`를 더한다. 변경 후 용량도 현재 유량 이상이어야 한다. | `O(1)` |

유량을 직접 바꿔 잔여망을 계속 사용하려면 중간 정점의 유량 보존도 맞춰야 한다. 용량 변경을 반영한 최대 유량은 `build()`로 다시 계산한다.

### 사용 예시

```cpp
_flow<int> g(4);
auto a = g.addsol(1, 2, 3);
g.addsol(1, 3, 2);
auto b = g.addsol(2, 4, 2);
g.addsol(3, 4, 2);

g.build(1, 4);
assert(g.ret() == 4 && a.get_flow() == 2);

auto side = g.min_cut(1); // 소스 쪽 정점은 1, 2
assert(side[1] && side[2] && !side[3] && !side[4]);
int cut_cap = 0;
g.it_all([&](auto h){
    if(side[h.from()] && !side[h.to()]) cut_cap += h.get_cap();
});
assert(cut_cap == g.ret());

b.add_cap(1);
g.build(1, 4); // 변경된 용량으로 처음부터 계산
assert(g.ret() == 5 && b.get_flow() == 3);

g.build(1, 4, 1); // 유량을 1까지만 계산
assert(g.ret() == 1 && g.reachable(1, 4));
```

## mcmf

`_mcmf<F = ll, C = ll>`은 한도 안에서 최대 유량을 흘리고, 그 유량을 보내는 최소 비용을 구한다. `F`는 용량·유량, `C`는 단위 비용·총비용의 부호 있는 정수 타입이다. `make_inf<T>()`는 각 바이트가 `0x3f`인 값을 만든다. `FINF`는 기본 유량 한도, `CINF`는 거리의 무한대다. [소스](../Flow.cpp#L166) · [VS Code 스니펫 `>>mcmf`](../snippets/mcmf.code-snippets)

### `_mcmf` API

`n`은 최대 정점 번호이고 정점 범위는 `[base, n]`이다. `V`는 정점 수, `E`는 추가한 방향 간선 수, `K`는 실제로 흘린 유량이다. 공간은 `O(1 + V + E)`다.

초기 그래프에서 양의 용량을 가진 간선에 음수 비용이 있으면 `use_spfa = true`를 사용한다. 이 간선들로 이루어진 그래프에 음수 비용 사이클이 없어야 한다. 유량과 비용 계산은 각 타입의 범위에 들어야 하며, 최단거리 탐색에 쓰는 유효한 거리는 `CINF`보다 작아야 한다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_mcmf(int n = 0, bool use_spfa = true, bool base = 1)` | 간선이 없는 그래프를 생성한다. `use_spfa`는 빌드 시작 시 SPFA로 포텐셜을 초기화할지 정한다. | `O(1 + V)` |
| `clear(int n, bool use_spfa = true, bool base = 1)` | 간선을 지우고 정점 범위와 SPFA 설정을 초기화한다. | 기존 그래프와 새 정점 수에 선형 |
| `reset()` | 간선·용량·비용을 유지하고 유량과 저장된 결과를 `0`으로 돌린다. | `O(1 + V + E)` |
| `addsol(int st, int en, F cap, C cost)` | `st → en` 간선의 용량과 단위 비용을 지정하고 `state`를 반환한다. `cap >= 0`이어야 한다. | `O(1)` amortized |
| `add(int st, int en, F cap, C cost)` | 두 방향에 같은 용량과 단위 비용의 간선을 추가하고 `{정방향 state, 반대 방향 state}`를 반환한다. | `O(1)` amortized |
| `build(int st, int en, F limit = FINF)` | 유량을 초기화한 뒤 `st`에서 `en`으로 최대 `limit`만큼 흘리며 최소 비용을 계산한다. 반환 타입은 `void`이며 `st != en`, `limit >= 0`이어야 한다. | `O((K + 1)(V + E log(E + 1)))` + 초기 SPFA `O(VE)` |
| `ret()` | 마지막 `build()`의 `{유량, 총비용}`을 `pair<F, C>`로 반환한다. 그래프의 입력을 바꾸면 다시 빌드한 뒤 조회한다. | `O(1)` |
| `it_idx(int idx, const T& f)` | `idx`에서 나가는 추가 간선마다 삽입 순서대로 `f(state h)`를 호출한다. | `O(1 + outdegree(idx))` + 콜백 비용 |
| `it_all(const T& f)` | 정점 번호 순으로 각 정점의 추가 간선을 순회한다. | `O(1 + V + E)` + 콜백 비용 |
| `get_edge(const state& h)` | `h`가 가리키는 간선을 `const edge&`로 반환한다. | `O(1)` |
| `get_rev(const state& h)` | `h`에 대응하는 잔여망의 역간선을 `const edge&`로 반환한다. 역간선의 단위 비용은 `-h.get_cost()`다. | `O(1)` |

순회에는 용량 `0`인 추가 간선도 포함된다. 순회 중에는 간선 구성을 유지한다. `edge`의 `nxt`, `rev`, `c`, `cap`은 `_flow`와 같은 의미이고, `cost`는 해당 방향의 단위 비용이다. 간선을 추가하거나 `clear()`를 호출하면 `edge` 참조가 무효화될 수 있다.

### `state` API

핸들은 간선 추가, `reset()`, `build()` 후에도 같은 간선을 가리킨다. 원래 그래프가 살아 있고 이동되거나 교체되지 않은 동안 사용한다. `clear()` 후에는 새 핸들을 얻는다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `from()` | 출발 정점을 반환한다. | `O(1)` |
| `to()` | 도착 정점을 반환한다. | `O(1)` |
| `h == other` | 같은 그래프의 같은 간선을 가리키면 `true`다. | `O(1)` |
| `get_flow()` | 현재 유량 `cap - c`를 반환한다. | `O(1)` |
| `set_flow(F f)` | 유량을 `f`로 바꾸고 양쪽의 잔여 용량을 갱신한다. `0 <= f <= get_cap()`이어야 한다. | `O(1)` |
| `add_flow(F delta)` | 현재 유량에 `delta`를 더한다. 변경 후 유량도 용량 범위 안에 있어야 한다. | `O(1)` |
| `get_cap()` | 설정 용량을 반환한다. | `O(1)` |
| `set_cap(F cap)` | 유량을 유지하면서 용량을 바꾼다. `cap >= get_flow()`여야 한다. | `O(1)` |
| `add_cap(F delta)` | 현재 용량에 `delta`를 더한다. 변경 후 용량도 현재 유량 이상이어야 한다. | `O(1)` |
| `get_cost()` | 유량 한 단위당 비용을 반환한다. | `O(1)` |
| `set_cost(C cost)` | 단위 비용을 `cost`로 바꾸고 역간선 비용을 `-cost`로 갱신한다. | `O(1)` |
| `add_cost(C delta)` | 현재 단위 비용에 `delta`를 더하고 역간선 비용도 갱신한다. | `O(1)` |

유량을 직접 바꿔 잔여망을 사용할 때는 중간 정점의 유량 보존을 맞춘다. 비용이나 용량을 바꾼 결과는 `build()`로 다시 계산한다.

### 사용 예시

```cpp
_mcmf<int, ll> g(4); // 음수 비용이 있으므로 기본값 use_spfa = true 사용
auto cheap = g.addsol(1, 2, 2, -3);
g.addsol(2, 4, 2, 1); // 이 경로의 단위 비용은 -2
g.addsol(1, 3, 2, 4);
g.addsol(3, 4, 2, 1); // 이 경로의 단위 비용은 5

g.build(1, 4, 3); // 저렴한 경로로 2, 나머지 경로로 1
auto [flow, cost] = g.ret();
assert(flow == 3 && cost == 1 && cheap.get_flow() == 2);

g.build(1, 4); // 유량을 초기화한 뒤 다시 계산
assert((g.ret() == pair<int, ll>{4, 6}));

cheap.set_cost(-1);
assert(g.get_rev(cheap).cost == 1);
g.build(1, 4);
assert((g.ret() == pair<int, ll>{4, 10}));
```
