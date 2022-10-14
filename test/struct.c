
struct Point
{
    int x;
    int y;
};
int main()
{
   struct Point p;
   p.x = 100;
   p.y = 200;
    print(p.x+p.y);
   print(p.x);
   struct Point s; //TD
   s.x = 116;

   print(p.x+s.x);


    return 0;
}
/*
Expected output:
300
100
216
*/