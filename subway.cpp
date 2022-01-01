#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <queue>
using namespace std;

class Station{
private:
    string name;
    vector<string> from_station;
    vector<string> from_line;
public:
    Station():name("???") {}
    void set_name(string& _name);
    void setLastStation(string& _name);
    void setFromLine(string& _line);
    string get_name() ;
    vector<string>& get_station() ;
    vector<string>& get_line() ;
};

void Station::set_name(string& _name) {name=_name;}

void Station::setLastStation(string& _name) { from_station.push_back(_name);}

void Station::setFromLine(string& _line) { from_line.push_back(_line); }

string Station::get_name() { return name; }

vector<string>& Station::get_station() { return from_station; }

vector<string>& Station::get_line() { return from_line; }


typedef vector<pair<string,string>> InfoList;

class Subway{
private:
    map<string,vector<pair<string,string>>> info;
    map<string,Station> stations;
    void init(string& name);
    void to_way(map<string,bool> &check, string& from, string& dest);
    void get_way(vector<pair<string,string>>& route, string& start, string& dest,string&);
    void out(string& start, string& dest);
public:
    map<string, vector<string>> lines; 
    int MAX;
    bool success;
    Subway(string& filename );
    void find_way(string& start, string& dest);
    void print();
    void print_map();
};

int main(){
    string data = "subway.txt";
    Subway g(data);
    while(true)
    {
        puts("");
        int op;
        cout << "输入1查询路线" << endl;
        cout << "输入2查询地铁线路" << endl;
        cout << "输入3退出" << endl;
        cin >> op;
        if(op == 1)
        {
            string start, dest;
            cout << "请输入始发站" << endl;
            cin >> start ; //= "珠江新城";
            cout << "请输入终点站" << endl;
            cin >> dest ;//= "鱼珠";
            cout << "始发站:" << start << endl;
            cout << "终点站:" << dest << endl;
            g.find_way(start,dest);
        }
        else if (op == 2)
        {
            string line;
            cout << "您想查看几号线" << endl;
            cin >> line;
            if (0 == g.lines[line].size()) {
                cout << "无此线路信息\n" << endl;
                continue;
            }
            for (auto& i: g.lines[line])
            {
                cout << i << ' ';
            }
            puts("");
        } else break;
    }
}

Subway::Subway(string &filename)
{
    init(filename);
    MAX = 1000;
    success = false;
}

void Subway::find_way(string &start, string &dest)
{
    map<string,bool> mp;
    mp[start] = true;
    to_way(mp,start,dest);

    out(start,dest);
}

void Subway::init(string &name)
{

    ifstream ifs(name);
    string str;

    while (getline(ifs,str))
    {
        if ((str.size() == 1 && isdigit(str[0])) || str=="3_1" || str=="APM" || str == "21" || str == "13")
        {
            Station last, curr;
            string temp;
            while (getline(ifs,temp))
            {
                if (temp.empty())
                    break;
                lines[str].push_back(temp);
                curr.set_name(temp);
                stations[temp] = curr;
                if (last.get_name() != "???")
                {
                    info[temp].push_back({last.get_name(), str});
                    info[last.get_name()].push_back({temp, str});
                }
                last = curr;
            }
        }
    }
}

void Subway::out(string &start, string &dest)
{
    Station curr = stations[dest];
    for(int k = 0; k<curr.get_station().size(); k++)
    {
        string now_line = curr.get_line()[k];
        InfoList route;
        route.push_back({dest, now_line});
        get_way(route,start,curr.get_station()[k],now_line);
    }
}

void Subway::get_way(vector<pair<string,string>>& route,string& start,string& dest,string& line)
{
    if (success)return;
    if (dest == start)
    {
        route.emplace_back(make_pair(start,route.back().second));
        if (route.size() > MAX) return;
        string tmp;
        puts("");
        for(int i = route.size()-1; i >= 0; i--)
        {
            if (i == route.size()-1)
            {
                cout << route[i].second  << "号线" << ": " <<route[i].first;
                tmp = route[i].second;
            }
            else if (tmp != route[i].second)
            {
                tmp = route[i].second;
                cout << endl << "换乘 " << tmp << " 号线" << ": " << route[i].first;
            }else{
                cout << " -> " << route[i].first;
            }
        }
        unsigned long long count = route.size();
        puts("");
        cout << endl << "共 " << count << " 站" <<endl;
        success = true;
    }
    else
    {
        Station curr = stations[dest];
        for(int k = 0; k < curr.get_station().size(); k ++)
        {
            string new_dest = curr.get_station()[k];
            string new_line = curr.get_line()[k];
            if (new_line != line) continue;
            route.push_back({curr.get_name(), new_line});
            get_way(route, start,new_dest,new_line);
            route.pop_back();
        }
        for(int k = 0; k < curr.get_station().size(); k ++)
        {
            string new_dest = curr.get_station()[k];
            string new_line = curr.get_line()[k];
            if (new_line == line) continue;
            route.push_back({curr.get_name(), new_line});
            get_way(route, start,new_dest,new_line);
            route.pop_back();
        }
    }
}

void Subway:: to_way(map<string,bool> &check, string& from, string& dest)
{
    queue< pair<string,string> > queue;
    check[from] = true;
    Station curr = stations[from];

    for(auto entry : info[from])
    {
        stations[entry.first].setLastStation(from);
        stations[entry.first].setFromLine(entry.second);
        queue.push(entry);
    }

    bool reach = false;
    while (!queue.empty())
    {
        auto now_pair = queue.front();
        queue.pop();
        string tmp;
        tmp = now_pair.first;
        check[tmp] = true;

        if(tmp == dest)
            reach = true;

        for(auto entry:info[tmp])
        {
            if(!check[entry.first] && !reach)
            {
                stations[entry.first].setLastStation(tmp);
                stations[entry.first].setFromLine(entry.second);
                queue.push(entry);
            }
        }
    }
}
