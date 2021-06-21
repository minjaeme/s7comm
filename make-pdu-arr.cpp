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
vector<string> split(string s, string divid);
void printPDU(vector<int> v);

// make parameter
int makeFunction (string function);
int makeSize (string size);
int makeLength (string length);
int makeDBNumber (string db);
int makeArea (string area);
int makeAddress (string addr);

int main() {
    int itemCount = 0;
    string function;
    vector<string> inp;
    int tmpValue = 0;
    
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
    for (int i=0; i<itemCount; i++) {
        cout << inp[i] << '\n';
    }
    cout << "---end---\n";
    // test end

    // step1. initPDU Id, POSCTR, reserved
    vector<int> pdu;
    pdu.push_back(0x32);
    pdu.push_back(0x01);
    pdu.push_back(0x00);
    pdu.push_back(0x00);

    // step1-2. reference, random
    // pdu.push_back(makeRandom(2));
    // pdu.push_back(makeRandom(2));
    pdu.push_back(0xff);
    pdu.push_back(0xff);

    // step2. add parameter length
    tmpValue = 2 + itemCount * 12;
    pdu.push_back(tmpValue / 0x100);
    pdu.push_back(tmpValue % 0x100);

    // step2-2. data length; 없으니 0
    pdu.push_back(0);
    pdu.push_back(0);

    // step3. parameter(1) offset0: function
    pdu.push_back(makeFunction("r"));
    
    // step3-2. parameter(2) offset1: count
    pdu.push_back(itemCount);

    // step4. make parameter
    for (int i=0; i<itemCount; i++) {
        /*
        cur[0] = area, cur[1] = size, cur[2] = addr
        */
        vector<string> cur = split(inp[i], " ");
        int index = 0;
        // step 4-2. spec && length
        pdu.push_back(0x12);
        pdu.push_back(0x0a);

        // step 4-3. syntax
        pdu.push_back(0x10);
        
        // step 4-4. size
        pdu.push_back(makeSize(cur[1]));

        // step 4-5. len
        tmpValue = makeLength(cur[1]);
        pdu.push_back(tmpValue / 0x100);
        pdu.push_back(tmpValue % 0x100);

        // step 4-6. db
        if (cur[0].find("DB") != std::string::npos) {
            // 만약 db다
            tmpValue = makeDBNumber(cur[0]);
            pdu.push_back(tmpValue / 0x100);
            pdu.push_back(tmpValue % 0x100);
        } else {
            // 아니라면
            pdu.push_back(0x00);
            pdu.push_back(0x00);
        }

        // step 4-7. area
        pdu.push_back(makeArea(cur[0]));
        
        // step 4-8. addr
        tmpValue = makeAddress(cur[2]);
        pdu.push_back(tmpValue >> 0x10);
        pdu.push_back((tmpValue >> 0x8) % 0x100);
        pdu.push_back(tmpValue % 0x100);
        // cout << resultPDU << '\n';
    }
    printPDU(pdu);
    return 0;
}

int makeFunction (string function) {
    int res = 0;
    // 현재는 read만 = 04
    if (!function.compare("r")) {
        res = 4;
    }
    return res;
}

int makeSize (string size) {
    /*
    0x01 - BIT
    0x02 - BYTE
    0x03 - CHAR
    0x04 - WORD
    0x05 - INT
    0x06 - DWORD    
    */
    int res = 0;
    if (!size.compare("X")){
        res = 0x01;
    } else if (!size.compare("B")){
        res = 0x02;
    } else if (!size.compare("W")){
        res = 0x04;
    } else if (!size.compare("D")){
        res = 0x06;
    }

    return res;
}

int makeLength (string length) {
    int res = 0;
    if (!length.compare("X")){
        res = 0x1;
    } else if (!length.compare("B")){
        res = 0x1;
    } else if (!length.compare("W")){
        res = 0x2;
    } else if (!length.compare("D")){
        res = 0x4;
    }
    return res;
}

int makeDBNumber (string db) {
    int res = 0;

    string dbNumber = "";
    for (int i=2; i<db.size(); i++) {
        // DB1.DB X 0.0 
        // DB100.DB x 0.0
        if (48 > db[i] || db[i] > 57) {
            break;
        }
        dbNumber += db[i];
    }
    res = stoi(dbNumber);
    return res;
}

int makeArea (string area) {
    int res = 0;
    if (!area.compare("P")){
        res = 0x80;
    } else if (!area.compare("I")){
        res = 0x81;
    } else if (!area.compare("Q")){
        res = 0x82;
    } else if (!area.compare("M")){
        res = 0x83;
    } else if (area.find("DB") != std::string::npos){
        res = 0x84;
    } 
    return res;
}


int makeAddress (string addr) {
    int res = 0;
    if (addr.find(".") == std::string::npos) {
        // bit가 없다면
        res = 8 * stoi(addr);
    } else {
        // 있다면
        vector<string> cur = split(addr, ".");
        res = 8 * stoi(cur[0]) + stoi(cur[1]);
    }
    return res;
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

void printPDU(vector<int> v) {
    for (int i : v) { 
        cout << setfill('0') << setw(2) << hex << i << " ";
    }
}