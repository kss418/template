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
