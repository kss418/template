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

auto labels = alphabet::tf<A>("0Az"); // {0, 10, 61}

_trie<A> trie;
trie.insert("0Az");
assert(trie.get_state(std::span<const int>(labels)).count() == 1);

auto root = trie.get_state(std::string{});
assert(root.go(A::encode('0')).valid());
```

### 사용자 정의 정책

dense 클래스에 사용할 정책은 컴파일 타임 정수 상수 `size`를 제공한다. 문자열 API나 `tf()`를 쓴다면 `encode(unsigned char)`도 구현하고, 허용 문자마다 `[0, size)`의 라벨을 반환하게 한다. 정수 라벨 API만 쓸 때는 `size`만 있으면 된다. `decode()`는 라벨을 문자로 복원할 때 필요하다.

## Trie

`_trie<Alphabet>`은 문자열을 중복 허용으로 저장한다. `_trie<>`의 기본 정책은 `alphabet::lowercase`다. 스니펫을 쓸 때는 `>>alphabet`을 먼저 넣는다. [소스](../Automata.cpp#L81) · [VS Code 스니펫 `>>trie`](../snippets/trie.code-snippets)

- 문자열은 정책이 허용하는 문자만 넣는다. `span<const int>`와 `state::go(c)`는 `[0, Alphabet::size)` 라벨을 받는다.
- 빈 문자열도 저장할 수 있다. `get_state("")`는 루트이며 `id()`는 `0`이다.
- 상태는 원본 trie의 현재 개수를 읽는다. `erase()`로 마지막 문자열을 지워도 경로는 남으므로, 문자열 존재 여부는 `end()`, 접두사 존재 여부는 `prefix_count() > 0`으로 확인한다. `clear()` 뒤에는 상태를 다시 얻는다.

### `_trie` API

`s`는 `const std::string&` 또는 `std::span<const int>`다. `L`은 입력 또는 결과 길이, `Σ`는 `Alphabet::size`다. 사전순은 라벨의 오름차순이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `insert(s)` | 한 개를 추가하고 끝 상태를 반환한다. | `O(1 + LΣ)` amortized |
| `erase(s)` | 한 개를 지운다. 해당 문자열이 없으면 `false`다. | `O(L)` |
| `get_state(s)` | 경로의 끝 상태를 반환한다. 경로가 없으면 무효 상태다. | `O(L)` |
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
trie.insert("ab");
trie.insert("ab");
trie.insert("abc");

auto ab = trie.get_state("ab");
assert(ab.count() == 2 && ab.prefix_count() == 3);
assert(trie.rank("abc") == 2);
assert((trie.kth(3) == std::vector<int>{0, 1, 2}));

ab.it_child([](int label, auto child){
    assert(label == 2 && child.end());
});
```

## Sparse Trie

`_sparse_trie<T>`는 `T` 타입의 라벨 수열을 중복 허용으로 저장한다. 기본 `T`는 `ll`이고, 라벨은 복사하고 `std::less<T>`로 비교할 수 있어야 한다. [소스](../Automata.cpp#L197) · [VS Code 스니펫 `>>sparse_trie`](../snippets/sparse_trie.code-snippets)

- `std::span<const T>`와 `state::go()`는 라벨을 그대로 받는다. 문자열을 넘기면 각 바이트를 `unsigned char`로 읽어 `T`로 변환한다. 문자열 API를 쓰려면 이 변환이 가능해야 한다.
- 빈 수열의 상태는 루트(`id() == 0`)다. 삭제 후에도 경로는 남으므로, 저장 여부는 `end()`, 접두사 존재 여부는 `prefix_count() > 0`으로 확인한다. `clear()` 뒤에는 상태를 다시 얻는다.

### `_sparse_trie` API

`s`는 `const std::string&` 또는 `std::span<const T>`다. `L`은 입력 또는 결과 길이, `D`는 상태의 최대 자식 수, `N`은 남아 있는 상태 수다. 사전순은 `std::less<T>`를 따르고, 복잡도는 라벨 연산이 `O(1)`일 때 기준이다.

| 함수 | 동작 | 시간 |
| --- | --- | --- |
| `insert(s)` | 한 개를 추가하고 끝 상태를 반환한다. | `O(1 + L log(D + 2))` amortized |
| `erase(s)` | 한 개를 지운다. 수열이 없으면 `false`다. | `O(1 + L log(D + 2))` |
| `get_state(s)` | 경로의 끝 상태를 반환한다. 경로가 없으면 무효 상태다. | `O(1 + L log(D + 2))` |
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
trie.insert(std::span<const ll>(word));

auto root = trie.get_state(std::span<const ll>{});
auto prefix = root.go(-5);
assert(prefix.prefix_count() == 1);
assert(prefix.go(1'000'000'000).end());
assert(trie.kth(1) == word);
```
