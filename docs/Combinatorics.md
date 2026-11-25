# Combinatorics.cpp

## comb

`_comb<mod>`는 팩토리얼 테이블로 조합과 순열을 계산한다. `mod`는 소수여야 하며 기본값은 `998244353`이다. 조회 결과는 `[0, mod)`의 `u64`다. 스니펫은 `>>mint` 다음에 `>>comb`를 넣는다. [소스](../Combinatorics.cpp#L14) · [VS Code 스니펫 `>>comb`](../snippets/comb.code-snippets)

### `_comb` API

`N`은 가장 최근에 전처리한 상한이다. 테이블은 `O(N + 1)` 공간을 쓴다. 전처리와 조회 범위는 호출자가 맞춘다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_comb(int n = 0)` | 생성과 함께 `build(n)`을 호출한다. | `O(n + log mod)` |
| `build(int n)` | `0`부터 `n`까지 다시 전처리한다. `0 <= n < mod`이고 `n < INT_MAX`여야 한다. | `O(n + log mod)` |
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
