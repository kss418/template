# DSU.cpp

## uf

`_uf`는 두 집합을 합치고 원소가 같은 집합에 속하는지 확인한다. 경로 압축과 크기 기준 합치기를 사용한다. [소스](../DSU.cpp#L13) · [VS Code 스니펫 `>>uf`](../snippets/uf.code-snippets)

### `_uf` API

`n`은 최대 원소 번호다. 모든 원소 인자는 `[base, n]`에 있어야 한다. 기본은 `1`번부터 시작하며, `base = 0`이면 `0`번도 포함한다. 공간은 `O(n + 1)`이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_uf(int n = 0, bool base = 1)` | 각 원소가 별도 집합인 상태로 생성한다. `_uf()`는 빈 상태다. | `O(n + 1)` |
| `clear(int n, bool base = 1)` | 기존 합치기를 모두 지우고 새 범위의 각 원소를 별도 집합으로 초기화한다. | `O(n + 1)` |
| `find(int x)` | `x`가 속한 집합의 대표 번호를 반환하고 경로를 압축한다. | `O(α(n))` amortized |
| `merge(int a, int b)` | 두 집합을 합쳤으면 `true`, 이미 같은 집합이면 `false`를 반환한다. 대표는 합치기 후 바뀔 수 있다. | `O(α(n))` amortized |
| `same(int a, int b)` | 두 원소가 같은 집합에 속하면 `true`를 반환한다. | `O(α(n))` amortized |
| `size(int x)` | `x`가 속한 집합의 원소 수를 반환한다. | `O(α(n))` amortized |
| `count()` | 현재 집합 수를 반환한다. | `O(1)` |
| `root(int x)` | `x` 자체가 현재 집합의 대표면 `true`를 반환한다. | `O(1)` |
| `it_root(const F& f)` | 대표 번호의 오름차순으로 `f(int r)`을 호출한다. | `O(n)` + 콜백 비용 |
| `groups()` | 모든 집합을 `vector<vector<int>>`로 반환한다. 집합은 대표 번호 순이고, 집합 안의 원소도 오름차순이다. | `O(n α(n))` |

### 사용 예시

```cpp
_uf uf(4); // 1~4번이 각자 별도 집합
bool joined = uf.merge(1, 2);
assert(joined);
uf.merge(2, 3);
assert(uf.same(1, 3));
assert(uf.size(2) == 3 && uf.count() == 2);
assert(uf.root(uf.find(3))); // 3이 속한 집합의 대표를 확인

vector<int> roots;
uf.it_root([&](int r){ roots.push_back(r); }); // {1, 4}
auto groups = uf.groups();                   // {{1, 2, 3}, {4}}

uf.clear(2, 0); // 0~2번으로 초기화
assert(uf.count() == 3 && uf.size(0) == 1);
```
