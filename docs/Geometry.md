# Geometry.cpp

## ccw

`_ccw`는 세 점 `a`, `b`, `c`의 방향을 판정한다. `ret()`은 `a → b`를 기준으로 `c`가 왼쪽이면 `1`(반시계), 오른쪽이면 `-1`(시계), 일직선이면 `0`을 반환한다. [소스](../Geometry.cpp#L14) · [VS Code 스니펫 `>>ccw`](../snippets/ccw.code-snippets)

### `_ccw` API

`pll`은 `pair<ll, ll>`이다. 좌표 차이는 `i128`로 계산하며, 외적을 계산하는 두 곱과 그 차이도 `i128` 범위에 있어야 한다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `cross(pll a, pll b, pll c)` | `(b - a) × (c - a)`를 `i128`로 반환한다. 절댓값은 삼각형 넓이의 두 배다. | `O(1)` |
| `cross(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3)` | 세 점의 좌표를 순서대로 받아 같은 외적을 구한다. | `O(1)` |
| `ret(pll a, pll b, pll c)` | 외적의 부호를 `int` 값 `1`, `-1`, `0`으로 반환한다. 둘 이상의 점이 같아도 `0`이다. | `O(1)` |
| `ret(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3)` | 세 점의 좌표를 순서대로 받아 방향을 판정한다. | `O(1)` |

### 사용 예시

```cpp
pll a{0, 0}, b{4, 0}, c{1, 3};
assert(_ccw::ret(a, b, c) == 1);  // 반시계
assert(_ccw::ret(a, c, b) == -1); // 점 순서를 뒤집으면 시계
assert(_ccw::cross(a, b, c) == 12); // 넓이의 두 배

assert(_ccw::ret(0, 0, 4, 0, 8, 0) == 0); // 선분 밖의 점도 같은 직선 위에 있으면 0
```

## angle_sort

`_angle_sort`는 기준점 `(cx, cy)`에서 오른쪽을 가리키는 방향부터 점을 각도순으로 정렬한다. 기본은 반시계 방향이며, `is_clock = true`이면 시계 방향이다. 기준점과 같은 점은 맨 앞에 오고, 같은 방향의 점은 가까운 순서로 놓인다. [소스](../Geometry.cpp#L37) · [VS Code 스니펫 `>>angle_sort`](../snippets/angle_sort.code-snippets)

### `_angle_sort` API

`point`는 좌표가 모두 정수인 타입을 요구한다. 템플릿의 `x`, `y` 매크로를 쓰면 좌표 멤버 이름은 `first`, `second`다. `pll`을 바로 쓸 수 있다. 좌표 차이와 외적·내적·거리 제곱 계산이 `i128` 범위에 있어야 한다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `cmp(ll cx = 0, ll cy = 0, bool is_clock = false)` | 두 점의 순서를 판단하는 비교 함수를 반환한다. `ranges::sort()` 등에 넘길 수 있다. | 생성·비교 각각 `O(1)` |
| `sort(R&& arr, ll cx = 0, ll cy = 0, bool is_clock = false)` | 점 `n`개의 순서를 제자리에서 바꾼다. 원소를 수정할 수 있는 연속 범위인 `vector`, 배열, `span` 등을 받는다. | `O(n log n)` |
| `is_same(const A& a, const B& b, ll cx = 0, ll cy = 0)` | 기준점에서 본 두 점이 같은 반직선 방향에 있으면 `true`다. 어느 한 점이 기준점과 같거나 두 점이 반대 방향이면 `false`다. | `O(1)` |

### 사용 예시

```cpp
vector<pll> points{{0, -1}, {2, 0}, {-1, 0}, {0, 1}, {1, 0}, {0, 0}};
_angle_sort::sort(points); // 원점 기준 반시계 방향
assert((points == vector<pll>{{0, 0}, {1, 0}, {2, 0}, {0, 1}, {-1, 0}, {0, -1}}));
assert(_angle_sort::is_same(points[1], points[2])); // 오른쪽 방향이 같다.
assert(!_angle_sort::is_same(points[0], points[1])); // 기준점에는 방향이 없다.

array<pll, 4> shifted{{{1, 4}, {0, 3}, {2, 3}, {1, 2}}};
auto cmp = _angle_sort::cmp(1, 3, true);
ranges::sort(shifted, cmp); // (1, 3) 기준으로 오른쪽, 아래쪽, 왼쪽, 위쪽
assert((shifted == array<pll, 4>{{{2, 3}, {1, 2}, {0, 3}, {1, 4}}}));
```

## li

`_li`는 정수 좌표의 점이 선분 위에 있는지, 두 선분이 교차하는지 판정한다. `pll`로 끝점을 넘기거나 `pair<pll, pll>`로 선분을 넘길 수 있다. 스니펫은 `>>ccw` 다음에 `>>li`를 넣는다. [소스](../Geometry.cpp#L79) · [VS Code 스니펫 `>>li`](../snippets/li.code-snippets)

### `_li` API

선분의 두 끝점은 어느 순서로 넣어도 된다. 끝점이 같으면 선분은 점 하나를 나타낸다. 좌표의 계산 범위는 `_ccw`와 같다. [CCW](#ccw)

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `ccw(const pll& v1, const pll& v2, const pll& v3)` | `_ccw::ret()`으로 세 점의 방향을 `1`, `-1`, `0`으로 반환한다. | `O(1)` |
| `on_segment(pll a, pll b, pll p)` | 점 `p`가 선분 `ab` 위에 있으면 `true`다. 끝점도 포함한다. | `O(1)` |
| `on_segment(pair<pll, pll> a, pll p)` | `a.first`와 `a.second`를 끝점으로 사용해 `p`의 포함 여부를 판정한다. | `O(1)` |
| `intersect(pll av1, pll av2, pll bv1, pll bv2)` | 두 선분에 공통점이 하나라도 있으면 `true`다. 끝점 접촉과 일직선 위의 겹침을 포함한다. | `O(1)` |
| `intersect(pair<pll, pll> a, pair<pll, pll> b)` | 두 선분을 끝점 쌍으로 받아 공통점이 있는지 판정한다. | `O(1)` |
| `proper_intersect(pll av1, pll av2, pll bv1, pll bv2)` | 서로 다른 직선 위의 두 선분이 양쪽 선분의 내부에서 교차하면 `true`다. | `O(1)` |
| `proper_intersect(pair<pll, pll> a, pair<pll, pll> b)` | 두 선분을 끝점 쌍으로 받아 같은 내부 교차 조건을 판정한다. | `O(1)` |

### 사용 예시

```cpp
using segment = pair<pll, pll>;
segment a{{0, 0}, {4, 0}};
segment crossing{{2, -1}, {2, 1}};
segment touching{{4, 0}, {5, 1}};
segment overlap{{2, 0}, {6, 0}};
segment point{{2, 0}, {2, 0}};

assert(_li::on_segment(a, {4, 0})); // 끝점도 포함
assert(_li::intersect(a, crossing) && _li::proper_intersect(a, crossing));
assert(_li::intersect(a, touching) && !_li::proper_intersect(a, touching));
assert(_li::intersect(a, overlap) && !_li::proper_intersect(a, overlap));
assert(_li::intersect(a, point) && !_li::proper_intersect(a, point));
```
