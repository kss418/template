#include "../Segtree.cpp"
static int checks=0;
#define CHECK(c) do { ++checks; if(!(c)) throw runtime_error(string("line ")+to_string(__LINE__)+": "+#c); } while(0)
template<class Tree, class Value>
void check_sum(bool base){
    using value=Value;
    vector<value> a{{7},{10},{20}};
    Tree tree(2,base);tree.make(a);
    CHECK(tree.query(0,2).v==(base?30:37));
    tree.make(vector<value>{});CHECK(tree.query(0,2).v==0);
    tree.make(a);CHECK(tree.query(0,2).v==(base?30:37));
    tree.make(vector<value>{{9}});CHECK(tree.query(0,0).v==(base?0:9));
    tree.clear(0,!base);tree.make(a);CHECK(tree.query(0,2).v==(!base?30:37));
    Tree constructed(span<const value>(a),base);CHECK(constructed.query(0,2).v==(base?30:37));
}
void check_sorted(bool base){
    using value=_mt<>::node;vector<value> a{{7},{10},{20}};
    _mt<> tree(2,base);tree.make(a);CHECK(tree.geq(0,2,{0})==(base?2:3));CHECK(tree.eq(0,2,{7})==(!base));
    tree.make(vector<value>{});CHECK(tree.geq(0,2,{0})==0);
    tree.make(vector<value>{{9}});CHECK(tree.eq(0,0,{9})==(!base));
    tree.clear(0,!base);tree.make(a);CHECK(tree.geq(0,2,{0})==(!base?2:3));
    _mt<> constructed(span<const value>(a),base);CHECK(constructed.geq(0,2,{0})==(base?2:3));
}
int main(){
    try{for(bool base:{false,true}){check_sum<_seg<>,monoid::node>(base);check_sum<_prop<>,action::node>(base);check_sum<_fw<>,group::node>(base);check_sorted(base);}cout<<"PASS "<<checks<<" retained-make-base checks\n";return 0;}
    catch(const exception&e){cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
