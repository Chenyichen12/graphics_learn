

#include <cassert>
#include <iostream>
#include <string>

struct Point {
  double x;
  double y;

  Point operator-(const Point &p) const { return {x - p.x, y - p.y}; }
};
// 两个向量的叉积

double cross(Point a, Point b) { return a.x * b.y - a.y * b.x; }

int main() {

    // 逆时针方向，向量两两的叉积都应该大于等于0，说明是凸多边形
  Point pTrueList[6] = {{0, 0}, {1, 0}, {3, 3}, {2, 4}, {1, 5}, {0, 2}};
  for (int i = 0; i < 5; i++) {
    Point a = pTrueList[i];
    Point b = pTrueList[(i + 1)];
    Point ab = b - a;

    Point c = pTrueList[(i + 2) % 6];
    Point bc = c - b;

    auto res = cross(ab, bc);
    assert(res >= 0);
  }
  std::wcout.imbue(std::locale("chs"));
  std::wcout << std::wstring(L"是凸多边形") << '\n';

  //   std::cout<<<<std::endl;

  Point pFalseList[6] = {{0, 0}, {1, 0}, {3, 3}, {2, 4}, 
  // [2，4] [1，1] [0，2] 这三个点不是凸多边形，res会小于0
  {1, 1}, {0, 2}};
  for (int i = 0; i < 5; i++) {
    Point a = pFalseList[i];
    Point b = pFalseList[(i + 1)];
    Point ab = b - a;

    Point c = pFalseList[(i + 2) % 6];
    Point bc = c - b;

    auto res = cross(ab, bc);
    if (res < 0) {
      std::wcout << L"不是凸多边形" << '\n';
      break;
    }
  }
}