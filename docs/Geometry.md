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

<a name="li"></a>

## linear_intersection

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

## frac

`_frac<T = i128>`은 분모가 양수인 기약분수를 저장한다. `T`는 부호 있는 정수 타입이고, `0`은 항상 `0/1`로 저장한다. [소스](../Geometry.cpp#L113) · [VS Code 스니펫 `>>frac`](../snippets/frac.code-snippets)

### `_frac` API

정규화와 연산 중의 부호 변경, 절댓값, 곱셈 등 모든 중간값이 `T` 범위에 있어야 한다. 표에서 `M`은 해당 연산의 `gcd`에 넘기는 정수의 최대 크기다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_frac(T p = 0, T q = 1)` | `p/q`를 정규화해 생성한다. `q != 0`이어야 한다. | `O(1 + log(M + 1))` |
| `set(T p, T q)` | 값을 `p/q`로 바꾸고 정규화한다. `q != 0`이어야 한다. | `O(1 + log(M + 1))` |
| `num()` | 정규화된 분자를 `T`로 반환한다. | `O(1)` |
| `den()` | 정규화된 분모를 `T`로 반환한다. | `O(1)` |
| `to_ld()` | 분수의 근삿값을 `long double`로 반환한다. | `O(1)` |
| `floor()` | 분수 이하의 가장 큰 정수를 `T`로 반환한다. | `O(1)` |
| `ceil()` | 분수 이상의 가장 작은 정수를 `T`로 반환한다. | `O(1)` |
| `-a` | `a`의 부호를 바꾼 분수를 반환한다. | `O(1 + log(M + 1))` |
| `inv()` | 역수를 반환한다. 값이 `0`이면 호출할 수 없다. | `O(1 + log(M + 1))` |
| `pow(u64 k)` | 이진 거듭제곱으로 `k`제곱을 반환한다. `k == 0`이면 `1`이다. | `O(1 + log(k + 1) · log(M + 1))` |

### 산술과 비교

같은 `_frac<T>`끼리 연산한다. 정수는 분모가 `1`인 분수로 사용할 수 있다. 덧셈·뺄셈에서는 분모의 최대공약수를 활용하고, 곱셈·나눗셈에서는 약분할 수 있는 항을 먼저 줄인다.

| 연산 | 동작 | 시간 |
| --- | --- | --- |
| `a + b` | 합을 반환한다. | `O(1 + log(M + 1))` |
| `a - b` | 차를 반환한다. | `O(1 + log(M + 1))` |
| `a * b` | 곱을 반환한다. | `O(1 + log(M + 1))` |
| `a / b` | 몫을 분수로 반환한다. `b`는 `0` 이외의 값이어야 한다. | `O(1 + log(M + 1))` |
| `a += b` | `a`를 합으로 바꾸고 `a`의 참조를 반환한다. | `O(1 + log(M + 1))` |
| `a -= b` | `a`를 차로 바꾸고 `a`의 참조를 반환한다. | `O(1 + log(M + 1))` |
| `a *= b` | `a`를 곱으로 바꾸고 `a`의 참조를 반환한다. | `O(1 + log(M + 1))` |
| `a /= b` | `a`를 몫으로 바꾸고 `a`의 참조를 반환한다. `b`는 `0` 이외의 값이어야 한다. | `O(1 + log(M + 1))` |
| `a == b` | 정규화된 분수 또는 정수와 값이 같은지 확인한다. | `O(1)` |
| `a <=> b` | 분자와 상대의 분모를 교차 곱해 대소를 비교한다. 두 곱이 `T` 범위에 있어야 한다. | `O(1)` |

### 사용 예시

```cpp
_frac<> a(6, -8); // -3/4로 정규화
assert(a.num() == -3 && a.den() == 4);
assert(a.floor() == -1 && a.ceil() == 0);
assert(a < 0);

_frac<ll> b(2, 3), c(5, 6);
assert(b + c == _frac<ll>(3, 2));
assert(b * c == _frac<ll>(5, 9));
assert(c.inv() == _frac<ll>(6, 5));
assert(b.pow(3) == _frac<ll>(8, 27));

a.set(0, -5);
assert(a == 0 && a.den() == 1); // 0/1
```

<a name="ch"></a>

## Convex Hull

`_ch`는 정수 좌표 점 집합의 볼록 껍질을 구한다. 스니펫은 `>>ccw` 다음에 `>>ch`를 넣는다. [소스](../Geometry.cpp#L191) · [VS Code 스니펫 `>>ch`](../snippets/ch.code-snippets)

### `_ch` API

꼭짓점은 `(x, y)`의 사전순 최솟값부터 반시계 순서로 저장하며, 시작점을 끝에 반복하지 않는다. 변 위의 중간 점은 제외하고, 중복 좌표에는 가장 먼저 넣은 점의 ID를 남긴다. 빈 입력이면 결과도 비어 있고, 서로 다른 좌표가 하나면 그 점 하나, 서로 다른 좌표가 모두 일직선 위에 있으면 양 끝점만 남긴다.

공개 타입 `_ch::node`는 `ll x, y, idx`를 담는다. `node(pll a, ll idx)` 또는 `node(ll x, ll y, ll idx)`로 만들고, `pll`로 암시적 변환하면 좌표를 얻는다. 각 동작은 `O(1)`이다.

`ret()`과 `idx()`는 첫 조회 전과 입력이 바뀔 때마다 `build()`한 뒤에 호출한다. 외적과 거리 제곱 계산의 중간값은 `i128` 범위에 있어야 한다. 표에서 `n`은 보관 중인 점 개수, `m`은 새 입력의 점 개수다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_ch()` | 빈 점 집합을 생성한다. | `O(1)` |
| `clear()` | 입력과 결과를 비우고 다음 ID를 `0`으로 되돌린다. | `O(n)` |
| `make(span<const pll> v)` | 입력 전체를 복사해 교체하고 ID를 `v`의 0 기반 위치로 부여한다. 볼록 껍질을 얻으려면 이어서 `build()`를 호출한다. `ret()`의 전체나 일부도 입력으로 넘길 수 있다. | `O(n + m)` |
| `add(pll a)` | 점을 추가하고 그 점의 0 기반 ID를 `ll`로 반환한다. 중복 좌표를 추가해도 새 ID를 부여한다. | `O(1)` amortized |
| `add(ll a, ll b)` | 두 좌표를 따로 받아 같은 방식으로 점을 추가한다. | `O(1)` amortized |
| `build()` | 저장된 입력을 보존하고 볼록 껍질을 다시 계산한다. | `O(n log n)` |
| `ret()` | 꼭짓점 좌표를 `const vector<pll>&`로 반환한다. | `O(1)` |
| `idx()` | 같은 순서로 좌표와 입력 ID를 담은 꼭짓점을 `const vector<node>&`로 반환한다. | `O(1)` |
| `ccw(pll v1, pll v2, pll v3)` | 세 점의 방향을 `ll` 값 `1`, `-1`, `0`으로 반환한다. | `O(1)` |
| `dist2(pll v1, pll v2)` | 두 점의 유클리드 거리 제곱을 `i128`로 반환한다. | `O(1)` |

### 사용 예시

```cpp
_ch hull;
vector<pll> points{{2, 0}, {0, 0}, {4, 0}, {4, 3}, {0, 3}, {0, 0}};
hull.make(points);
hull.build();
assert((hull.ret() == vector<pll>{{0, 0}, {4, 0}, {4, 3}, {0, 3}}));
assert(hull.idx()[0].idx == 1); // (0, 0)의 최초 ID

ll id = hull.add(5, 0);
hull.build(); // 추가한 점을 반영
assert(id == 6 && hull.idx()[1].idx == id);
```

## mod_matrix

`_mod_matrix<mod = 998244353>`은 고정 모듈러로 행렬의 덧셈, 뺄셈, 곱셈과 거듭제곱을 계산한다. `mod`는 양의 컴파일 타임 `u64` 값이며 합성수와 `1`도 쓸 수 있다. 원소 타입 `mint`는 `_mint<mod>`의 별칭이다. 스니펫은 `>>mint` 다음에 `>>mod_matrix`를 넣는다. [소스](../Geometry.cpp#L255) · [VS Code 스니펫 `>>mod_matrix`](../snippets/mod_matrix.code-snippets) · [VS Code 스니펫 `>>mint`](../snippets/mint.code-snippets)

### `_mod_matrix` API

`n`과 `m`은 행과 열의 개수다. 차원과 인덱스는 `int`이며, 차원은 각각 `0` 이상이어야 한다. 원소에 접근할 때는 `0 <= i < n`, `0 <= j < m`이어야 하며 인덱스는 검사하지 않는다. 행렬은 `O(1 + nm)` 공간을 쓰며 생성 직후부터 조회하고 수정할 수 있다.

`at(i, j)`는 원소 자체의 참조를 반환한다. 부호 있는 정수는 `a.at(i, j) = -1`처럼 대입하거나 `a.at(i, j) += -1`처럼 더하면 모듈러로 정규화된다. 저장된 나머지는 `a.at(i, j).v`로 읽을 수 있다. `set`, `add`, `+=`, `-=`는 기존 원소 참조를 유지한다. `clear`와 `*=`를 호출한 뒤에는 원소 참조를 다시 얻어야 한다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_mod_matrix()` | `0 × 0` 행렬을 생성한다. | `O(1)` |
| `_mod_matrix(int n, int m)` | 모든 원소가 `0`인 `n × m` 행렬을 생성한다. | `O(1 + nm)` |
| `clear(int n, int m)` | 크기를 `n × m`으로 정하고 모든 원소를 `0`으로 채운다. 크기가 같아도 초기화한다. | `O(1 + nm)` |
| `rows()` | 행의 개수를 `int`로 반환한다. | `O(1)` |
| `cols()` | 열의 개수를 `int`로 반환한다. | `O(1)` |
| `at(int i, int j)` | `mint&`를 반환한다. `const` 행렬에서는 `const mint&`를 반환한다. | `O(1)` |
| `set(int i, int j, u64 v)` | 원소에 `v % mod`를 대입한다. | `O(1)` |
| `add(int i, int j, u64 v)` | 원소에 `v`를 더하고 `mod`로 정규화한다. | `O(1)` |
| `id(int n)` | `n × n` 항등행렬을 반환하는 정적 함수다. 대각 원소는 `1 % mod`다. | `O(1 + n²)` |
| `pow(u64 k)` | 정사각 행렬의 `k`제곱을 이진 거듭제곱으로 구한다. `k == 0`이면 `id(n)`을 반환한다. | `O((1 + n³)(1 + log(k + 1)))` |
| `transpose()` | 새 `m × n` 전치행렬을 반환한다. | `O(1 + n + nm)` |
| `out << a` | 행별로 원소를 출력한다. 각 원소 뒤에 공백 한 칸, 각 행 뒤에 줄바꿈을 쓰고 스트림을 반환한다. | `O(1 + n + nm)` |

### 산술과 비교

산술과 비교는 같은 `mod`의 행렬끼리 한다. 아래에서 `a`는 `n × m`이고, 곱셈의 `b`는 `m × p`다. 곱셈은 왼쪽 행렬에서 값이 `0`인 원소의 곱을 건너뛴다. `s`는 왼쪽 행렬의 0이 아닌 원소 수다. 덧셈·뺄셈·곱셈과 `pow()`의 차원 조건은 `assert`로 확인하므로 호출자가 맞춰야 한다.

| 연산 | 동작 | 시간 |
| --- | --- | --- |
| `a + b`, `a - b` | 같은 크기의 두 행렬의 합이나 차를 새 행렬로 반환한다. | `O(1 + nm)` |
| `a += b`, `a -= b` | 같은 크기의 `b`를 `a`에 더하거나 빼고 `a`의 참조를 반환한다. `b`가 `a`와 같은 객체여도 된다. | `O(1 + nm)` |
| `a * b` | `n × m` 행렬과 `m × p` 행렬을 곱해 새 `n × p` 행렬을 반환한다. `m == 0`이면 모든 원소가 `0`이다. | `O(1 + np + nm + sp)` |
| `a *= b` | `a`를 `n × p` 곱으로 바꾸고 `a`의 참조를 반환한다. 정사각 행렬이면 `a *= a`도 가능하다. | `O(1 + np + nm + sp)` |
| `a == b`, `a != b` | 두 행렬의 행 수, 열 수, 각 원소를 비교한다. 원소가 없는 행렬도 크기에 따라 구분한다. | 최악 `O(1 + nm)` |

### 사용 예시

```cpp
using matrix = _mod_matrix<7>;
matrix a(1, 2), b(2, 1);
a.set(0, 0, 12);
a.at(0, 1) = -1;
b.set(0, 0, 2);
b.add(1, 0, 1);

a *= b; // (12 × 2 - 1) mod 7 = 2
assert(a.rows() == 1 && a.cols() == 1 && a.at(0, 0) == 2);
assert(a.pow(3) == matrix::id(1));

a.clear(1, 1); // 같은 크기여도 원소는 0으로 초기화
assert(a.at(0, 0) == 0);
```

## area

`_area`는 단순 다각형 넓이의 두 배를 구한다. 꼭짓점은 경계를 따라 시계 또는 반시계 순서로 넣는다. 마지막 점에서 첫 점으로 돌아가는 변은 함수가 포함한다. [소스](../Geometry.cpp#L342) · [VS Code 스니펫 `>>area`](../snippets/area.code-snippets)

### `_area` API

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `ret(span<const pll> v)` | 신발끈 공식의 합에 절댓값을 취해 두 배 넓이를 `ll`로 반환한다. 점이 3개 미만이면 `0`이다. | `O(n)` |

`n = v.size()`다. 곱과 누적합은 `i128` 범위에, 두 배 넓이는 `ll` 범위에 있어야 한다.

### 사용 예시

```cpp
vector<pll> polygon{{0, 0}, {3, 0}, {0, 1}};
ll twice = _area::ret(polygon); // 삼각형 넓이는 1.5
assert(twice == 3);
reverse(polygon.begin(), polygon.end());
assert(_area::ret(polygon) == twice); // 꼭짓점 순서의 방향을 뒤집어도 넓이는 같다.
```

## line

`_line<T = i128>`은 직선을 `y = ax + b` 또는 `x = c`로 나타낸다. 기울기, 절편, 함수값과 교점 좌표는 `_frac<T>`로 정확하게 계산한다. 스니펫은 `>>frac` 다음에 `>>line`을 넣는다. [소스](../Geometry.cpp#L358) · [VS Code 스니펫 `>>line`](../snippets/line.code-snippets) · [분수](#frac)

### `_line` API

`T`는 부호 있는 정수 타입이다. 좌표 차이와 분수 연산의 모든 중간값이 `T` 범위에 있어야 한다. 표에서 `M`은 해당 연산의 `gcd`에 넘기는 정수의 최대 크기다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_line()` | 직선 `y = 0`을 생성한다. | `O(1)` |
| `_line(const pair<T, T>& p1, const pair<T, T>& p2)` | 두 점을 지나는 직선을 생성한다. x좌표가 같으면 같은 점을 두 번 넘긴 경우까지 포함해 그 x좌표의 수직선을 만든다. | 최악 `O(1 + log(M + 1))`, 수직선은 `O(1)` |
| `_line(const _frac<T>& slope, const pair<T, T>& p)` | 기울기가 `slope`이고 정수 좌표의 점 `p`를 지나는 직선을 생성한다. | `O(1 + log(M + 1))` |
| `is_vertical()` | 수직선이면 `true`를 반환한다. | `O(1)` |
| `slope()` | 기울기를 `const _frac<T>&`로 반환한다. 수직선이 아닌 경우에 호출한다. | `O(1)` |
| `intercept()` | y절편을 `const _frac<T>&`로 반환한다. 수직선이 아닌 경우에 호출한다. | `O(1)` |
| `vertical_x()` | 수직선의 x좌표를 `T`로 반환한다. 수직선에만 호출한다. | `O(1)` |
| `f(const _frac<T>& x)` | x좌표에 대응하는 y좌표를 `_frac<T>`로 반환한다. 수직선이 아닌 경우에 호출한다. | `O(1 + log(M + 1))` |
| `relation(const _line& ot)` | 교점 하나면 `relation_type::point`, 서로 다른 평행선이면 `parallel`, 같은 직선이면 `coincident`를 반환한다. | `O(1)` |
| `contains(const pair<_frac<T>, _frac<T>>& p)` | 점이 직선 위에 있으면 `true`를 반환한다. 분수 좌표 쌍으로 변환 가능한 정수 좌표 쌍도 받는다. | 최악 `O(1 + log(M + 1))`, 수직선은 `O(1)` |
| `intersect(const _line& ot)` | 유일한 교점의 좌표를 `optional<pair<_frac<T>, _frac<T>>>`로 반환한다. 평행하거나 일치하면 `nullopt`다. | 최악 `O(1 + log(M + 1))`, 교점이 유일하지 않으면 `O(1)` |
| `same(const _line& ot)` | 두 직선이 일치하면 `true`를 반환한다. | `O(1)` |

### 사용 예시

```cpp
using line = _line<>;
using frac = _frac<>;
line diagonal(frac(1, 2), {0, 0});
line vertical(pll{3, 0}, pll{3, 1});
assert(vertical.is_vertical() && vertical.vertical_x() == 3);
assert(diagonal.relation(vertical) == line::relation_type::point);

auto hit = diagonal.intersect(vertical);
assert(hit && hit->first == 3 && hit->second == frac(3, 2));
assert(diagonal.contains(pll{4, 2}));

line parallel(frac(1, 2), {0, 1});
assert(diagonal.relation(parallel) == line::relation_type::parallel);
assert(!diagonal.intersect(parallel));
assert(diagonal.same(line(frac(1, 2), {4, 2})));
```

## inter

`_inter`는 점이 단순 다각형의 외부, 경계, 내부 중 어디에 있는지 판정한다. 각 변에서 경계 포함 여부를 확인하고, 점에서 오른쪽으로 뻗는 수평 반직선과 변의 교차 횟수의 홀짝으로 내부를 판정한다. 스니펫은 `>>ccw`, `>>li`, `>>inter` 순서로 넣는다. [소스](../Geometry.cpp#L426) · [VS Code 스니펫 `>>inter`](../snippets/inter.code-snippets) · [선분 판정](#linear_intersection)

### `_inter` API

`p`는 다각형 경계를 따라 나열한 정수 좌표 꼭짓점이다. 시계·반시계 순서 모두 지원하며 마지막 꼭짓점과 첫 꼭짓점도 변으로 잇는다. 첫 꼭짓점을 끝에 한 번 더 넣어도 된다. 꼭짓점이 3개 미만이면 `outside`다. 꼭짓점이 3개 이상이고 모두 일직선 위에 있으면 변 위의 점은 `boundary`이고 나머지는 `outside`다. 좌표의 계산 범위는 `_ccw`와 같다. 두 함수의 추가 공간은 `O(1)`이다. [CCW](#ccw)

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `locate(span<const pll> p, pll now)` | `location::outside`, `location::boundary`, `location::inside` 중 하나를 반환한다. 변과 꼭짓점 위의 점은 `boundary`다. | 꼭짓점 `n`개에 `O(n)` |
| `ret(span<const pll> p, pll now)` | 점이 내부 또는 경계에 있으면 `true`를 반환한다. | `O(n)` |

### 사용 예시

```cpp
using _inter::location;
vector<pll> polygon{{0, 0}, {6, 0}, {6, 6}, {4, 6},
                    {4, 2}, {2, 2}, {2, 6}, {0, 6}};
assert(_inter::locate(polygon, {1, 4}) == location::inside);
assert(_inter::locate(polygon, {3, 4}) == location::outside);
assert(_inter::locate(polygon, {3, 2}) == location::boundary);
assert(_inter::ret(polygon, {2, 2})); // 꼭짓점도 포함
assert(_inter::locate(span<const pll>{}, {0, 0}) == location::outside);
```

<a name="rc"></a>

## Rotating Calipers

`_rc`는 회전 캘리퍼스로 볼록 다각형에서 가장 멀리 떨어진 두 점과 그 거리를 구한다. 스니펫은 `>>ccw`, `>>ch`, `>>rc` 순서로 넣는다. [소스](../Geometry.cpp#L446) · [VS Code 스니펫 `>>rc`](../snippets/rc.code-snippets)

### `_rc` API

볼록 다각형의 경계를 시계 또는 반시계 순서로 넘긴다. 시작 꼭짓점은 자유롭게 고를 수 있고, 변 위의 중간 점도 허용한다. 인접한 중복 좌표와 시작점을 닫는 중복 좌표는 하나로 합치고 가장 작은 ID를 남긴다. 모든 점이 일직선 위에 있으면 입력 순서에 관계없이 양 끝점을 고른다. 순서 없는 일반 점 집합은 먼저 볼록 껍질을 구한다. [볼록 껍질](#convex-hull)

공개 타입 `_rc::node`는 `_ch::node`의 별칭이다. 최대 거리가 같은 점 쌍이 여럿이면 각 쌍의 ID를 오름차순으로 놓고 사전순으로 가장 작은 쌍을 고른다. `ret()`의 두 좌표도 `idx()`의 ID 순서에 맞춰 반환한다.

조회 전에 `build()`를 호출한다. 빈 입력이면 좌표 쌍은 `{{0, 0}, {0, 0}}`, ID 쌍은 `{-1, -1}`, 거리는 `0`이다. 서로 다른 좌표가 하나면 같은 점과 ID를 두 번 반환하고 거리는 `0`이다. 외적의 절댓값과 거리 제곱을 비롯한 중간 계산은 `i128` 범위에 있어야 한다. 표에서 `n`은 처리하는 입력의 점 개수다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `build(span<const node> arr)` | 입력을 복사해 교체하고 각 점의 ID를 그대로 사용해 지름을 계산한다. `hull.idx()`를 넘기면 원래 점 집합의 ID를 사용한다. | `O(n)` |
| `build(span<const pll> arr)` | 입력을 복사해 교체하고 다각형에서의 0 기반 위치를 ID로 사용해 지름을 계산한다. `hull.ret()`를 넘길 수 있다. | `O(n)` |
| `build()` | 내부에 저장된 입력으로 지름을 다시 계산한다. | `O(n)` |
| `ret()` | 선택한 두 점을 `const pair<pll, pll>&`로 반환한다. | `O(1)` |
| `idx()` | 두 점의 ID 쌍을 `const pll&`로 반환한다. | `O(1)` |
| `dist2()` | 거리 제곱을 `ll`로 반환한다. 호출하려면 최종 거리 제곱이 `ll` 범위에 있어야 한다. | `O(1)` |
| `dist()` | 유클리드 거리를 `ld`(`long double`)로 반환한다. | `O(1)` |

### 사용 예시

```cpp
_ch hull;
vector<pll> points{{4, 3}, {4, 0}, {0, 3}, {0, 0}};
hull.make(points);
hull.build();

_rc diameter;
diameter.build(hull.idx()); // 원래 입력의 ID로 대각선 동률을 결정
assert((diameter.idx() == pll{0, 3}));
assert((diameter.ret() == pair<pll, pll>{points[0], points[3]}));
assert(diameter.dist2() == 25 && diameter.dist() == 5);

diameter.build(hull.ret()); // 볼록 껍질에서의 위치를 ID로 사용
assert((diameter.idx() == pll{0, 2}));
```

## dot

`_dot`은 정수 좌표로 내적을 계산한다. 점 세 개를 넣으면 가운데 점을 꼭짓점으로 사용한다. [소스](../Geometry.cpp#L522) · [VS Code 스니펫 `>>dot`](../snippets/dot.code-snippets)

### `_dot` API

좌표 차이, 곱, 합은 `i128` 범위에 있어야 하고 반환하는 내적은 `ll` 범위에 있어야 한다. 두 벡터의 길이가 양수일 때 내적이 양수면 각도가 `90°`보다 작고, `0`이면 `90°`, 음수면 `90°`보다 크다. 한쪽 벡터가 영벡터면 내적은 `0`이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `ret(pll a, pll b)` | 두 벡터 `a`와 `b`의 내적을 `ll`로 반환한다. | `O(1)` |
| `ret(pll l, pll m, pll r)` | `m → l`과 `m → r`의 내적을 반환한다. | `O(1)` |
| `ret(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3)` | 세 점의 좌표를 받아 가운데 점을 기준으로 같은 내적을 구한다. | `O(1)` |

### 사용 예시

```cpp
assert(_dot::ret({2, 3}, {-1, 4}) == 10); // 두 벡터의 내적
pll l{1, 0}, m{0, 0};
assert(_dot::ret(l, m, {1, 1}) > 0);  // 예각
assert(_dot::ret(l, m, {-1, 1}) < 0); // 둔각
assert(_dot::ret(1, 0, 0, 0, 0, 1) == 0); // 직각
```

## gf2

`_gf2<m>`은 원소가 0 또는 1인 GF(2)에서 연립방정식 `A v = b`를 가우스-조르당 소거법으로 푼다. 변수 개수 `m`은 컴파일 시 정하고, 방정식 개수 `n`은 생성자 또는 `clear(n)`으로 정한다. 행과 변수의 인덱스는 0부터 시작한다. [소스](../Geometry.cpp#L539) · [VS Code 스니펫 `>>gf2`](../snippets/gf2.code-snippets)

### `_gf2` API

확대 행렬의 각 행은 `bitset<m + 1>`이다. 비트 `0`부터 `m - 1`까지는 각 변수의 계수이고 비트 `m`은 우변이다. `n`은 음이 아닌 정수이고 `m = 0`과 `n = 0`도 허용한다. `build()`는 보존된 입력으로 매번 결과를 다시 계산한다. 처음 결과를 읽을 때와 입력을 바꾼 뒤에는 `ret()`보다 먼저 `build()`를 호출한다.

시간 복잡도에서 `w`는 비트셋의 워드 크기, `B = ceil((m + 1) / w)`는 한 행의 워드 수, `r = rank(A)`, `f = m - r`이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_gf2(ll n = 0)` | 0으로 채운 방정식 `n`개를 만든다. | `O((n + 1)B)` |
| `rows()` | 방정식 개수 `n`을 `ll`로 반환한다. | `O(1)` |
| `get_row(ll cy)` | 입력 행 `cy`를 const 참조로 반환한다. | `O(1)` |
| `set_row(ll cy, const bitset<m + 1>& v)` | 행 `cy`의 계수와 우변을 한꺼번에 바꾼다. | `O(B)` |
| `set_a(ll cy, ll cx, bool v)` | 행 `cy`의 변수 `cx` 계수를 바꾼다. | `O(1)` |
| `set_b(ll cy, bool v)` | 행 `cy`의 우변을 바꾼다. | `O(1)` |
| `clear()` | 행 수를 유지하고 모든 행을 0으로 채운다. | `O(1 + nB)` |
| `clear(ll n)` | 입력을 0으로 채운 `n`개 행으로 교체한다. | `O(1 + nB)` |
| `build(bool with_basis = 0)` | 입력의 복사본을 소거해 결과를 저장한다. `true`이면 영공간 기저도 저장한다. | 기본 `O(1 + m + nB + nm + nrB)`, 기저 생성에 `O(m(1 + f))` 추가 |
| `ret()` | 마지막으로 계산한 결과를 `const result&`로 반환한다. | `O(1)` |

### `result` API

| 함수·필드 | 동작 |
| --- | --- |
| `status` | `state::inconsistent`, `state::unique`, `state::multiple` 중 하나다. |
| `inconsistent()` | 해가 없으면 `true`다. |
| `unique()` | 해가 정확히 하나면 `true`다. |
| `multiple()` | 해가 여러 개면 `true`다. |
| `rank()` / `rk` | 해의 유무와 관계없이 계수 행렬 `A`의 랭크를 얻는다. |
| `free_count()` / `fr` | 자유 변수의 개수 `m - rank()`를 얻는다. |
| `get_value()` / `value` | 해가 있으면 모든 자유 변수를 0으로 놓은 해를 얻는다. 해가 없으면 모든 비트가 0이다. 비트 `i`는 변수 `i`에 대응한다. |
| `get_basis()` / `basis` | `build(true)`로 만든 `ker(A)`의 기저를 얻는다. 자유 변수의 열 번호가 작은 순서이며, 벡터마다 해당 자유 변수만 1이고 나머지 자유 변수는 0이다. 기본 `build()`의 기저는 빈 벡터다. |

조회 함수는 모두 `O(1)`이다. `get_value()`와 `get_basis()`는 const 참조를 반환한다. 기저는 우변과 무관하므로 해가 없는 경우에도 `build(true)`로 얻을 수 있다. 해가 있으면 `get_value()`에 기저의 임의 부분집합을 XOR해서 모든 해를 만들 수 있다. 기저를 저장하는 데 `O(f · ceil(m / w))`워드가 든다.

### 사용 예시

```cpp
_gf2<3> system(2);
system.set_row(0, bitset<4>(0b1111)); // v[0] XOR v[1] XOR v[2] = 1
system.build(true);

const auto& answer = system.ret();
assert(answer.multiple() && answer.rank() == 1 && answer.free_count() == 2);
assert(answer.get_value() == bitset<3>(0b001));
assert((answer.get_basis() == vector<bitset<3>>{0b011, 0b101}));
assert((answer.get_value() ^ answer.get_basis()[0]) == bitset<3>(0b010));

system.set_row(1, bitset<4>(0b0111)); // 같은 좌변이 0이라는 조건 추가
system.build(true);
assert(system.ret().inconsistent() && system.ret().get_basis().size() == 2);
```

## xor_basis

`_xor_basis<N = 64>`는 삽입한 `u64` 값의 부분집합으로 만들 수 있는 XOR 값들을 선형 기저로 관리한다. 빈 부분집합의 XOR은 0이며, 랭크가 `r`이면 서로 다른 값이 `2^r`개다. [소스](../Geometry.cpp#L612) · [VS Code 스니펫 `>>xor_basis`](../snippets/xor_basis.code-snippets)

### `_xor_basis` API

`u64`는 `uint64_t`이고 비트 폭 `N`은 `0 <= N <= 64`이다. `insert()`에 넘기는 값은 하위 `N`비트에 들어가야 하며, 호출자가 이 조건을 보장한다. `N = 0`이면 삽입할 수 있는 값과 만들 수 있는 값은 모두 0뿐이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_xor_basis()` | 랭크가 0인 빈 기저를 생성한다. | `O(1 + N)` |
| `clear()` | 기존 기저를 비우고 랭크를 0으로 만든다. | `O(1 + N)` |
| `insert(u64 x)` | `x`를 추가해 랭크가 증가하면 `true`, 이미 만들 수 있으면 `false`를 반환한다. | `O(1 + N)` |
| `merge(const _xor_basis& ot)` | 같은 폭의 두 기저가 생성하는 공간의 합을 현재 기저에 저장한다. 자기 자신도 넘길 수 있다. | `O(1 + N^2)` |
| `can_make(u64 x)` | `x`를 부분집합의 XOR로 만들 수 있는지 확인한다. `N`비트 범위 위에 1이 있으면 `false`다. | `O(1 + N)` |
| `max_xor(u64 x = 0)` | 만들 수 있는 값 `v`에 대해 `x ^ v`의 최댓값을 `u64`로 반환한다. `x`의 상위 비트는 결과에 그대로 남는다. | `O(1 + N)` |
| `min_xor(u64 x = 0)` | 만들 수 있는 값 `v`에 대해 `x ^ v`의 최솟값을 `u64`로 반환한다. `x`의 상위 비트는 결과에 그대로 남는다. | `O(1 + N)` |
| `kth(u64 k)` | 서로 다른 XOR 값을 부호 없는 오름차순으로 놓았을 때 인덱스 `k`의 값을 `optional<u64>`로 반환한다. `k = 0`은 0에 대응하며, `k >= 2^r`이면 `nullopt`다. 랭크가 64이면 모든 `u64` 인덱스가 유효하다. | `O(1 + N)` |
| `get_basis()` | 피벗별 기저 배열을 `const array<u64, N>&`로 반환한다. `i`번째 칸에는 최상위 1비트가 `i`인 기저 벡터가 들어가며, 빈 칸의 값은 0이다. | `O(1)` |
| `rank()` | 기저 벡터의 개수를 `int`로 반환한다. | `O(1)` |

### 사용 예시

```cpp
_xor_basis<3> basis;
bool added = basis.insert(6);
assert(added);
basis.insert(5);
assert(basis.rank() == 2 && basis.can_make(3)); // 6 XOR 5 = 3
assert(basis.kth(0) == 0 && basis.kth(2) == 5 && !basis.kth(4));
assert(basis.max_xor(8) == 14 && basis.min_xor(8) == 8);

_xor_basis<3> other;
other.insert(1);
basis.merge(other);
assert(basis.rank() == 3 && basis.kth(7) == 7);
```
