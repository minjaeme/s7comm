/*
@todo(21 06 18)
- 

*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <string> 
#include <cstring>
#include <random>
using namespace std;

// util
int makeRandom(int s); // 자리수
string int_to_hex_len_two (int i);
string int_to_hex_len_four (int i);
string int_to_hex_len_six (int i);
vector<string> split(string s, string divid);

// make parameter
string makeFunction (string function);
string makeSize (string size);
string makeLength (string length);
string makeDBNumber (string db);
string makeArea (string area);
string makeAddress (string addr);

int main() {

    int itemCount = 0;
    string function;
    vector<string> inp;
    
    // step0. inp 파일 넣기
    cin >> itemCount >> function;
    // step0. getline 입력 받기
    cin.ignore();
    for (int i=0; i<itemCount; i++) {
        string assignPLC;
        getline(cin, assignPLC);
        inp.push_back(assignPLC);
    }

    // info
    cout << "---inp data info---\n";
    cout << "function: ";
    cout << !function.compare("r") ? "read" : "write";
    cout << '\n';
    for (int i=0; i<itemCount; i++) {
        cout << inp[i] << '\n';
    }
    cout << "---end---\n";
    // test end

    // step1. initPDU Id, POSCTR, reserved
    string resultPDU = "32 01 00 00 ";

    // step1-2. reference, random
    resultPDU += "xx xx ";
    // resultPDU += int_to_hex_len_four(makeRandom(4));

    // step2. add parameter length
    resultPDU += int_to_hex_len_four(2 + itemCount * 12);

    // step2-2. data length; 없으니 0
    resultPDU += "00 00 ";

    // step3. parameter(1) offset0: function
    resultPDU += makeFunction("r");
    resultPDU += " ";
    
    // step3-2. parameter(2) offset1: count
    resultPDU += int_to_hex_len_two(itemCount);
    resultPDU += " ";

    // step4. make parameter
    for (int i=0; i<itemCount; i++) {
        /*
        cur[0] = area, cur[1] = size, cur[2] = addr
        */
        vector<string> cur = split(inp[i], " ");
        int index = 0;
        // step 4-2. spec && length
        resultPDU += "12 0a ";

        // step 4-3. syntax
        resultPDU += "10 ";
        
        // step 4-4. size
        resultPDU += makeSize(cur[1]);

        // step 4-5. len
        resultPDU += makeLength(cur[1]);

        // step 4-6. db
        if (cur[0].find("DB") != std::string::npos) {
            // 만약 db다
            resultPDU += makeDBNumber(cur[0]);
        } else {
            // 아니라면
            resultPDU += "00 00 ";
        }

        // step 4-7. area
        resultPDU += makeArea(cur[0]);
        resultPDU += " ";
        
        // step 4-8. addr
        resultPDU += makeAddress(cur[2]);
        // cout << resultPDU << '\n';
    }
    cout << resultPDU << '\n';
    return 0;
}

string makeFunction (string function) {
    string res = "";
    // 현재는 read만 = 04
    if (!function.compare("r")) {
        res = "04";
    }
    return res;
}

string makeSize (string size) {
    /*
    0x01 - BIT
    0x02 - BYTE
    0x03 - CHAR
    0x04 - WORD
    0x05 - INT
    0x06 - DWORD    
    */
    string res = "";
    if (!size.compare("X")){
        res = "01";
    } else if (!size.compare("B")){
        res = "02";
    } else if (!size.compare("W")){
        res = "04";
    } else if (!size.compare("D")){
        res = "06";
    }
    res += " ";
    return res;
}

string makeLength (string length) {
    string res = "";
    if (!length.compare("X")){
        res = int_to_hex_len_four(1);
    } else if (!length.compare("B")){
        res = int_to_hex_len_four(1);
    } else if (!length.compare("W")){
        res = int_to_hex_len_four(2);
    } else if (!length.compare("D")){
        res = int_to_hex_len_four(4);
    }
    return res;
}

string makeDBNumber (string db) {
    string res = "";

    string dbNumber = "";
    for (int i=2; i<db.size(); i++) {
        // DB1.DB X 0.0 
        // DB100.DB x 0.0
        if (48 > db[i] || db[i] > 57) {
            break;
        }
        dbNumber += db[i];
    }

    while(dbNumber.size() != 4) {
        dbNumber = "0" + dbNumber;
    }
    res = dbNumber.substr(0, 2) + " " 
    + dbNumber.substr(2, 2) + " ";
    return res;
}

string makeArea (string area) {
    string res = "";
    if (!area.compare("P")){
        res = "80";
    } else if (!area.compare("I")){
        res = "81";
    } else if (!area.compare("Q")){
        res = "82";
    } else if (!area.compare("M")){
        res = "83";
    } else if (area.find("DB") != std::string::npos){
        res = "84";
    } 
    return res;
}


string makeAddress (string addr) {
    string res = "";
    int tmp = 0;
    if (addr.find(".") == std::string::npos) {
        // bit가 없다면
        tmp = 8 * stoi(addr);
    } else {
        // 있다면
        vector<string> cur = split(addr, ".");
        tmp = 8 * stoi(cur[0]) + stoi(cur[1]);
    }
    res = int_to_hex_len_six(tmp);
    return res;
}

string int_to_hex_len_two(int i)
{
  stringstream stream;
  stream << setfill ('0') << setw(2) 
         << hex << i;
  return stream.str();
}

string int_to_hex_len_four(int i)
{
    stringstream stream;
    if (i < 0x100) {
        stream << "00 ";
        stream << setfill ('0') << setw(2) << hex << i;
    } else {
        int first = i >> 8;
        int last = i % 0x100;
        stream << setfill('0') << setw(2) << hex << first;
        stream << setfill('0') << setw(2) << hex << last;
    }
    stream << " ";
    return stream.str();
}

string int_to_hex_len_six(int i) {
    stringstream stream;
    if (i > 0x10000) {
        int first = i >> 16;
        int second = (i % 0x10000) >> 8;
        int last = i % 0x100;
        stream << setfill('0') << setw(2) << hex << first;
        stream << setfill('0') << setw(2) << hex << second;
        stream << setfill('0') << setw(2) << hex << last;
    } else if (i > 0x100) {
        int first = i >> 8;
        int last = i % 0x100;
        stream << "00 ";
        stream << setfill('0') << setw(2) << hex << first;
        stream << setfill('0') << setw(2) << hex << last;
    } else {
        stream << "00 00 ";
        stream << setfill ('0') << setw(2) << hex << i;
    }
    stream << " ";
    return stream.str();
}


vector<string> split(string s, string divid) {
	vector<string> v;
	char* c = strtok((char*)s.c_str(), divid.c_str());
	while (c) {
		v.push_back(c);
		c = strtok(NULL, divid.c_str());
	}
	return v;
}

int makeRandom(int s) {
    // 시드값을 얻기 위한 random_device 생성.
    std::random_device rd;

    // random_device 를 통해 난수 생성 엔진을 초기화 한다.
    std::mt19937 gen(rd());

    // 0 부터 10^s-1 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
    std::uniform_int_distribution<int> dis(0, pow(10, s)-1);
    int res = dis(gen);

    return res;
}