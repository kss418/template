# Combinatorics.cpp

## comb

`_comb<mod>`는 팩토리얼 테이블로 조합과 순열을 계산한다. `mod`는 소수여야 하며 기본값은 `998244353`이다. 조회 결과는 `[0, mod)`의 `u64`다. 스니펫은 `>>mint` 다음에 `>>comb`를 넣는다. [소스](../Combinatorics.cpp#L14) · [VS Code 스니펫 `>>comb`](../snippets/comb.code-snippets)

### `_comb` API

`N`은 가장 최근에 전처리한 상한이다. 테이블은 `O(N + 1)` 공간을 쓴다. 전처리와 조회 범위는 호출자가 맞춘다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_comb(int n = 0)` | 생성과 함께 `build(n)`을 호출한다. | `O(n + log mod)` |
| `build(int n)` | `0`부터 `n`까지 다시 전처리한다. `0 <= n < mod`여야 한다. | `O(n + log mod)` |
| `comb(int n, int r)` | 조합 `nCr`을 반환한다. `n < 0`, `r < 0`, `r > n`이면 `0`이다. 그 외에는 `n <= N`이어야 한다. | `O(1)` |
| `perm(int n, int r)` | 순열 `nPr`을 반환한다. `n < 0`, `r < 0`, `r > n`이면 `0`이다. 그 외에는 `n <= N`이어야 한다. | `O(1)` |
| `multicomb(int n, int r)` | `n`종에서 `r`개를 중복 선택하는 수를 반환한다. 음수 인자면 `0`이고, `n == 0`이면 `r == 0`일 때만 `1`이다. 그 외에는 `n + r - 1 <= N`이어야 한다. | `O(1)` |
| `factorial(int n)` | `n!`을 반환한다. `0 <= n <= N`이어야 한다. | `O(1)` |
| `inv_factorial(int n)` | `n!`의 모듈러 역원을 반환한다. `0 <= n <= N`이어야 한다. | `O(1)` |

### 사용 예시

```cpp
_comb<7> table(6); // 0!부터 6!까지 전처리
assert(table.comb(5, 2) == 3);      // 10 mod 7
assert(table.perm(5, 2) == 6);      // 20 mod 7
assert(table.multicomb(3, 4) == 1); // C(6, 4) mod 7

assert(table.factorial(4) == 3);
assert(table.inv_factorial(4) == 5); // 3 * 5 mod 7 == 1
```

## lucas

`_lucas`는 소수 `mod`에 대해 `n`이 `mod` 이상인 조합도 계산한다. 전처리 공간은 `O(mod)`이고, 조합·순열 결과는 `[0, mod)`의 `int`다. 스니펫은 `>>dynamic_mint` 다음에 `>>lucas`를 넣는다. [소스](../Combinatorics.cpp#L52) · [VS Code 스니펫 `>>lucas`](../snippets/lucas.code-snippets)

### `_lucas` API

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_lucas()` | 전처리 전 상태로 생성한다. 조합·순열을 조회하기 전에 `build(mod)`를 호출한다. | `O(1)` |
| `_lucas(int mod)` | 생성과 함께 `build(mod)`를 호출한다. | `O(mod)` |
| `build(int mod)` | 소수 `mod >= 2`를 받아 다시 전처리한다. | `O(mod)` |
| `get_mod()` | 현재 모듈러를 반환한다. 전처리 전에는 `0`이다. | `O(1)` |
| `comb(ll n, ll r)` | 조합 `nCr`을 반환한다. 음수 인자 또는 `r > n`이면 `0`이다. | `O(1 + log_mod(n + 1))` |
| `perm(ll n, ll r)` | 순열 `nPr`을 반환한다. 음수 인자, `r > n` 또는 `r > n % mod`이면 `0`이다. | `O(1)` |
| `multicomb(ll n, ll r)` | `n`종에서 `r`개를 중복 선택하는 수를 반환한다. 음수 인자면 `0`이고, `n == 0`이면 `r == 0`일 때만 `1`이다. 그 외에는 `n + r - 1 <= LLONG_MAX`여야 한다. | `O(1 + log_mod(n + r + 1))` |

### 사용 예시

```cpp
_lucas table(7); // 소수 7로 전처리
assert(table.comb(10, 3) == 1); // 120 mod 7
assert(table.comb(1'000'000'000'000LL, 1) == 1); // n mod 7
assert(table.perm(10, 4) == 0); // 10 * 9 * 8 * 7에는 7이 포함된다.
assert(table.multicomb(4, 3) == 6); // C(6, 3) mod 7

table.build(5); // 모듈러를 바꾸고 다시 전처리
assert(table.comb(10, 3) == 0);
```

## comb_crt

`_comb_crt`는 등록한 소수 거듭제곱 `p^e`의 최소공배수 `M`을 모듈러로 사용한다. 같은 소수를 여러 번 등록할 수 있고, 등록한 인수가 없으면 `M = 1`이다. 스니펫은 `>>dynamic_mint`, `>>crt`, `>>lucas`를 준비한 뒤 `>>comb_crt`를 넣는다. [소스](../Combinatorics.cpp#L97) · [VS Code 스니펫 `>>comb_crt`](../snippets/comb_crt.code-snippets)

### `_comb_crt` API

`k`는 등록한 인수 수, `S`는 등록한 `p^e`의 합이다. 전처리에 `O(1 + S)` 공간을 쓴다. 인수를 등록하거나 지운 뒤에는 `build()`를 호출하고 조회한다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `add(int p, int e)` | 소수 `p`의 거듭제곱 `p^e`를 등록한다. `e <= 0`이면 건너뛴다. | `O(1)` amortized |
| `build()` | 등록한 인수로 다시 전처리한다. 각 `p^e < LLONG_MAX`이고 `M <= LLONG_MAX`여야 한다. | `O(1 + S)` |
| `comb(ll n, ll r)` | 조합 `nCr`의 나머지를 담은 `result`를 반환한다. 음수 인자 또는 `r > n`이면 나머지는 `0`이다. | `O(1 + k(log(n + 2) + log M))` |
| `perm(ll n, ll r)` | 순열 `nPr`의 나머지를 담은 `result`를 반환한다. 음수 인자 또는 `r > n`이면 나머지는 `0`이다. | `O(1 + k(log(n + 2) + log M))` |
| `multicomb(ll n, ll r)` | `n`종에서 `r`개를 중복 선택하는 수의 나머지를 반환한다. 음수 인자면 `0`이고, `n == 0`이면 `r == 0`인 경우만 `1 mod M`이다. 그 외에는 `n + r - 1 <= LLONG_MAX`여야 한다. | `O(1 + k(log(n + r + 2) + log M))` |
| `clear()` | 등록한 인수와 테이블을 모두 지운다. | `O(1 + S)` 상한 |

조회가 돌려준 `result`에서 `get_value()`는 `[0, M)`의 나머지, `get_mod()`는 `M`을 반환한다. 둘 다 `O(1)`이다.

### 사용 예시

```cpp
_comb_crt table;
table.add(2, 2);
table.add(2, 3);
table.add(3, 2);
table.build(); // lcm(4, 8, 9) = 72

auto result = table.comb(10, 4);
assert(result.get_mod() == 72);
assert(result.get_value() == 66); // 210 mod 72

table.add(5, 1);
table.build(); // 인수를 추가했으니 다시 전처리
auto next = table.comb(10, 4);
assert(next.get_mod() == 360 && next.get_value() == 210);
```
