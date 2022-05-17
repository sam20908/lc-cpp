#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <array>
#include <charconv>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
using namespace std;

#ifdef LC_LOCAL
struct TreeNode {
  int val;
  TreeNode *left = nullptr;
  TreeNode *right = nullptr;
};
struct ListNode {
  int val;
  ListNode *next = nullptr;
};
#endif

template <typename T, typename = void> struct is_container {
  static constexpr bool value = false;
};
template <typename T>
struct is_container<T, void_t<decltype(T{}.begin()), decltype(T{}.end())>> {
  static constexpr bool value = true;
};
template <typename = void> inline constexpr bool always_false = false;

template <typename T, bool ReadEnd> T parse(FILE *f, int &c) {
  if constexpr (is_same_v<T, int>) {
    int ans = 0, neg = 0;
    while ((char(c = fgetc(f)) >= '0' && (char)c <= '9') || (char)c == '-') {
      if ((char)c == '-')
        neg = 1;
      else
        ans = ans * 10 + ((char)c - '0') * (1 - 2 * neg);
    }
    if constexpr (!ReadEnd)
      ungetc(c, f);
    return ans;
  } else if constexpr (is_same_v<T, string>) {
    string ans;
    fgetc(f);
    while (char(c = fgetc(f)) != '"')
      ans += (char)c;
    if constexpr (ReadEnd)
      c = fgetc(f);
    return ans;
  } else if constexpr (is_same_v<T, TreeNode *>) {
    int e = 2;
    auto dummy = new TreeNode{};
    deque<TreeNode *> q;
    q.push_back(dummy);
    fgetc(f);
    while ((char)c != ']') {
      int sz = q.size();
      while (sz > 0) {
        int val = 0, neg = 0;
        bool is_null = false;
        while (char(c = fgetc(f)) != ',' && (char)c != ']') {
          if ((char)c >= '0' && (char)c <= '9')
            val = val * 10 + ((char)c - '0') * (1 - 2 * neg);
          else if ((char)c == '-')
            neg = 1;
          else
            is_null = true;
        }
        if (!is_null) {
          auto new_node = new TreeNode{val};
          if (e & 1)
            q[0]->left = new_node;
          else
            q[0]->right = new_node;
          q.push_back(new_node);
        }
        if (e++ % 2 == 0) {
          q.pop_front();
          sz--;
        }
      }
      e = 1;
    }
    if constexpr (ReadEnd)
      c = fgetc(f);
    return dummy->right;
  } else if constexpr (is_same_v<T, ListNode *>) {
    auto dummy = new ListNode{};
    auto cur = dummy;
    fgetc(f);
    if (char(c = fgetc(f)) != ']') {
      ungetc(c, f);
      while ((char)c != ']') {
        cur->next = new ListNode{parse<int, true>(f, c)};
        cur = cur->next;
      }
    }
    if constexpr (ReadEnd)
      c = fgetc(f);
    return dummy->next;
  } else if constexpr (is_container<T>::value) {
    T ans;
    fgetc(f);
    if (char(c = fgetc(f)) != ']') {
      ungetc(c, f);
      while ((char)c != ']')
        ans.emplace_back(parse<typename T::value_type, true>(f, c));
    }
    if constexpr (ReadEnd)
      c = fgetc(f);
    return ans;
  } else
    static_assert(always_false<T>, "parsing for type not supported");
}

template <bool WriteEnd, typename T> void write(FILE *f, const T &val) {
  static constexpr const char *end = WriteEnd ? "\n" : "";
  if constexpr (is_same_v<T, int>)
    fprintf(f, "%d%s", val, end);
  else if constexpr (is_same_v<T, float>)
    fprintf(f, "%.f%s", val, end);
  else if constexpr (is_same_v<T, bool>)
    fprintf(f, "%s%s", val ? "true" : "false", end);
  else if constexpr (is_same_v<T, string>)
    fprintf(f, "\"%s\"%s", val.data(), end);
  else if constexpr (is_same_v<T, TreeNode *>) {
    deque<TreeNode *> q;
    fprintf(f, "[");
    if (val) {
      q.push_back(val);
      fprintf(f, "%d", val->val);
    }
    while (!q.empty()) {
      int sz = q.size();
      string ans;
      while (sz-- > 0) {
        auto cur = q.front();
        q.pop_front();
        if (cur->left) {
          q.push_back(cur->left);
          ans += ',';
          ans += to_string(cur->left->val);
        } else
          ans += ",null";
        if (cur->right) {
          q.push_back(cur->right);
          ans += ',';
          ans += to_string(cur->right->val);
        } else
          ans += ",null";
      }
      if (!q.empty())
        fprintf(f, ans.data());
    }
    fprintf(f, "]%s", end);
  } else if constexpr (is_same_v<T, ListNode *>) {
    int c = 0;
    auto cur = val;
    fprintf(f, "[");
    while (cur) {
      fprintf(f, "%s%d", c++ ? "," : "", cur->val);
      cur = cur->next;
    }
    fprintf(f, "]%s", end);
  } else if constexpr (is_container<T>::value) {
    fprintf(f, "[");
    int c = 0;
    for (int i = 0; i < val.size(); i++) {
      fprintf(f, "%s", c++ ? "," : "");
      write<false>(f, val[i]);
    }
    fprintf(f, "]%s", end);
  } else
    static_assert(always_false<T>, "writing for type not supported");
}

