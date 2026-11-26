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

## uf_policy

`uf_policy`는 `_uf_value`의 기본 정책이다. 기본 `node`는 빈 타입이다. 집합에 값을 붙이려면 세 멤버를 정의한 정책을 넘긴다. [소스](../DSU.cpp#L61) · [VS Code 스니펫 `>>uf_value`](../snippets/uf_value.code-snippets)

| 멤버 | 계약 |
| --- | --- |
| `node` | 집합의 값 타입이다. 복사 생성과 복사 대입이 가능해야 한다. |
| `static node leaf()` | 원소 하나로 이루어진 집합의 기본값을 반환한다. |
| `static void merge(node& a, node& b)` | 대표를 유지하는 쪽이 `a`, 흡수되는 쪽이 `b`다. 합친 값을 `a`에 저장하며 `b`도 수정할 수 있다. |

집합 크기가 큰 쪽의 대표를 유지한다. 크기가 같으면 `_uf_value::merge()`의 첫 번째 인자가 속한 집합의 대표를 유지한다.

### 사용 예시

```cpp
class sum_policy{
public:
    class node{
    public:
        ll sum;
    };
    static node leaf(){ return {0}; }
    static void merge(node& a, node& b){ a.sum += b.sum; }
};
```

## uf_value

`_uf_value<policy>`는 집합마다 [`policy::node`](#uf_policy) 값을 하나씩 관리한다. 스니펫은 `>>uf` 다음에 `>>uf_value`를 넣는다. [소스](../DSU.cpp#L70) · [VS Code 스니펫 `>>uf_value`](../snippets/uf_value.code-snippets)

### `_uf_value` API

원소 범위는 `_uf`와 같은 `[base, n]`이다. `_uf`의 `find()`, `same()`, `size()`, `count()`, `root()`, `it_root()`, `groups()`도 사용할 수 있다. 집합과 값을 함께 갱신하려면 `_uf_value`의 `merge()`와 `clear()`를 호출한다. 공간은 `O(n + 1)`이고, 표의 시간은 `leaf()`와 `node` 복사가 `O(1)`일 때 기준이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_uf_value(int n = 0, bool base = 1)` | 각 원소를 별도 집합으로 만들고 `policy::leaf()`의 값으로 초기화한다. | `O(n + 1)` |
| `clear(int n, bool base = 1)` | 범위를 다시 지정하고 집합과 값을 초기화한다. | `O(n + 1)` |
| `make(span<const node> v)` | 현재 `base`로 집합을 다시 만들고 원소 `i`의 값을 `v[i]`로 초기화한다. `base = 1`이면 `v[0]`을 건너뛴다. 빈 입력은 빈 상태로 초기화한다. | `O(v.size() + 1)` |
| `merge(int a, int b)` | 서로 다른 집합을 합치고 정책으로 값을 병합한 뒤 `true`를 반환한다. 이미 같은 집합이면 `false`를 반환한다. | `O(α(n))` amortized + 정책 병합 비용 |
| `get(int x)` | `x`가 속한 집합의 값을 수정할 수 있는 `node&`로 반환한다. | `O(α(n))` amortized |
| `set(int x, const node& v)` | `x`가 속한 집합의 값 전체를 `v`로 교체한다. | `O(α(n))` amortized |
| `it_component(const F& f)` | 대표 번호의 오름차순으로 `f(int r, node& value)`를 호출한다. 콜백에서 집합 값을 수정할 수 있다. | `O(n)` + 콜백 비용 |

`merge()` 뒤에는 `get(x)`로 현재 집합 값의 참조를 다시 얻는다. `clear()`와 `make()`는 이전 참조를 무효화한다.

### 사용 예시

```cpp
_uf_value<sum_policy> uf(4);
const sum_policy::node values[]{{0}, {2}, {3}, {5}, {7}};
uf.make(values); // 1~4번 원소에 2, 3, 5, 7을 넣는다.

uf.merge(2, 3);
uf.merge(1, 2); // 크기가 큰 2번 집합이 대표를 유지한다.
assert(uf.find(1) == 2 && uf.get(1).sum == 10);

uf.set(3, {20}); // 1, 2, 3이 속한 집합의 값 전체를 바꾼다.
assert(uf.get(1).sum == 20);

vector<pair<int, ll>> components;
uf.it_component([&](int r, sum_policy::node& value){
    components.emplace_back(r, value.sum);
}); // {{2, 20}, {4, 7}}
```

## wuf

`_wuf`는 두 원소의 값 차이 조건을 추가하고, 그 조건들로 계산한 차이를 조회한다. `merge(a, b, c)`와 `diff(a, b)`의 방향은 모두 `val[b] - val[a]`다. [소스](../DSU.cpp#L107) · [VS Code 스니펫 `>>wuf`](../snippets/weight%20uf.code-snippets)

### `_wuf` API

`n`은 최대 원소 번호이며 원소 범위는 `[base, n]`이다. 내부에 저장되는 차이와 조회하는 차이는 `ll` 범위에 있어야 한다. 공간은 `O(n + 1)`이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_wuf(int n = 0, bool base = 1)` | 각 원소를 별도 집합으로 초기화한다. | `O(n + 1)` |
| `clear(int n, bool base = 1)` | 기존 차이 조건과 모순 상태를 지우고 새 범위로 초기화한다. | `O(n + 1)` |
| `find(int num)` | `{대표 번호, val[num] - val[대표]}`를 `pair<int, ll>`로 반환하고 경로를 압축한다. 차이의 유효성은 `diff()`로 확인한다. | `O(α(n))` amortized |
| `merge(int a, int b, ll c)` | `val[b] - val[a] = c` 조건을 추가한다. 두 원소가 다른 집합이면 합친다. 같은 집합에서 차이가 충돌하면 집합 전체를 모순으로 표시한다. 모순 상태는 합친 집합에도 이어진다. | `O(α(n))` amortized |
| `diff(int a, int b)` | 두 원소가 다른 집합이면 `disconnected`, 같은 집합에 모순이 있으면 `inconsistent`, 그 외에는 `known`과 `val[b] - val[a]`를 담아 반환한다. | `O(α(n))` amortized |
| `same(int a, int b)` | 같은 집합이면 모순 상태여도 `true`를 반환한다. | `O(α(n))` amortized |
| `size(int x)` | `x`가 속한 집합의 원소 수를 반환한다. | `O(α(n))` amortized |
| `count()` | 현재 집합 수를 반환한다. | `O(1)` |
| `root(int x)` | `x`가 현재 집합의 대표면 `true`를 반환한다. | `O(1)` |
| `it_root(const F& f)` | 대표 번호의 오름차순으로 `f(int r)`을 호출한다. | `O(n)` + 콜백 비용 |
| `groups()` | 집합을 대표 번호 순으로 반환한다. 각 집합의 원소도 오름차순이다. | `O(n α(n))` |

### `result` API

| 멤버 | 동작 | 시간 |
| --- | --- | --- |
| `status` | `state::disconnected`, `state::inconsistent`, `state::known` 중 하나다. | `O(1)` |
| `disconnected()` | 상태가 `disconnected`면 `true`를 반환한다. | `O(1)` |
| `inconsistent()` | 상태가 `inconsistent`면 `true`를 반환한다. | `O(1)` |
| `get_value()` | `ll` 멤버 `value`를 반환한다. `known`이면 계산한 차이이고, 그 외에는 `0`이다. | `O(1)` |

### 사용 예시

```cpp
_wuf uf(4);
uf.merge(1, 2, 5);  // val[2] - val[1] = 5
uf.merge(2, 3, -2); // val[3] - val[2] = -2

auto res = uf.diff(1, 3);
assert(res.status == _wuf::state::known && res.get_value() == 3);
assert(uf.diff(3, 1).get_value() == -3); // 순서를 뒤집으면 부호도 바뀐다.
assert(uf.diff(1, 4).disconnected());

uf.merge(1, 3, 4); // 이미 계산된 차이 3과 충돌
assert(uf.diff(2, 3).inconsistent());

uf.merge(3, 4, 1); // 4번이 모순 상태인 집합에 합쳐진다.
assert(uf.same(1, 4) && uf.diff(1, 4).inconsistent());
```

## ufset

`_ufset`은 정수 범위를 연속된 구간들로 관리한다. 처음에는 원소 하나가 구간 하나이며, `merge_prv()`와 `merge_nxt()`로 바로 이웃한 구간을 합친다. [소스](../DSU.cpp#L178) · [VS Code 스니펫 `>>ufset`](../snippets/ufset.code-snippets)

### `_ufset` API

`n`은 최대 원소 번호, `k`는 구간 수다. 원소 범위는 `[base, n]`이고, 그 밖의 경계값은 표에 명시된 호출에서 사용할 수 있다. 공간은 `O(n + 1)`이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_ufset(int n = 0, bool base = 1)` | 각 원소를 한 칸짜리 구간으로 초기화한다. | `O(n + 1)` |
| `clear(int n, bool base = 1)` | 병합 기록을 지우고 새 범위로 초기화한다. | `O(n + 1)` |
| `find(int num)` | `num`이 속한 구간의 대표를 반환한다. `base - 1`과 `n + 1`은 그대로 반환한다. | `O(α(n))` amortized |
| `merge_prv(int x)` | `x`의 구간과 왼쪽 이웃을 합쳐 `true`를 반환한다. 왼쪽 이웃이 없으면 `false`다. | `O(α(n))` amortized |
| `merge_nxt(int x)` | `x`의 구간과 오른쪽 이웃을 합쳐 `true`를 반환한다. 오른쪽 이웃이 없으면 `false`다. | `O(α(n))` amortized |
| `left(int x)` | `x`의 구간에서 가장 작은 원소를 반환한다. `base - 1`과 `n + 1`은 그대로 반환한다. | `O(α(n))` amortized |
| `right(int x)` | `x`의 구간에서 가장 큰 원소를 반환한다. `base - 1`과 `n + 1`은 그대로 반환한다. | `O(α(n))` amortized |
| `prv(int x)` | 왼쪽 이웃 구간의 대표를 반환한다. 이웃이 없으면 `base - 1`이다. `prv(n + 1)`로 맨 오른쪽 구간부터 순회할 수 있다. | `O(α(n))` amortized |
| `nxt(int x)` | 오른쪽 이웃 구간의 대표를 반환한다. 이웃이 없으면 `n + 1`이다. `nxt(base - 1)`로 맨 왼쪽 구간부터 순회할 수 있다. | `O(α(n))` amortized |
| `same(int a, int b)` | 두 원소가 같은 구간에 속하면 `true`를 반환한다. | `O(α(n))` amortized |
| `size(int x)` | `x`가 속한 구간의 원소 수를 반환한다. | `O(α(n))` amortized |
| `count()` | 현재 구간 수를 반환한다. | `O(1)` |
| `root(int x)` | `x`가 현재 구간의 대표면 `true`를 반환한다. | `O(1)` |
| `it_root(const F& f)` | 대표 번호의 오름차순으로 `f(int r)`을 호출한다. | `O(n)` + 콜백 비용 |
| `it_interval(const F& f)` | 왼쪽부터 구간마다 `f(int lo, int hi)`를 호출한다. 양 끝 모두 구간에 포함된다. 순회 중에는 구간 구성을 유지해야 한다. | `O(1 + k α(n))` amortized + 콜백 비용 |

### 사용 예시

```cpp
_ufset uf(5);
uf.merge_nxt(2); // [2, 3], 대표는 2
uf.merge_prv(5); // [4, 5], 대표는 5

assert(uf.left(3) == 2 && uf.right(3) == 3);
assert(uf.prv(5) == 2 && uf.nxt(3) == 5); // 이웃 구간의 대표를 조회
assert(!uf.merge_prv(1) && !uf.merge_nxt(5)); // 양 끝의 이웃은 없다.

uf.merge_prv(5); // 두 구간을 [2, 5]로 합친다.
assert(uf.same(2, 5) && uf.size(2) == 4);

vector<pair<int, int>> intervals;
uf.it_interval([&](int lo, int hi){
    intervals.emplace_back(lo, hi);
}); // {{1, 1}, {2, 5}}
```
