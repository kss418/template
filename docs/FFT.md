# FFT.cpp

## FFT

`_fft<sz, T = ll>`은 정수 계수의 다항식을 변환해 덧셈과 곱셈을 계산한다. `T`는 입력 계수의 정수 타입이다. 계수는 낮은 차수부터 넣고, `node`는 변환된 상태를 보관한다. [소스](../FFT.cpp#L14) · [VS Code 스니펫 `>>fft`](../snippets/fft.code-snippets)

### FFT API

`N = capacity()`는 FFT 길이이며 `node` 하나는 `O(N)` 공간을 쓴다. 같은 `_fft<sz, T>`의 `node`끼리 연산한다. 내부에서 `long double`을 사용하므로 길이와 계수 크기에 따라 반올림 오차로 결과 정수가 달라질 수 있다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `capacity()` | `sz` 이상인 최소 2의 거듭제곱을 반환한다. `sz = 0`이면 `0`이다. | `O(1)` |
| `make(span<const T> arr)` | 정수 계수열을 변환해 `node`를 만든다. `arr.size() <= N`이어야 한다. 같은 원소 타입의 연속 범위는 `span`으로 전달할 수 있다. | `O(N log N)` |
| `inverse<R = ll>(const node& a)` | 역변환하고 반올림한 계수를 `vector<R>`로 반환한다. 계수는 정수 타입 `R` 범위에 있어야 한다. 빈 `node`이면 빈 벡터다. | `O(N log N)` |
| `inverse_carry(const node& a, ll radix = 10)` | 역변환한 비음수 `ll` 계수에 올림을 적용해 낮은 자리부터 `vector<ll>`로 반환한다. `radix >= 2`여야 한다. 상위의 0을 제거하되 값이 0이면 `{0}`을 반환한다. 빈 `node`는 빈 벡터이고, 결과는 `N`자리보다 길 수 있다. | `O(N log N + k)` |
| `carry(const node& a, ll radix = 10)` | `inverse_carry()`로 구한 자릿수를 다시 변환해 `node`로 반환한다. 올림 후 자릿수도 `N` 이하여야 한다. | `O(N log N)` |

`k`는 올림 후 자릿수다. `N <= 1`이면 FFT 단계는 생략한다.

### Node API

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `size()` | 현재 다항식의 계수 개수를 `ll`로 반환한다. | `O(1)` |
| `empty()` | 계수 개수가 `0`이면 `true`다. | `O(1)` |
| `a += b` | `a`에 `b`를 더한다. 결과 길이는 두 길이 중 큰 값이다. | `O(N)` |
| `a + b` | 합을 새 `node`로 반환한다. | `O(N)` |
| `a *= b` | `a`를 곱셈 결과로 바꾼다. 한쪽이 비었으면 빈 `node`가 된다. 그 외에는 `a.size() + b.size() - 1 <= N`이어야 한다. | `O(N)` |
| `a * b` | 곱을 새 `node`로 반환한다. `*=`과 같은 길이 조건을 적용한다. | `O(N)` |
| `out << a` | 역변환한 계수를 높은 차수부터 구분자 없이 출력한다. 빈 `node`는 `0`을 출력한다. 곱셈 결과를 십진 정수로 출력할 때는 `carry()`로 자릿수를 먼저 정리한다. | `O(N log N)` |

### 다항식 곱셈 예시

```cpp
_fft<5, int> fft;
static_assert(fft.capacity() == 8);

const int a[]{1, 2}; // 1 + 2x
auto fa = fft.make(span{a});
auto fb = fft.make(vector<int>{3, 4, 5}); // 3 + 4x + 5x²

auto product = fa * fb;
assert(product.size() == 4);
assert((fft.inverse<int>(product) == vector<int>{3, 10, 13, 10}));
```

### 자릿수 올림 예시

```cpp
_fft<4> fft;
auto a = fft.make(vector<ll>{9, 9}); // 99를 낮은 자리부터 저장
auto square = a * a;
assert((fft.inverse(square) == vector<ll>{81, 162, 81})); // 올림 전 계수
assert((fft.inverse_carry(square) == vector<ll>{1, 0, 8, 9}));

auto digits = fft.carry(square); // 올림 후에도 node로 보관
ostringstream out;
out << digits;
assert(out.str() == "9801");
```