template <typename Args, size_t... Idx>
void write_args(FILE *f, const Args &args, index_sequence<Idx...>) {
  ((fprintf(f, "#%lld: ", Idx + 1), write<true>(f, get<Idx + 1>(args))), ...);
}

template <typename Solution, typename R, typename... Ts>
void exec(R (Solution::*fn)(Ts...)) {
  constexpr bool returns = !is_same_v<R, void>;
  auto out = fopen("output.txt", "w");
  if constexpr (sizeof...(Ts) == 0) {
    if constexpr (returns) {
      write<true>(out, (Solution{}.*fn)());
    } else
      (Solution{}.*fn)();
  } else {
    auto in = fopen("input.txt", "r");
    int c = fgetc(in);
    if (c == EOF) {
      fclose(in);
      return;
    } else
      ungetc(c, in);
    while (true) {
      if (char(c = fgetc(in)) == EOF)
        break;
      else
        ungetc(c, in);
      if (char(c = fgetc(in)) == '/') {
        while ((c = fgetc(in)) != '\n' && c != EOF) {
        }
      } else {
        ungetc(c, in);
        tuple args{Solution{}, parse<decay_t<Ts>, true>(in, c)...};
        if constexpr (!returns) {
          apply(fn, args);
          fprintf(out, "New state of parameters:\n");
          write_args(out, args, index_sequence_for<Ts...>{});
        } else
          write<true>(out, apply(fn, args));
#ifdef LC_LOCAL
        cout << endl;
#endif
      }
    }
    fclose(in);
  }
  fclose(out);
}

void _print(const string &x) { cout << '\"' << x << '\"'; }
void _print(bool x) { cout << (x ? "true" : "false"); }
template <typename T> enable_if_t<is_arithmetic_v<T>> _print(const T &x) {
  cout << x;
}
template <typename T, size_t... Idx>
void _print2(const T &x, index_sequence<Idx...>) {
  int c = 0;
  ((cout << (c++ ? "," : ""), _print(get<Idx>(x))), ...);
}
template <typename T> void_t<typename tuple_size<T>::type> _print(const T &x) {
  cout << '{';
  _print2(x, make_index_sequence<tuple_size_v<T>>{});
  cout << '}';
}
template <typename T> enable_if_t<is_container<T>::value> _print(const T &x) {
  cout << '[';
  int c = 0;
  for (const auto &e : x) {
    cout << (c++ ? "," : "");
    _print(e);
  }
  cout << ']';
}

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define NUM_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define DBG_VAL(x) cout << '[' << #x << " = ", _print(x), cout << "] "
#define DBG_1(x) DBG_VAL(x)
#define DBG_2(x, ...) DBG_VAL(x), DBG_1(__VA_ARGS__)
#define DBG_3(x, ...) DBG_VAL(x), DBG_2(__VA_ARGS__)
#define DBG_4(x, ...) DBG_VAL(x), DBG_3(__VA_ARGS__)
#define DBG_5(x, ...) DBG_VAL(x), DBG_4(__VA_ARGS__)
#define DBG_6(x, ...) DBG_VAL(x), DBG_5(__VA_ARGS__)
#define DBG_7(x, ...) DBG_VAL(x), DBG_6(__VA_ARGS__)
#define DBG_8(x, ...) DBG_VAL(x), DBG_7(__VA_ARGS__)
#define DBG_9(x, ...) DBG_VAL(x), DBG_8(__VA_ARGS__)
#define DBG_10(x, ...) DBG_VAL(x), DBG_9(__VA_ARGS__)
// supports up to 10 arguments debugging at one time

#ifdef LC_LOCAL
#define dbg(...) CONCAT(DBG_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__), cout << endl
#else
#define dbg(...)
#endif

#define all(x) begin(x), end(x)
#define has(c, x) (c.find(x) != c.end())
#define umap unordered_map
#define uset unordered_set
#define vec vector
#define valid(i, j, m, n) (i >= 0 && i < m && j >= 0 && j < n)
#define valid2(i, j, sm, sn, m, n) (i >= sm && i < m && j >= sn && j < n)

template <typename T> T &amin(T &a, const T &b) { return a = min(a, b); }
template <typename T> T &amax(T &a, const T &b) { return a = max(a, b); }

using pii = pair<int, int>;
using pipii = pair<int, pii>;
using pippiipii = pair<int, pair<pii, pii>>;
using ll = long long;

namespace std {
template <typename T> class hash<std::pair<T, T>> {
public:
  size_t operator()(const pair<T, T> &x) const {
    // https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
    ll lhs = hash<T>()(x.first), rhs = hash<T>()(x.second);
    lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
  }
};
} // namespace std

constexpr int dx[] = {-1, 0, 0, 1, -1, -1, 1, 1};
constexpr int dy[] = {0, -1, 1, 0, -1, 1, -1, 1};
constexpr int INF = 1e9;
constexpr double EPS = 1e-9;

// ----- CHANGE FOR PROBLEM -----
class Solution {
public:
  void test() {}
};
// ----- CHANGE FOR PROBLEM -----

#ifdef LC_LOCAL
int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);
  exec(&Solution::test); // CHANGE FOR PROBLEM
}
#endif
