use colored::*;
use std::io::{BufRead, BufReader, Write};

fn main() {
    let mut bank = Bank::default();
    bank.init();
    bank.safe().show();

    loop {
        if bank.alloc() {
            bank.safe().show();
        } else {
            bank.show();
        }
    }
}

#[derive(Default)]
struct Bank {
    pcb_num: usize, // 进程数
    res_num: usize, // 资源种类数

    max: Vec<Vec<u32>>,        // 最大需求
    allocation: Vec<Vec<u32>>, // 分配数量
    need: Vec<Vec<u32>>,       // 需求数量
    available: Vec<u32>,       // 可用资源
}

impl Bank {
    /// 从标准读入中初始化
    fn init(&mut self) {
        print!("进程数> ");
        std::io::stdout().flush().unwrap();
        self.pcb_num = read_num();
        print!("资源种类数> ");
        std::io::stdout().flush().unwrap();
        self.res_num = read_num();
        // 资源初始化
        println!("最大需求矩阵.max> ");
        self.max = read_vec(self.pcb_num);
        println!("分配矩阵.allocation> ");
        self.allocation = read_vec(self.pcb_num);
        println!("可用资源.available> ");
        self.available = read_vec(1)[0].clone();
        // 更新need
        self.need = self.allocation.clone();
        for p in 0..self.pcb_num {
            for r in 0..self.res_num {
                self.need[p][r] = self.max[p][r] - self.allocation[p][r]
            }
        }
    }

    fn show(&self) {
        println!("当前可用资源:\n\t{:?}", self.available);
        println!("资源分配情况:\n\t最大需求矩阵\t\t分配矩阵\t\t需求矩阵");
        for p in 0..self.pcb_num {
            println!(
                "P{:2}.\t{:?}\t\t{:?}\t\t{:?}",
                p, self.max[p], self.allocation[p], self.need[p],
            )
        }
    }
    // 安全性判断
    fn safe(&self) -> &Self {
        let mut num = 0;
        let mut finish = [false; 10];
        let mut avai = self.available.clone();
        let mut safe_list = Vec::with_capacity(self.pcb_num);

        for _ in 0..self.pcb_num {
            // 已分配进程数
            if num == self.pcb_num {
                break;
            }
            for (id, item) in finish.iter_mut().enumerate().take(self.pcb_num) {
                if *item {
                    continue;
                } else if less_or_equal(&self.need[id], &avai) {
                    // 可用资源大于需求
                    *item = true;
                    safe_list.push(id);
                    num += 1;
                    // 释放资源
                    (0..self.res_num).for_each(|r| {
                        avai[r] += self.allocation[id][r];
                    });
                }
            }
        }
        // 判断当前系统是否安全
        for item in finish.iter().take(self.pcb_num) {
            if !item {
                println!("{}", "当前系统不安全".red());
                return self;
            }
        }
        // 输出安全序列
        println!("{}\n序列为: {:?}", "当前系统安全".green(), safe_list);
        self
    }

    fn alloc(&mut self) -> bool {
        print!("分配资源的进程编号> ");
        std::io::stdout().flush().unwrap();
        let id = read_num();
        println!("请求资源数({}个)> ", self.res_num);
        let request = read_vec(1)[0].clone();

        if less_or_equal(&self.need[id], &request) {
            println!("{}", "进程请求资源数大于所需资源数，无法分配".red());
            return false;
        } else if less_or_equal(&self.available, &request) {
            println!("{}", "进程请求资源数大于可用资源数，无法分配".red());
            return false;
        } else {
            // 资源逐一分配
            for (i, req) in request.iter().enumerate() {
                self.allocation[id][i] += req;
                self.need[id][i] -= req;
                self.available[i] -= req;
            }
        }
        println!("{}", "分配完成".green());
        true
    }
}
// 小于或等于
fn less_or_equal(a: &[u32], b: &[u32]) -> bool {
    for r in 0..a.len() {
        if a[r] > b[r] {
            return false;
        }
    }
    true
}
// 从标准控制台中读入一个数
fn read_num() -> usize {
    let mut input = String::new();
    std::io::stdin().read_line(&mut input).unwrap();
    input.split_whitespace().next().unwrap().parse().unwrap()
}
// 从标准控制台中读入一个二维数组
fn read_vec(num: usize) -> Vec<Vec<u32>> {
    let mut reader = BufReader::new(std::io::stdin());
    let mut list = Vec::with_capacity(num);
    for _ in 0..num {
        let mut buf = String::new();
        reader.read_line(&mut buf).unwrap();
        list.push(buf.trim().split(' ').map(|i| i.parse().unwrap()).collect());
    }
    list
}