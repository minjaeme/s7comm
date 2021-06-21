
## 명령어
```
g++ make-pdu.cpp && cat inp | ./a.out >> out
```

## input 파일 형식

```
I X 4.0
M B 2
M D 3
DB1.DB X 0.0
DB7.DB W 6
DB10.DB D 6

메모리 어드레스 바이트.비트

I: 입력 메모리
Q: 출력 메모리
M: 비트 메모리
DB: 데이터 블록

X: bit
B: byte
W: word
D: Dword
```



## output 예제
```
---inp data info---
function: 1
DB1.DB X 0.0
DB1.DB X 0.4
DB5.DB X 0.0
DB5.DB B 4
DB7.DB W 6
DB10.DB D 6
---end---
32 01 00 00 ff ff 00 4a 00 00|04 06|12 0a 10 01|00 01 00 01|84 00 00 00|12 0a 10 01|00 01 00 01|84 00 00 04|12 0a 10 01|00 01 00 05|84 00 00 00|12 0a 10 02|00 01 00 05|84 00 00 20|12 0a 10 04|00 02 00 07|84 00 00 30|12 0a 10 06|00 04 00 10|84 00 00 30|
```
