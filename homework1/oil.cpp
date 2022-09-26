#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Oil   //油瓶类
{
public:
    int ten;
    int seven;
    int three;
public:
    Oil(){}
    Oil(int x, int y, int z):ten(x),seven(y),three(z){}
    ~Oil(){}
};

class State //状态类，用于回溯
{
public:
    Oil* cur_oil;
    int parent;
public:
    State(){}
    State(Oil* o,int p):cur_oil(o),parent(p){}
    ~State(){}
};

class Solution{
    public:
    vector<State*> is_visited;

    void initialize(int ten, int seven, int three){
        Oil* ori_oil = new Oil(ten,seven,three);
        State* ori_state = new State(ori_oil,-1);
        waiting_q.push(ori_state);
        return;
    }

    bool in_visited_vector(State* s){
        for(int i = 0; i < is_visited.size(); i++){
            if(s->cur_oil->ten == is_visited[i]->cur_oil->ten
              && s->cur_oil->seven == is_visited[i]->cur_oil->seven){
                return true;
            }
        }
        return false;
    }

    void BFS(){
        while(!waiting_q.empty()){
            cur_state = waiting_q.front();
            waiting_q.pop();
            
            cout << cur_state->cur_oil->ten << " " 
                << cur_state->cur_oil->seven << " " 
                << cur_state->cur_oil->three << " " 
                << cur_state->parent << endl;
            times++;    //记录检查状态次数

            if(cur_state->cur_oil->ten == 5 
              && cur_state->cur_oil->seven == 5){   //已找到目标状态
                is_visited.push_back(cur_state);
                break;
            }
            else if(in_visited_vector(cur_state)){  //已经遍历过该状态，跳过
                continue;
            }
            else{   //找到未遍历过的新状态，对新状态根据规则进行变化，并将新产生的状态加入等待队列
                if(cur_state->cur_oil->seven < 7){  //七两瓶不满时装满
                    Oil* newOil = new Oil(3-cur_state->cur_oil->three, 7, cur_state->cur_oil->three);
                    State* newState = new State(newOil,count);
                    waiting_q.push(newState);
                }
                if(cur_state->cur_oil->three < 3){  //三两瓶不满时装满
                    Oil* newOil = new Oil(7-cur_state->cur_oil->seven, cur_state->cur_oil->seven, 3);
                    State* newState = new State(newOil,count);
                    waiting_q.push(newState);
                }
                if(cur_state->cur_oil->seven > 0){  //七两瓶不空时倒空
                    Oil* newOil = new Oil(10-cur_state->cur_oil->three, 0, cur_state->cur_oil->three);
                    State* newState = new State(newOil,count);
                    waiting_q.push(newState);
                }
                if(cur_state->cur_oil->three > 0){  //三两瓶不空时倒空
                    Oil* newOil = new Oil(10-cur_state->cur_oil->seven, cur_state->cur_oil->seven, 0);
                    State* newState = new State(newOil,count);
                    waiting_q.push(newState);
                }
                if(cur_state->cur_oil->seven > 0    //七两瓶中油全倒入三两瓶
                  && cur_state->cur_oil->seven + cur_state->cur_oil->three <= 3){
                    Oil* newOil = new Oil(cur_state->cur_oil->ten, 0, cur_state->cur_oil->seven+cur_state->cur_oil->three);
                    State* newState = new State(newOil,count);
                    waiting_q.push(newState);
                }
                if(cur_state->cur_oil->three > 0    //三两瓶中油全倒入七两瓶
                  && cur_state->cur_oil->seven + cur_state->cur_oil->three <= 7){
                    Oil* newOil = new Oil(cur_state->cur_oil->ten, cur_state->cur_oil->seven+cur_state->cur_oil->three, 0);
                    State* newState = new State(newOil,count);
                    waiting_q.push(newState);
                }
                if(cur_state->cur_oil->three < 3    //用七两瓶中油装满三两瓶
                  && cur_state->cur_oil->seven + cur_state->cur_oil->three >= 3){
                    Oil* newOil = new Oil(cur_state->cur_oil->ten, cur_state->cur_oil->seven+cur_state->cur_oil->three-3, 3);
                    State* newState = new State(newOil,count);
                    waiting_q.push(newState);
                }
                if(cur_state->cur_oil->seven < 7    //用三两瓶中油装满七两瓶
                  && cur_state->cur_oil->seven + cur_state->cur_oil->three >= 7){
                    Oil* newOil = new Oil(cur_state->cur_oil->ten, 7, cur_state->cur_oil->seven+cur_state->cur_oil->three-7);
                    State* newState = new State(newOil,count);
                    waiting_q.push(newState);
                }
                is_visited.push_back(cur_state);
                count++;
            }
        }
        return;
    }

    void printResult(){
        vector<State*> trace;
        trace.push_back(is_visited.back());
        int idx = 0;
        while(trace[idx]->parent != -1){
            trace.push_back(is_visited[trace[idx]->parent]);
            idx++;
        }
        cout << endl;
        cout << "Times for checking state:\t" << times << endl;
        cout << "Result:" << endl;
        for(int i = trace.size()-1; i>=0; i--){
            cout << trace[i]->cur_oil->ten << " " << trace[i]->cur_oil->seven << " " << trace[i]->cur_oil->three << endl;
        }
        cout << endl;
        return;
    }

    private:
    int count = 0;
    int times = 0;
    State* cur_state;
    queue<State*> waiting_q;
};

int main(){
    Solution sol;
    sol.initialize(10,0,0);
    sol.BFS();
    sol.printResult();
    return 0;
}