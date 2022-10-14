void main()
{
   int a;
   int*p;
   int**pp;
  
   a = 100;
   p = &a;
   pp = &p;

   //int c;
  // c = **pp;
   //p = 1000;

    print(**pp);
   // print(c);
    return;
}
/*
Expected output:
100
*/