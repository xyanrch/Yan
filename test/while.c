void main(){ 
 int i;
  i=1;
  while (i <= 10) {
    int a;
    a = 1;
    print(i);
    i= i + a;
  }
  
  do 
  {
      print(i);
      i = i-2;
      
  }while(i >= 0);

  return;
}
/*
Expected output:
1
2
3
4
5
6
7
8
9
10
11
9
7
5
3
1
*/