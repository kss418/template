#include <bits/stdc++.h>
namespace graphs {
#include "../Graph.cpp"
}
using namespace std;
static int checks = 0;
#define CHECK(c) do { ++checks; if (!(c)) throw runtime_error(string("line ") + to_string(__LINE__) + ": " + #c); } while (0)
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().point()), const vector<long long>&>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().line()), const vector<pair<long long,long long>>&>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().ret()), const vector<vector<pair<long long,long long>>>&>);

void check_bcc(bool base){
    int a = base, b = base+1, c = base+2;
    graphs::_bcc graph(c,base); graph.add(a,b); graph.add(b,c); graph.build();
    const auto& read = graph;
    CHECK(read.point() == vector<long long>{b});
    CHECK(read.line().size() == 2); CHECK(read.ret().size() == 2); CHECK(graph.num[b] == 1);
    auto points = read.point(), low = graph.mn, order = graph.seq, split = graph.num;
    auto bridges = read.line(); auto blocks = read.ret(); auto visits = graph.cnt;
    const auto* stored_points = &read.point(); const auto* stored_lines = &read.line(); const auto* stored_blocks = &read.ret();
    for(int i = 0; i < 10; ++i){
        CHECK(&read.point() == stored_points); CHECK(&read.line() == stored_lines); CHECK(&read.ret() == stored_blocks);
        CHECK(read.point() == points); CHECK(read.line() == bridges); CHECK(read.ret() == blocks);
        CHECK(graph.mn == low); CHECK(graph.seq == order); CHECK(graph.num == split); CHECK(graph.cnt == visits);
    }
    graph.build(); CHECK(read.point() == points); CHECK(read.line() == bridges); CHECK(read.ret() == blocks);
    graph.add(a,c); graph.build();
    CHECK(read.point().empty()); CHECK(read.line().empty()); CHECK(read.ret().size() == 1); CHECK(read.ret()[0].size() == 3);
    CHECK(graph.num[b] == 0); CHECK(graph.st.empty()); CHECK(points == vector<long long>{b});
    graph.clear(c,base); graph.add(a,b); graph.add(a,c); graph.build();
    CHECK(read.point() == vector<long long>{a}); CHECK(graph.num[a] == 2);
    graph.clear(); graph.build(); CHECK(read.point() == vector<long long>{a}); CHECK(read.line().size() == 2);
    graph.clear(base,base); graph.build(); CHECK(read.point().empty()); CHECK(read.line().empty()); CHECK(read.ret().empty());
    graphs::_bcc empty(0,base); empty.build(); CHECK(empty.point().empty()); CHECK(empty.line().empty()); CHECK(empty.ret().empty());
}

template<class Cycle>
void complete_graph(Cycle& graph, int count, int base){
    for(int i = 0; i < count; ++i) for(int j = i+1; j < count; ++j) graph.add(i+base,j+base);
}
void check_cycles(bool base){
    graphs::_3cycle triangle(base+4,base); graphs::_4cycle square(base+4,base);
    complete_graph(triangle,5,base); complete_graph(square,5,base);
    const auto& t = triangle; const auto& s = square;
    for(int repeat = 0; repeat < 2; ++repeat){
        CHECK(triangle.build() == 10); CHECK(square.build() == 15);
        for(int query = 0; query < 20; ++query){ CHECK(t.ret() == 10); CHECK(s.ret() == 15); }
        CHECK(triangle.build(3) == 1); CHECK(square.build(3) == 0); CHECK(t.ret() == 1); CHECK(s.ret() == 0);
        CHECK(triangle.build(7) == 3); CHECK(square.build(7) == 1); CHECK(t.ret() == 3); CHECK(s.ret() == 1);
        CHECK(triangle.build(1) == 0); CHECK(square.build(1) == 0); CHECK(t.ret() == 0); CHECK(s.ret() == 0);
    }
    triangle.clear(base+3,base); square.clear(base+3,base);
    for(int i = 0; i < 3; ++i){ triangle.add(base+i,base+i+1); square.add(base+i,base+i+1); }
    CHECK(triangle.build() == 0); CHECK(square.build() == 0);
    triangle.add(base,base+3); square.add(base,base+3);
    CHECK(triangle.build() == 0); CHECK(square.build() == 1); CHECK(t.ret() == 0); CHECK(s.ret() == 1);
    triangle.add(base,base+2); square.add(base,base+2);
    CHECK(triangle.build() == 2); CHECK(square.build() == 1);
    triangle.add(base+1,base+3); square.add(base+1,base+3);
    CHECK(triangle.build() == 4); CHECK(square.build() == 3);
    triangle.clear(0,base); square.clear(0,base);
    CHECK(triangle.build() == 0); CHECK(square.build() == 0); CHECK(t.ret() == 0); CHECK(s.ret() == 0);
}
int main(){
    try {
        for(bool base : {false,true}){ check_bcc(base); check_cycles(base); }
        cout << "PASS " << checks << " stored-graph-result checks\n";
        return 0;
    } catch(const exception& e){ cerr << "FAIL " << e.what() << '\n'; return 1; }
}
