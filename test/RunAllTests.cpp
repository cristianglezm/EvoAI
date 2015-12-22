#include <iostream>
#include <vector>
#include <thread>

struct point{
    double x;
    double y;
    point(){
        x=0;
        y=0;
    }
    point(int x, int y){
        this->x=x;
        this->y=y;
    }
};

int main(){
    std::vector<point> puntos(3);
    puntos[0] = point(-4,3);
    puntos[1] = point(2,5);
    puntos[2] = point(5,1);
    std::vector<point> calc;
    int size = puntos.size();
    std::cout << "Size: " << size << std::endl;
    for(int i=(size-1);i>=0;--i){
        calc.push_back(puntos[i]);
    }
    calc.push_back(puntos[2]);
    int res1 = 0;
    int res2 = 0;
    for(int i=0, j=1;i<(size);++i,++j){
        std::cout << calc[i].x << " * " << calc[j].y << std::endl;
        std::cout << calc[i].y <<" * "<< calc[j].x<< std::endl;
        res1 += calc[i].x * calc[j].y;
        res2 += calc[i].y * calc[j].x;
    }
    double area = 0.5 * (res1 - res2);
    std::cout << "AREA: " << area << std::endl;
}
