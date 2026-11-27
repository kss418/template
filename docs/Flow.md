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
