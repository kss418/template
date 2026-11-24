# Automata.cpp

## alphabet

`alphabet`은 문자열의 각 바이트를 0부터 시작하는 정수 라벨로 바꾸는 정책 모음이다. `_trie<Alphabet>`과 `_ac<Alphabet>`은 `Alphabet::size`를 상태당 전이 배열의 크기로 쓰고, 문자열 입력을 받을 때 `Alphabet::encode()`로 라벨을 만든다. 두 클래스의 기본 정책은 `alphabet::lowercase`다. [소스](../Automata.cpp#L13) · [VS Code 스니펫 `>>alphabet`](../snippets/alphabet.code-snippets)

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

표의 복잡도는 기본 정책 기준이며, `n = s.size()`는 바이트 수다. `tf()`의 반환 벡터는 `O(n)` 공간을 쓰고, 빈 문자열이면 빈 벡터를 반환한다. 기본 정책의 `encode()`와 `decode()`는 `constexpr`이며 유효한 입력에서 서로 역변환이다.

- `encode()`에 넘기는 문자와 `decode()`에 넘기는 라벨은 호출자가 범위를 맞춘다. 범위 검사는 없다. 잘못된 문자를 dense 전이에 넘기면 배열 범위를 벗어날 수 있다.
- `_trie`·`_ac`의 `span<const int>` 입력과 `state::go(int)`는 인코딩된 라벨을 받는다. 예를 들어 `alphabet::alnum`에서 문자 `'A'`의 정수 라벨은 `10`이다.
- 라벨 번호가 dense trie의 사전순을 정한다. `alnum`에서는 숫자, 대문자, 소문자 순이다. `bytes`는 `std::string`을 바이트 단위로 처리하므로 UTF-8 문자는 여러 라벨이 될 수 있다. 문자열에 포함된 NUL 바이트도 보존한다.
- `_sparse_trie`와 `_sparse_ac`는 `Alphabet` 정책 인자를 받지 않는다. 문자열 입력은 바이트 값을 라벨로 변환한다. 다른 매핑을 쓰려면 `tf<Alphabet, T>()`로 직접 변환해 `span<const T>` 입력으로 넘긴다.

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
