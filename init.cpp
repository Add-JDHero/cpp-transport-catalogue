/*#include <iostream>
#include <numeric>
#include <vector>

int pivotIndex(std::vector<int>& nums) {
    int sum = accumulate(nums.begin(), nums.end(), 0);        
    for (int i = 0, leftSum = 0; i < nums.size(); leftSum += nums[i++])
        if (2*leftSum == sum - nums[i]) return i;

    return -1;
}
int main() {
    std::vector<int> tmp{1,7,3,6,5,6};
    int t = pivotIndex(tmp);
    std::cout << t << std::endl;
}*/


/*#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>

int main() {
	int n;
    std::cin >> n;
    std::unordered_set<int> digits;
    std::vector<int> individual;

    while (n > 0) {
        int tmp;
        std::cin >> tmp;
        auto it = digits.insert(tmp);
        if (it.second) {
            individual.push_back(tmp);
        }

        --n;
    }

    for (auto& elem: individual) {
        std::cout << elem << std::endl;
    }
}
*/

/*
#include <iostream>

int main() {
    int n;
	std::cin >> n;
    int max_counter{};
	int counter{};

	while (n > 0) {
        int digit;
        std::cin >> digit;
        if (digit == 1) {
            ++counter;
        } 
        if (max_counter < counter) {
            max_counter = counter;
        }
        counter = 0;

        --n;
    }

	std::cout << max_counter << std::endl;
}*/



#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include <string>
#include <vector>
#include <utility>

#include "geo.h"
#include "log_duration.h"
/*
int main() {
	std::string str1, str2;

	str1 = "egg";
    str2 = "add";

    if (str1.size() != str2.size()) {
        std::cout << false;
        return 0;
    }

    std::unordered_map<char, int> digits1;
	std::unordered_map<char, int> digits2;
	
	for (char c: str1) {
    	++digits1[c];
    }


	for (char c: str2) {
    	++digits2[c];
    }
	
    bool flag = true;

    for (auto& [symb, count]: digits1) {
        if (digits2[symb] != count) {
            flag = false;
            break;
        }
    }

    std::cout << flag;

}*/


/*
struct point {
    double x = 0;
    double y = 0;
};

std::ostream& PrintPoint(std::ostream& out, point& p) {
    out << p.x << " " << p.y;
}

int main() {    
    point p{0,1};
    PrintPoint(std::cout, p);
}*/


/*bool isIsomorphic(std::string s, std::string t) {
    bool flag = true;
    size_t size1 = s.size();
    size_t size2 = t.size();
    if (size1 != size2) {
        return !flag;
    }

    std::unordered_map<char, char> m;
    std::unordered_set<char> chars;
    
    for (size_t i = 0; i < size1; ++i) {
        auto it = m.find(s[i]);
        if ( it != m.end() && m[s[i]] != t[i] && chars.find(m[s[i]]) == chars.end()) { return !flag; };
        m[s[i]] = t[i];
        chars.insert(t[i]);

    }

    return flag;
}


int main() {
    std::cout << std::boolalpha << isIsomorphic("badc", "baba") << std::endl;
}*/

/*struct Stop {
    Stop(std::string_view stop_name,  const Coordinates& p)
        : stop_name_(stop_name), coords_(p)
    {}

    Coordinates coords_;
    std::string stop_name_;
};

Stop ParseStop(std::istream& input) {
    std::string stop_name;
    char tmp_char{};                    
    for (char c; input >> c && c != ':';) {
        stop_name += c;
        if (c != ' ' || tmp_char != ' ') {
            tmp_char = c;
        }
    }

    double x, y;
    char commа;
    input >> x >> commа >> y;

    return Stop(std::move(stop_name), Coordinates{x, y});
}


int main() {
    ParseStop(std::cin);

    std::cout << "WTF";
}*/



/*class Test {
public:
    Test() {
        std::cout << "Ctor run\n";
    }

    Test& operator=(const Test& other) {
        if (this != &other) {
            std::swap(this->s_, other->s_);
        }

        return *this;
    }

    ~Test() {
        std::cout << "Destruction\n";
    }
private:
    std::string s_ = std::string(100000, 'a');
};

Test ReturnValue() {
    Test test;
    return test;
}

int main() {
    Test tmp = ReturnValue();
    Test t = tmp;
}*/