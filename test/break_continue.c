void main() {
  int x;
  x = 0;
  while (x < 100) {
    if (x == 5) { x = x + 2; continue; }
    print(x);
    if (x == 14) { break; }
    x = x + 1;
  }
  return ;
}
/*
Expected output:
0
1
2
3
4
7
8
9
10
11
12
13
14
*/