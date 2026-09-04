// 文件名：main.cpp
// 作者：谢荣政; DeepseekAi
// 创建日期：2026/7/21
// 最后更新日期：2026/7/25
// 介绍：作为核心计算引擎，接收参数 → 计算 → 输出结果
// 联动：waeco.h, waeco.cpp

#include "waeco.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

// 打印使用说明
void print_usage() {
    cout << "用法: WAECO_model.exe [选项]" << endl;
    cout << "选项:" << endl;
    cout << "  --eta <值>     设置偏好因子 (0~1)，默认 0.5" << endl;
    cout << "  --epsilon <值> 设置收敛阈值，默认 0.001" << endl;
    cout << "  --maxiter <值> 设置最大迭代次数，默认 100" << endl;
    cout << "  --help         显示此帮助信息" << endl;
    cout << "示例: WAECO_model.exe --eta 0.6 --epsilon 0.0001" << endl;
}

int main(int argc, char* argv[]) {
    // ===== 解析命令行参数 =====
    double eta = 0.5;
    double epsilon = 0.001;
    int max_iterations = 100;
    bool show_help = false;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            show_help = true;
            break;
        } else if (arg == "--eta" && i + 1 < argc) {
            try {
                eta = stod(argv[++i]);
                if (eta < 0.0 || eta > 1.0) {
                    cerr << "⚠️ eta 超出 [0,1] 范围，自动修正为 0.5" << endl;
                    eta = 0.5;
                }
            } catch (...) {
                cerr << "⚠️ eta 格式错误，使用默认值 0.5" << endl;
                eta = 0.5;
            }
        } else if (arg == "--epsilon" && i + 1 < argc) {
            try {
                epsilon = stod(argv[++i]);
                if (epsilon < 0.0) {
                    cerr << "⚠️ epsilon 不能为负数，使用默认值 0.001" << endl;
                    epsilon = 0.001;
                }
            } catch (...) {
                cerr << "⚠️ epsilon 格式错误，使用默认值 0.001" << endl;
                epsilon = 0.001;
            }
        } else if (arg == "--maxiter" && i + 1 < argc) {
            try {
                max_iterations = stoi(argv[++i]);
                if (max_iterations < 1) {
                    cerr << "⚠️ maxiter 必须大于 0，使用默认值 100" << endl;
                    max_iterations = 100;
                }
            } catch (...) {
                cerr << "⚠️ maxiter 格式错误，使用默认值 100" << endl;
                max_iterations = 100;
            }
        } else {
            cerr << "⚠️ 未知参数: " << arg << endl;
            print_usage();
            return 1;
        }
    }

    if (show_help) {
        print_usage();
        return 0;
    }

    // ===== 核心计算 =====
    cout << "=== WAECO 核心计算引擎 ===" << endl;
    cout << "参数: eta=" << eta << ", epsilon=" << epsilon << ", maxiter=" << max_iterations << endl;

    Waeco model;
    model.set_eta(eta);
    model.set_epsilon(epsilon);
    model.set_max_iterations(max_iterations);

    // 从 Data/input/ 读取数据
    string base_path = "Data/input/";
    model.load_all_data(base_path);

    // 检查数据是否加载成功
    if (!model.data_loaded_check()) {
        cerr << "❌ 数据加载失败，请确保 Data/input/ 目录下有正确的 CSV 文件" << endl;
        return 1;
    }

    // 运行迭代
    cout << "\n🚀 开始迭代计算..." << endl;
    model.run_iteration();

    // 保存结果
    string output_path = "Data/output/";
    model.save_iteration_results(output_path + "IterationResults.csv");
    model.save_crop_params(output_path + "CropParams_Result.csv");
    model.save_veg_params(output_path + "VegetationParams_Result.csv");
    model.save_crop_water(output_path + "CropWater_Result.csv");
    model.save_veg_water(output_path + "VegetationWater_Result.csv");

    cout << "✅ 计算完成！结果已保存到 Data/output/" << endl;
    return 0;
}