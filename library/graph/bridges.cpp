vector<pair<int, int>> bridges(int n, vector<vector<int>> &g) {
  vector<int> tin(n, -1), low(n, -1);
  vector<pair<int, int>> ans;
  auto dfs = [&, timer = 0](auto &self, int v, int p = -1) mutable -> void {
    tin[v] = low[v] = timer++;
    for (int to : g[v]) {
      if (to == p)
        continue;
      if (tin[to] != -1) {
        low[v] = min(low[v], tin[to]);
      } else {
        self(self, to, v);
        low[v] = min(low[v], low[to]);
        if (low[to] > tin[v])
          ans.push_back({v, to});
      }
    }
  };
  for (int i = 0; i < n; ++i)
    if (tin[i] == -1)
      dfs(dfs, i);
  return ans;
};