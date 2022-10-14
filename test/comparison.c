int main()
{
int x;
x= 7 < 9;  print (x);
x= 7 <= 9;  print (x);
x= 7 != 9; print (x);
x= 7 == 7;  print (x);
x= 7 >= 7;  print (x);
x= 7 <= 7;  print (x);
x= 9 > 7;   print (x);
x= 9 >= 7;  print (x);
x= 9 != 7;  print (x);

x= 7 > 9;  print (x);
x= 7 >= 9;  print (x);
x= 7 != 9; print (x);
x= 7 == 7;  print (x);
x= 7 >= 7;  print (x);
x= 7 <= 7;  print (x);
x= 9 > 7;   print (x);
x= 9 >= 7;  print (x);
x= 9 == 7;  print (x);
x=  -1 >= 1;  print (x);
x=7<7;print(x);
x = 7>7;print(x);
return;

}
/*
Expected output:
1
1
1
1
1
1
1
1
1
0
0
1
1
1
1
1
1
0
0
0
0
*/