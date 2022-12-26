#include <iostream>

void input();
void show();
void safe();
bool banker();
bool compare(int m[], int n[]);

int p = 5;                      // 进程数
int r = 3;                      // 资源种类数
int maxs[10][10] = {{0}};       // 最大需求
int allocation[10][10] = {{0}}; // 分配数量
int need[10][10] = {{0}};       // 需求数量
int available[10] = {0};        // 可用资源

// 矩阵初始化
void input() {
  int i, j;
  // maxs
  std::cout << "请输入最大需求矩阵maxs\n";
  for (i = 0; i < p; i++)
    for (j = 0; j < r; j++)
      std::cin >> maxs[i][j];
  // allocation
  std::cout << "请输入分配矩阵allocation\n";
  for (i = 0; i < p; i++)
    for (j = 0; j < r; j++)
      std::cin >> allocation[i][j];
  // available
  std::cout << "请输入可用资源向量available\n";
  for (i = 0; i < r; i++)
    std::cin >> available[i];
  // 更新need矩阵
  for (i = 0; i < p; i++)
    for (j = 0; j < r; j++)
      need[i][j] = maxs[i][j] - allocation[i][j];
}
// 安全性检查
void safe() {
  int num = 0;               // 已试分配进程个数
  bool finish[10] = {false}; // 试分配进程完成情况
  int safeList[10] = {0};    // 安全序列
  int res[10];               // 可分配资源
  // 初始化res
  for (int i = 0; i < r; ++i)
    res[i] = available[i];

  for (int k = 0; k < p; ++k) {
    if (num == p) // 所有进程完成分配
      break;
    // 试分配进程
    for (int id = 0; id < p; ++id) {
      if (finish[id]) { // 当前进程以分配
        continue;
      } else if (compare(need[id], res)) {
        // need <= res
        finish[id] = true;  // 当前进程以分配
        safeList[num] = id; // 安全序列添加当前进程id
        num++;
        // 资源释放
        for (int _r = 0; _r < r; ++_r)
          res[_r] += allocation[id][_r];
      }
    }
  }
  // 判断分配情况
  for (int i = 0; i < p; ++i) {
    if (!finish[i]) { // 存在进程未分配
      std::cout << "当前系统不安全" << std::endl;
      return;
    }
  }
  // 输出安全序列
  std::cout << "安全序列为: ";
  for (int i = 0; i < p - 1; ++i)
    std::cout << safeList[i] << "->";
  std::cout << safeList[p - 1] << std::endl;
}

// 资源分配
bool banker() {
  int id = 0;            // 请求进程id
  int request[10] = {0}; // 请求资源数量

  std::cout << "请求资源编号:";
  std::cin >> id;
  std::cout << "资源数量:" << std::endl;
  for (int i = 0; i < r; ++i)
    std::cin >> request[i];

  if (compare(need[id], request)) {
    std::cout << "请求资源数大于所需资源数，分配失败" << std::endl;
    return false;
  } else if (compare(available, request)) {
    std::cout << "请求资源数大于可用资源数，分配失败" << std::endl;
    return false;
  } else {
    // 满足条件，资源分配
    for (int _r = 0; _r < r; _r++) {
      allocation[id][_r] += request[_r];
      need[id][_r] -= request[_r];
      available[_r] -= request[_r];
    }
    std::cout << "分配结束" << std::endl;
  }
  return true;
}
// m<=n时返回true
bool compare(int m[], int n[]) {
  int i;
  for (i = 0; i < r; i++)
    if (m[i] > n[i])
      return false;
  return true;
}
// 当前资源分配情况
void show() {
  std::cout << "可用资源: ";
  for (int i = 0; i < r; ++i)
    std::cout << available[i] << " ";

  std::cout << "\n资源分配情况\nmaxs\t\tallocation\tneed" << std::endl;
  for (int _p = 0; _p < p; ++_p) {
    for (int _r = 0; _r < r; ++_r)
      std::cout << maxs[_p][_r] << " ";
    std::cout << "\t\t";
    for (int _r = 0; _r < r; ++_r)
      std::cout << allocation[_p][_r] << " ";
    std::cout << "\t\t";
    for (int _r = 0; _r < r; ++_r)
      std::cout << need[_p][_r] << " ";
    std::cout << std::endl;
  }
}
int main() {
  int choice = 1;
  std::cout << "进程数: ";
  std::cin >> p;
  std::cout << "资源种类数: ";
  std::cin >> r;

  input();
  safe();
  show();

  std::cout << "是否为进程分配资源(0:false/1:true): ";
  std::cin >> choice;
  while (choice) {
    if (banker()) {
      safe();
      show();
    } else {
      show();
    }
    std::cout << "是否为进程分配资源(0:false/1:true): ";
    std::cin >> choice;
  }
}