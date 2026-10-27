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
class pst_text {
public:
    class node {
    public:
        string v;
        node(string v):v(move(v)){}
    };
    static node id(){return {""};}
    static node op(const node& a,const node& b){return {a.v+b.v};}
};
class pst_xor {
public:
    class node {
    public:
        int v;
        node(int v):v(v){}
    };
    static node id(){return {0};}
    static node op(const node& a,const node& b){return {a.v^b.v};}
    static node inv(const node& a){return a;}
};
template<class T>
concept has_pst_difference = requires(const T& tree){tree.diff_query(0,0,0,0);};
template<class T>
concept has_pst_inverse = requires(const T& tree,typename T::node a){tree.inv(a);};
static_assert(!has_pst_difference<_pst<>>);
static_assert(!has_pst_inverse<_pst<group>>);
static_assert(has_pst_difference<_pst_diff<>>);
static_assert(is_base_of_v<_pst<group>,_pst_diff<>>);
static_assert(is_same_v<_pst<>::node,monoid::node>);
static_assert(!is_default_constructible_v<pst_text::node>);

void check_pst_text(bool base){
    int n=base+6;_pst<pst_text> tree(n,base);mt19937 rng(404+base);
    vector<int> roots{0};vector<vector<string>> versions(1,vector<string>(n+1));
    for(int step=0;step<24;step++){
        int from=rng()%roots.size(),pos=base+rng()%(n-base+1);
        auto values=versions[from];values[pos]=string(rng()%3,char('a'+rng()%2));
        roots.push_back(tree.set(roots[from],pos,{values[pos]}));versions.push_back(values);
    }
    int nodes=tree.node_count();const auto& read=tree;
    for(int v=0;v<(int)roots.size();v++){
        for(int l=base-1;l<=n+1;l++)for(int r=l;r<=n+1;r++){
            string expected;for(int i=max(l,int(base));i<=min(r,n);i++)expected+=versions[v][i];
            CHECK(read.query(roots[v],l,r).v==expected);
        }
        auto pred=[](const pst_text::node& a){return a.v.find("ab")==string::npos;};
        for(int start=base;start<=n+1;start++){
            string acc;int right=start-1;
            while(right<n&&pred({acc+versions[v][right+1]}))acc+=versions[v][++right];
            CHECK(read.max_right(roots[v],start,pred)==right);
        }
        for(int end=base-1;end<=n;end++){
            string acc;int left=end+1;
            while(left>base&&pred({versions[v][left-1]+acc}))acc=versions[v][--left]+acc;
            CHECK(read.min_left(roots[v],end,pred)==left);
        }
    }
    CHECK(tree.node_count()==nodes);
    tree.clear(0,base);CHECK(tree.node_count()==0);CHECK(tree.query(0,-1,1).v.empty());
    auto always=[](const auto&){return true;};
    CHECK(tree.max_right(0,base,always)==0);CHECK(tree.min_left(0,0,always)==int(base));
    tree.clear(2,!base);int rt=tree.set(0,!base,{"ab"});CHECK(tree.get(rt,!base).v=="ab");CHECK(tree.get(0,!base).v.empty());
}

void check_pst_diff(bool base){
    int n=base+7;_pst_diff<> tree(n,base);mt19937 rng(909+base);
    vector<int> roots{0};vector<vector<long long>> versions(1,vector<long long>(n+1,0));
    for(int step=0;step<18;step++){
        auto values=versions.back();int pos=base+rng()%(n-base+1);values[pos]+=rng()%4;
        roots.push_back(tree.set(roots.back(),pos,{values[pos]}));versions.push_back(values);
    }
    int nodes=tree.node_count();const auto& read=tree;
    for(int a=0;a<(int)roots.size();a++)for(int b=0;b<(int)roots.size();b++){
        for(int l=base-1;l<=n+1;l++)for(int r=l;r<=n+1;r++){
            long long expected=0;for(int i=max(l,int(base));i<=min(r,n);i++)expected+=versions[b][i]-versions[a][i];
            CHECK(read.diff_query(roots[a],roots[b],l,r).v==expected);
        }
        for(int i=base;i<=n;i++)CHECK(read.diff_get(roots[a],roots[b],i).v==versions[b][i]-versions[a][i]);
        if(a>b)continue;
        for(long long cap:{0,1,4,100}){
            auto pred=[&](const auto& v){return v.v<=cap;};
            for(int start=base;start<=n+1;start++){
                long long sum=0;int right=start-1;
                while(right<n&&sum+versions[b][right+1]-versions[a][right+1]<=cap){right++;sum+=versions[b][right]-versions[a][right];}
                CHECK(read.diff_max_right(roots[a],roots[b],start,pred)==right);
            }
            for(int end=base-1;end<=n;end++){
                long long sum=0;int left=end+1;
                while(left>base&&sum+versions[b][left-1]-versions[a][left-1]<=cap){left--;sum+=versions[b][left]-versions[a][left];}
                CHECK(read.diff_min_left(roots[a],roots[b],end,pred)==left);
            }
        }
    }
    CHECK(tree.node_count()==nodes);
    CHECK(read.query(roots.back(),base,n).v==accumulate(versions.back().begin()+base,versions.back().end(),0LL));
    CHECK(read.inv({7}).v==-7);
    tree.clear(0,base);CHECK(tree.node_count()==0);CHECK(tree.diff_query(0,0,-1,1).v==0);
    auto always=[](const auto&){return true;};
    CHECK(tree.diff_max_right(0,0,base,always)==0);CHECK(tree.diff_min_left(0,0,0,always)==int(base));
    tree.clear(2,!base);int rt=tree.set(0,!base,{9});CHECK(tree.diff_get(0,rt,!base).v==9);
}

void check_pst_xor(bool base){
    int n=base+4;_pst_diff<pst_xor> tree(n,base);mt19937 rng(141+base);
    vector<int> roots{0};vector<vector<int>> versions(1,vector<int>(n+1,0));
    for(int step=0;step<12;step++){
        int from=rng()%roots.size(),pos=base+rng()%(n-base+1);auto values=versions[from];values[pos]=rng()%32;
        roots.push_back(tree.set(roots[from],pos,{values[pos]}));versions.push_back(values);
    }
    for(int a=0;a<(int)roots.size();a++)for(int b=0;b<(int)roots.size();b++)for(int l=base;l<=n;l++)for(int r=l;r<=n;r++){
        int expected=0;for(int i=l;i<=r;i++)expected^=versions[a][i]^versions[b][i];
        CHECK(tree.diff_query(roots[a],roots[b],l,r).v==expected);
    }
}

int main(){
    try{for(bool base:{false,true}){check_sum<_seg<>,monoid::node>(base);check_sum<_prop<>,action::node>(base);check_sum<_fw<>,group::node>(base);check_sorted(base);check_pst_text(base);check_pst_diff(base);check_pst_xor(base);}cout<<"PASS "<<checks<<" make-base/PST checks\n";return 0;}
    catch(const exception&e){cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
