# Automata.cpp

## alphabet

`alphabet`은 문자열의 각 바이트를 정수 라벨로 바꾸는 정책 모음이다. `_trie<>`와 `_ac<>`의 기본 정책은 `alphabet::lowercase`다. [소스](../Automata.cpp#L13) · [VS Code 스니펫 `>>alphabet`](../snippets/alphabet.code-snippets)

### 기본 정책

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

복잡도는 기본 정책 기준이다. `n`은 문자열의 바이트 수이며 `tf()`의 반환 벡터는 `O(n)` 공간을 쓴다.

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
