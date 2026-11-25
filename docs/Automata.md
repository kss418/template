# Automata.cpp

## alphabet

`alphabet`은 문자열의 각 바이트를 정수 라벨로 바꾸는 정책 모음이다. [소스](../Automata.cpp#L13) · [VS Code 스니펫 `>>alphabet`](../snippets/alphabet.code-snippets)

### 제공 정책

| 정책 | `size` | 허용 문자와 라벨 |
| --- | ---: | --- |
| `alphabet::lowercase` | 26 | `a`–`z` → `0`–`25` |
| `alphabet::uppercase` | 26 | `A`–`Z` → `0`–`25` |
| `alphabet::digits` | 10 | `0`–`9` → `0`–`9` |
| `alphabet::alnum` | 62 | `0`–`9` → `0`–`9`, `A`–`Z` → `10`–`35`, `a`–`z` → `36`–`61` |
| `alphabet::bytes` | 256 | 바이트 값 `0`–`255` → 같은 값 |

### 계약과 복잡도

| API | 동작 | 시간 |
| --- | --- | --- |
| `Alphabet::size` | 라벨의 개수인 `static constexpr int` 상수. 유효한 라벨은 `[0, size)`다. | `O(1)` |
| `Alphabet::encode(unsigned char c)` | 허용 문자 `c`의 라벨을 `int`로 반환한다. | `O(1)` |
| `Alphabet::decode(int i)` | 유효한 라벨 `i`에 대응하는 문자를 `unsigned char`로 반환한다. | `O(1)` |
| `alphabet::tf<Alphabet, T = int>(const std::string& s)` | 문자열의 바이트 순서대로 라벨을 담은 `std::vector<T>`를 반환한다. `T`는 라벨 값을 표현할 수 있어야 한다. | `O(n)` |

복잡도는 내장 정책 기준이다. `n`은 문자열의 바이트 수이며 `tf()`의 반환 벡터는 `O(n)` 공간을 쓴다.

- 허용 문자와 `[0, size)` 라벨만 넘긴다. 범위 검사는 없다.
- `_trie`·`_ac`의 `span<const int>`와 `state::go(int)`는 이미 인코딩된 라벨을 받는다.

### 사용 예시

```cpp
using A = alphabet::alnum;
static_assert(A::encode('A') == 10);
static_assert(A::decode(61) == 'z');

auto labels = alphabet::tf<A>("0Az"); // 문자별 라벨 {0, 10, 61}

std::string word;
for(int c : labels) word += static_cast<char>(A::decode(c));
assert(word == "0Az");

_trie<A> trie;
trie.insert(word);
// 이미 변환한 라벨로 같은 경로를 조회한다.
assert(trie.get_state(std::span<const int>(labels)).count() == 1);

// 길이를 지정하면 NUL 바이트도 변환한다.
auto bytes = alphabet::tf<alphabet::bytes>(std::string("\0\xff", 2));
assert((bytes == std::vector<int>{0, 255}));
```

### 사용자 정의 정책

dense 클래스에 사용할 정책은 컴파일 타임 정수 상수 `size`를 제공한다. 문자열 API나 `tf()`를 쓴다면 `encode(unsigned char)`도 구현하고, 허용 문자마다 `[0, size)`의 라벨을 반환하게 한다. 정수 라벨 API만 쓸 때는 `size`만 있으면 된다. `decode()`는 라벨을 문자로 복원할 때 필요하다.

## Trie

`_trie<Alphabet>`은 문자열을 중복 허용으로 저장한다. `_trie<>`의 기본 정책은 `alphabet::lowercase`다. 스니펫을 쓸 때는 `>>alphabet`을 먼저 넣는다. [소스](../Automata.cpp#L81) · [VS Code 스니펫 `>>trie`](../snippets/trie.code-snippets)

- 문자열은 정책이 허용하는 문자만 넣는다. `span<const int>`와 `state::go(c)`는 `[0, Alphabet::size)` 라벨을 받는다.
- 빈 문자열도 저장할 수 있다. 빈 문자열의 끝 상태는 `root()`와 같다.
- 상태는 원본 trie의 현재 개수를 읽는다. `erase()`로 마지막 문자열을 지워도 경로는 남으므로, 문자열 존재 여부는 `end()`, 접두사 존재 여부는 `prefix_count() > 0`으로 확인한다. `clear()` 뒤에는 상태를 다시 얻는다.

### `_trie` API

`s`는 `const std::string&` 또는 `std::span<const int>`다. `L`은 입력 또는 결과 길이, `Σ`는 `Alphabet::size`다. 사전순은 라벨의 오름차순이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `insert(s)` | 한 개를 추가하고 끝 상태를 반환한다. | `O(1 + LΣ)` amortized |
| `erase(s)` | 한 개를 지운다. 해당 문자열이 없으면 `false`다. | `O(L)` |
| `get_state(s)` | 경로의 끝 상태를 반환한다. 경로가 없으면 무효 상태다. | `O(L)` |
| `root()` | 루트 상태를 반환한다. 비어 있어도 `id()`는 `0`이다. | `O(1)` |
| `size()` | 중복을 포함한 문자열 개수를 반환한다. | `O(1)` |
| `rank(s)` | `s`보다 사전순으로 앞선 문자열 개수를 반환한다. | `O(LΣ)` |
| `kth(int k)` | 중복을 포함해 `1`부터 세는 `k`번째 문자열을 `vector<int>` 라벨로 반환한다. 범위를 벗어나면 빈 벡터다. | `O(LΣ)` |
| `clear()` | 빈 트라이로 초기화한다. | `O(Σ)` |

`kth()`가 반환한 빈 벡터는 저장된 빈 문자열일 수도 있으므로, 호출 전에 `1 <= k <= size()`로 구분할 수 있다.

### `state` API

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `id()` | 상태 번호를 반환한다. 경로가 없으면 `-1`이다. | `O(1)` |
| `valid()` | 경로가 있으면 `true`다. `id() != -1`과 같다. | `O(1)` |
| `count()` | 이 경로와 정확히 같은 문자열의 개수를 반환한다. 무효 상태에서는 `0`이다. | `O(1)` |
| `end()` | 이 경로에서 끝나는 문자열이 있으면 `true`다. `count() > 0`과 같다. | `O(1)` |
| `prefix_count()` | 이 경로를 접두사로 갖는 문자열의 개수를 반환한다. 무효 상태에서는 `0`이다. | `O(1)` |
| `has_child()` | 이 경로를 접두사로 갖는 더 긴 문자열이 있으면 `true`다. `prefix_count() > count()`와 같다. | `O(1)` |
| `go(int c)` | 라벨 `c`의 자식 상태로 이동한다. 간선이 없으면 무효 상태다. | `O(1)` |
| `it_child(const F& f)` | 유효한 문자열이 남아 있는 직계 자식을 라벨 순으로 순회하며 `f(int label, state child)`를 호출한다. | `O(Σ)` |
| `it_subtree(const F& f)` | 유효한 문자열이 남아 있는 상태를 자기 자신부터 깊이 우선으로 순회하며 `f(state node)`를 호출한다. | `O(1 + KΣ)`, `K`는 방문 상태 수 |

### 사용 예시

```cpp
_trie<> trie;
auto ab = trie.insert("ab"); // 삽입한 문자열의 끝 상태
trie.insert("ab");           // 중복도 개수에 포함
trie.insert("abc");

auto a = trie.root().go(alphabet::lowercase::encode('a')); // 루트에서 'a'로 이동
assert(a.prefix_count() == 3);

assert(ab.count() == 2 && ab.prefix_count() == 3); // 정확히 일치 2개, 접두사 일치 3개
assert(trie.rank("abc") == 2);                     // "abc"보다 앞선 문자열 개수
assert((trie.kth(3) == std::vector<int>{0, 1, 2}));

ab.it_child([](int label, auto child){
    assert(label == 2 && child.end()); // 직계 자식 "abc"
});

bool erased = trie.erase("ab"); // 한 개만 삭제
assert(erased && ab.count() == 1);

int words = 0;
ab.it_subtree([&](auto node){ words += node.count(); }); // "ab" 상태부터 순회
assert(words == 2); // "ab", "abc" 각 1개
```

## Sparse Trie

`_sparse_trie<T>`는 `T` 타입의 라벨 수열을 중복 허용으로 저장한다. 기본 `T`는 `ll`이고, 라벨은 복사하고 `std::less<T>`로 비교할 수 있어야 한다. [소스](../Automata.cpp#L198) · [VS Code 스니펫 `>>sparse_trie`](../snippets/sparse_trie.code-snippets)

- `std::span<const T>`와 `state::go()`는 라벨을 그대로 받는다. 문자열을 넘기면 각 바이트를 `unsigned char`로 읽어 `T`로 변환한다. 문자열 API를 쓰려면 이 변환이 가능해야 한다.
- 빈 수열의 끝 상태는 `root()`와 같다. 삭제 후에도 경로는 남으므로, 저장 여부는 `end()`, 접두사 존재 여부는 `prefix_count() > 0`으로 확인한다. `clear()` 뒤에는 상태를 다시 얻는다.

### `_sparse_trie` API

`s`는 `const std::string&` 또는 `std::span<const T>`다. `L`은 입력 또는 결과 길이, `D`는 상태의 최대 자식 수, `N`은 남아 있는 상태 수다. 사전순은 `std::less<T>`를 따르고, 복잡도는 라벨 연산이 `O(1)`일 때 기준이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `insert(s)` | 한 개를 추가하고 끝 상태를 반환한다. | `O(1 + L log(D + 2))` amortized |
| `erase(s)` | 한 개를 지운다. 수열이 없으면 `false`다. | `O(1 + L log(D + 2))` |
| `get_state(s)` | 경로의 끝 상태를 반환한다. 경로가 없으면 무효 상태다. | `O(1 + L log(D + 2))` |
| `root()` | 루트 상태를 반환한다. 비어 있어도 `id()`는 `0`이다. | `O(1)` |
| `size()` | 중복을 포함한 수열 개수를 반환한다. | `O(1)` |
| `rank(s)` | `s`보다 사전순으로 앞선 수열 개수를 반환한다. | `O(1 + L(D + 1))` |
| `kth(int k)` | 중복을 포함해 `1`부터 세는 `k`번째 수열을 `vector<T>`로 반환한다. 범위 밖이면 빈 벡터다. | `O(1 + L(D + 1))` |
| `clear()` | 빈 트라이로 초기화한다. | `O(N)` |

저장된 빈 수열도 `kth()`에서 빈 벡터로 나오므로, `1 <= k <= size()`로 범위 안인지 확인한다.

### `state` API

`d`는 현재 상태의 자식 수다. `D`와 `d`에는 삭제 후 남은 경로도 포함한다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `id()` | 상태 번호를 반환한다. 경로가 없으면 `-1`이다. | `O(1)` |
| `valid()` | 경로가 있으면 `true`다. `id() != -1`과 같다. | `O(1)` |
| `count()` | 이 경로와 정확히 같은 수열의 개수를 반환한다. 무효 상태에서는 `0`이다. | `O(1)` |
| `end()` | 이 경로에서 끝나는 수열이 있으면 `true`다. `count() > 0`과 같다. | `O(1)` |
| `prefix_count()` | 이 경로를 접두사로 갖는 수열의 개수를 반환한다. 무효 상태에서는 `0`이다. | `O(1)` |
| `has_child()` | 이 경로를 접두사로 갖는 더 긴 수열이 있으면 `true`다. `prefix_count() > count()`와 같다. | `O(1)` |
| `go(const T& c)` | 라벨 `c`의 자식 상태로 이동한다. 간선이 없으면 무효 상태다. | `O(log(d + 2))` |
| `it_child(const F& f)` | 수열이 남아 있는 직계 자식을 라벨 순으로 순회하며 `f(const T& label, state child)`를 호출한다. | `O(1 + d)` |
| `it_subtree(const F& f)` | 수열이 남아 있는 상태를 자기 자신부터 깊이 우선으로 순회하며 `f(state node)`를 호출한다. | `O(1 + K + E)` |

`K`는 방문 상태 수, `E`는 그 상태에서 검사하는 간선 수다. 저장된 수열이 없는 가지의 간선도 `E`에 포함한다.

### 사용 예시

```cpp
_sparse_trie<> trie;
const std::vector<ll> word{-5, 1'000'000'000};
trie.insert(std::span<const ll>(word)); // 정수 라벨을 그대로 저장
trie.insert("A");                      // 바이트 라벨 {65}

auto root = trie.root();
auto prefix = root.go(-5);
assert(prefix.prefix_count() == 1);
assert(root.go(65).end()); // "A"가 저장된 상태

prefix.it_child([](const ll& label, auto child){
    assert(label == 1'000'000'000 && child.end());
});

assert(trie.rank("A") == 1); // -5로 시작하는 수열이 앞선다.
assert(trie.kth(1) == word);
```

## XOR Trie

`_xor_trie`는 `u64` 값을 중복 허용으로 저장하고, `x ^ y`의 최솟값·최댓값·순위·개수를 조회한다. `y`는 현재 저장된 값이다. [소스](../Automata.cpp#L325) · [VS Code 스니펫 `>>xor_trie`](../snippets/xor_trie.code-snippets)

- `max_bit`는 가장 높은 비트 번호이며 `0`–`63`이다. 저장할 값과 전체 XOR 마스크가 `B = max_bit + 1`비트에 들어오게 설정한다. 기본값 `63`은 전체 `u64` 범위를 지원한다. XOR 비교의 기준값과 임계값은 `max_bit`와 관계없이 전체 `u64`를 쓸 수 있다.
- `xor_all(mask)`는 현재 저장된 값마다 `y ^ mask`를 적용한다. 이후에도 삽입·삭제·조회 인자는 현재 값 기준으로 넘긴다. `state::go()`도 현재 값의 비트를 높은 자리부터 받는다.
- `kth`와 `argkth`의 순위는 `1 <= k <= size()` 범위다.

### `_xor_trie` API

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `_xor_trie(int max_bit = 63)` | 빈 트라이를 만든다. | `O(1)` |
| `clear(int max_bit)` | 값을 모두 지우고 비트 범위와 누적 XOR을 초기화한다. | `O(1)` |
| `root()` | 루트 상태를 반환한다. 비어 있어도 `id()`는 `0`이다. | `O(1)` |
| `size()` | 중복을 포함한 값의 개수를 반환한다. | `O(1)` |
| `insert(u64 x)` | `x` 한 개를 추가하고 끝 상태를 반환한다. | `O(B)` amortized |
| `erase(u64 x)` | `x` 한 개를 지운다. 없으면 `false`다. | `O(B)` |
| `get_state(u64 x)` | `x`의 끝 상태를 반환한다. 경로가 없으면 무효 상태다. | `O(B)` |
| `xor_all(u64 mask)` | 현재 저장된 모든 값에 `mask`를 XOR한다. | `O(1)` |
| `min(u64 x)` | `x ^ y`의 최솟값을 반환한다. 빈 트라이에서는 `x`다. | `O(B)` |
| `argmin(u64 x)` | `x ^ y`를 최소로 만드는 `y`를 반환한다. 빈 트라이에서는 `0`이다. | `O(B)` |
| `max(u64 x)` | `x ^ y`의 최댓값을 반환한다. 빈 트라이에서는 `x`다. | `O(B)` |
| `argmax(u64 x)` | `x ^ y`를 최대로 만드는 `y`를 반환한다. 빈 트라이에서는 `0`이다. | `O(B)` |
| `kth(u64 x, int k)` | `x ^ y`를 오름차순으로 정렬했을 때 중복을 포함한 `k`번째 결과를 반환한다. | `O(B)` |
| `argkth(u64 x, int k)` | `k`번째 XOR 결과를 만드는 `y`를 반환한다. | `O(B)` |
| `less(u64 x, u64 k)` | `x ^ y < k`인 `y`의 개수를 반환한다. | `O(B)` |
| `equal(u64 x, u64 k)` | `x ^ y == k`인 `y`의 개수를 반환한다. | `O(B)` |
| `greater(u64 x, u64 k)` | `x ^ y > k`인 `y`의 개수를 반환한다. | `O(B)` |

### `state` API

삭제 후에도 경로는 남을 수 있다. 값이 남았는지는 끝 상태에서 `end()`로, 비트 접두사가 살아 있는지는 `prefix_count() > 0`으로 확인한다. `clear()` 뒤에는 상태를 다시 얻는다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `id()` | 상태 번호를 반환한다. 무효 상태에서는 `-1`이다. | `O(1)` |
| `valid()` | 경로가 있으면 `true`다. `id() != -1`과 같다. | `O(1)` |
| `count()` | 끝 상태에서 해당 값의 개수를 반환한다. 그 외에는 `0`이다. | `O(1)` |
| `end()` | 이 끝 상태에 값이 있으면 `true`다. `count() > 0`과 같다. | `O(1)` |
| `prefix_count()` | 현재 비트 접두사로 시작하는 값의 개수를 반환한다. 무효 상태에서는 `0`이다. | `O(1)` |
| `has_child()` | 이 접두사에서 더 이어지는 값이 있으면 `true`다. | `O(1)` |
| `go(bool c)` | 현재 값 기준 비트 `c`로 이동한다. 다음 경로가 없으면 무효 상태다. | `O(1)` |
| `it_child(const F& f)` | 값이 남은 직계 자식에 대해 비트 `0`, `1` 순으로 `f(int bit, state child)`를 호출한다. | `O(1)` |
| `it_subtree(const F& f)` | 값이 남은 상태를 자기 자신부터 깊이 우선으로 순회하며 `f(state node)`를 호출한다. | `O(1 + K)`, `K`는 방문 상태 수 |

### 사용 예시

```cpp
_xor_trie trie(2); // 2번 비트까지, 값의 범위는 0~7
trie.insert(1);
trie.insert(6);
trie.insert(6);

assert(trie.max(3) == 5);       // XOR 결과는 {2, 5, 5}
assert(trie.argmax(3) == 6);    // 결과 5를 만드는 저장값
assert(trie.kth(3, 2) == 5);
assert(trie.equal(3, 5) == 2); // 중복도 센다.

trie.xor_all(3); // 현재 값은 {2, 5, 5}
trie.insert(7);  // 전체 XOR 이후에도 현재 값으로 삽입
bool erased = trie.erase(5);
assert(erased && trie.size() == 3);

auto five = trie.root().go(1).go(0).go(1); // 현재 값 5의 비트 101
assert(five.count() == 1);
```

## Aho-Corasick

`_ac<Alphabet>`은 텍스트에서 여러 패턴의 출현을 찾는다. `_ac<>`의 기본 정책은 `alphabet::lowercase`다. 스니펫을 쓸 때는 `>>alphabet`을 먼저 넣는다. [소스](../Automata.cpp#L475) · [VS Code 스니펫 `>>ac`](../snippets/ac.code-snippets)

- `insert()`로 패턴을 등록한 뒤 `build()`를 호출하고 매칭을 조회한다. 패턴을 더 등록하면 다시 `build()`를 호출한다.
- 문자열은 정책이 허용하는 문자만 넣는다. `std::span<const int>`와 `go(int c)`에는 `[0, Alphabet::size)` 라벨을 넘긴다.

### `_ac` API

`s`는 `const std::string&` 또는 `std::span<const int>`다. `L`은 입력 길이, `N`은 상태 수, `Σ`는 `Alphabet::size`다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `insert(s)` | 패턴 한 개를 등록하고 끝 상태를 반환한다. 중복 등록이 가능하며 패턴은 비어 있지 않아야 한다. | `O(1 + LΣ)` amortized |
| `build()` | 등록된 패턴으로 매칭을 준비한다. 같은 패턴 목록으로 다시 호출할 수 있다. | `O(NΣ)` |
| `clear()` | 패턴을 모두 지운다. 이전 상태는 다시 얻어야 한다. | `O(Σ)` |
| `root()` | `id()`가 `0`인 루트를 반환한다. | `O(1)` |
| `size()` | 루트를 포함한 상태 수를 반환한다. | `O(1)` |
| `depth()` | 상태 번호로 접근하는 깊이 배열을 참조로 반환한다. | `O(1)` |
| `terminal()` | 상태 번호로 접근하는 패턴 끝 여부 배열을 참조로 반환한다. | `O(1)` |
| `count(s)` | 겹치는 출현과 중복 등록을 모두 포함한 총 매칭 수를 반환한다. | `O(L)` |
| `count_each(s)` | 길이 `size()`의 출현 횟수 벡터를 반환한다. 패턴의 끝 상태 번호로 조회하며, 중복 등록해도 해당 칸은 패턴 한 개의 출현 횟수다. | `O(L + N)` |
| `it_state(s, const F& f)` | 루트에서 시작해 각 라벨을 읽은 뒤 `f(state node)`를 한 번 호출한다. | `O(L)` |

### `state` API

`M`은 현재 위치에서 끝나는 서로 다른 패턴 수다. 순회 복잡도에는 콜백 본문 비용을 포함하지 않는다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `id()` | 상태 번호를 반환한다. | `O(1)` |
| `terminal_count()` | 이 상태에 직접 등록된 패턴의 등록 횟수를 반환한다. | `O(1)` |
| `match_count()` | 현재 위치에서 매칭된 패턴 수를 중복 등록까지 포함해 반환한다. | `O(1)` |
| `go(int c)` | 라벨 `c` 하나를 읽은 뒤의 상태를 반환한다. | `O(1)` |
| `max_match_len()` | 현재 위치에서 매칭된 가장 긴 패턴 길이를 반환한다. 매칭이 없으면 `0`이다. | `O(1)` |
| `it_match(const F& f)` | 매칭된 패턴마다 긴 것부터 `f(state matched)`를 호출한다. 같은 패턴의 중복 등록은 한 번 방문한다. | `O(1 + M)` |
| `it_match(int k, const F& f)` | 위 순서로 최대 `k`개의 패턴을 순회한다. `k <= 0`이면 호출하지 않는다. | `O(1 + min(k, M))` (`k > 0`) |
| `it_match_lens(const F& f)` | 매칭된 패턴마다 긴 것부터 `f(int len)`을 호출한다. 같은 패턴의 중복 등록은 한 번 방문한다. | `O(1 + M)` |
| `it_match_lens(int k, const F& f)` | 위 순서로 최대 `k`개의 길이를 순회한다. `k <= 0`이면 호출하지 않는다. | `O(1 + min(k, M))` (`k > 0`) |

### 사용 예시

```cpp
_ac<> ac;
auto he = ac.insert("he"); // 패턴별 조회에 끝 상태 번호를 쓴다.
ac.insert("he");
auto she = ac.insert("she");
ac.build();

assert(ac.count("sheshe") == 6); // "he"는 두 번 등록되어 4회, "she"는 2회
auto each = ac.count_each("sheshe");
assert(each[he.id()] == 2 && each[she.id()] == 2); // 패턴별 출현 횟수

auto last = ac.root();
ac.it_state("she", [&](auto node){ last = node; }); // 마지막 문자를 읽은 상태
assert(last.terminal_count() == 1 && last.match_count() == 3);

std::vector<int> lengths;
last.it_match_lens([&](int len){ lengths.push_back(len); });
assert((lengths == std::vector<int>{3, 2})); // "she", "he" 순서로 한 번씩
```

## Sparse Aho-Corasick

`_sparse_ac<T>`는 `T` 타입의 라벨 수열에서 여러 패턴의 출현을 찾는다. 기본 `T`는 `ll`이다. `T`는 기본 생성, 복사, `==` 비교가 가능해야 한다. [소스](../Automata.cpp#L605) · [VS Code 스니펫 `>>sparse_ac`](../snippets/sparse_ac.code-snippets)

- 문자열 입력은 각 바이트를 `unsigned char`로 읽어 `T`로 변환한다. `std::span<const T>`와 `go(const T& c)`는 라벨을 그대로 받는다.
- 패턴 등록을 마친 뒤 `build()`를 호출한다. 패턴을 더 등록하면 매칭을 조회하기 전에 다시 `build()`를 호출한다.

### `_sparse_ac` API

`s`는 `const std::string&` 또는 `std::span<const T>`다. `L`은 입력 길이, `N`은 상태 수, `H`는 가장 긴 패턴 길이, `D`는 상태당 최대 자식 수다. 라벨 연산 비용은 `O(1)`로 본다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `insert(s)` | 패턴 한 개를 등록하고 끝 상태를 반환한다. 중복 등록이 가능하며 패턴은 비어 있지 않아야 한다. | `O(1 + L(D + 1))` amortized |
| `build()` | 등록된 패턴으로 매칭을 준비한다. 같은 목록으로 다시 호출할 수 있다. | 최대 `O(N(H + 1)(D + 1))` |
| `clear()` | 패턴을 모두 지운다. 이전 상태는 다시 얻어야 한다. | `O(N)` 상한 |
| `root()` | `id()`가 `0`인 루트를 반환한다. | `O(1)` |
| `size()` | 루트를 포함한 상태 수를 반환한다. | `O(1)` |
| `depth()` | 상태 번호로 접근하는 깊이 배열을 참조로 반환한다. | `O(1)` |
| `terminal()` | 상태 번호로 접근하는 패턴 끝 여부 배열을 참조로 반환한다. | `O(1)` |
| `count(s)` | 겹치는 출현과 중복 등록을 모두 포함한 총 매칭 수를 반환한다. | `O(1 + L(D + 1))` |
| `count_each(s)` | 길이 `size()`의 출현 횟수 벡터를 반환한다. 패턴 끝 상태 번호로 조회하며, 중복 등록 횟수를 곱하지 않는다. | `O(N + L(D + 1))` |
| `it_state(s, const F& f)` | 루트에서 시작해 각 라벨을 읽은 뒤 `f(state node)`를 한 번 호출한다. | `O(1 + L(D + 1))` |

`go()`는 실패 링크를 여러 번 따라갈 수 있다. 텍스트 전체를 루트부터 읽을 때는 상태 깊이가 한 라벨당 최대 1 증가하므로, 실패 링크를 따라가는 총 횟수는 `O(L)`이다.

### `state` API

`M`은 현재 위치에서 끝나는 서로 다른 패턴 수다. 순회 비용은 콜백 본문을 제외한다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `id()` | 상태 번호를 반환한다. | `O(1)` |
| `terminal_count()` | 이 상태에 직접 등록된 패턴의 등록 횟수를 반환한다. | `O(1)` |
| `match_count()` | 현재 위치에서 끝나는 패턴 수를 중복 등록까지 포함해 반환한다. | `O(1)` |
| `go(const T& c)` | 라벨 `c` 하나를 읽은 뒤의 상태를 반환한다. | 최대 `O((H + 1)(D + 1))` |
| `max_match_len()` | 현재 위치에서 끝나는 가장 긴 패턴 길이를 반환한다. 매칭이 없으면 `0`이다. | `O(1)` |
| `it_match(const F& f)` | 매칭된 패턴마다 긴 것부터 `f(state matched)`를 한 번 호출한다. | `O(1 + M)` |
| `it_match(int k, const F& f)` | 위 순서로 최대 `k`개를 순회한다. `k <= 0`이면 호출하지 않는다. | `O(1 + min(k, M))` (`k > 0`) |
| `it_match_lens(const F& f)` | 매칭된 패턴마다 긴 것부터 `f(int len)`을 한 번 호출한다. | `O(1 + M)` |
| `it_match_lens(int k, const F& f)` | 위 순서로 최대 `k`개의 길이를 순회한다. `k <= 0`이면 호출하지 않는다. | `O(1 + min(k, M))` (`k > 0`) |

### 사용 예시

```cpp
_sparse_ac<> ac;
const std::vector<ll> tail{1'000'000'000}, word{-5, 1'000'000'000};
auto tail_state = ac.insert(std::span<const ll>(tail));
auto word_state = ac.insert(std::span<const ll>(word));
ac.insert("A"); // 바이트 라벨 {65}
ac.build();

const std::vector<ll> text{-5, 1'000'000'000, 65, -5, 1'000'000'000};
auto labels = std::span<const ll>(text);
assert(ac.count(labels) == 5); // word 2회, tail 2회, "A" 1회
auto each = ac.count_each(labels);
assert(each[word_state.id()] == 2 && each[tail_state.id()] == 2);

auto last = ac.root().go(-5).go(1'000'000'000);
assert(last.terminal_count() == 1); // 이 상태에 직접 등록된 패턴은 word
assert(last.match_count() == 2);    // 같은 자리에서 tail도 끝난다.
```
