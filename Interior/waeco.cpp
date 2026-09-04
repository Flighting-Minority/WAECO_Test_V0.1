// 文件名：waeco.cpp
// 作者：谢荣政; DeepseekAi
// 创建日期：2026/7/20
// 最后更新日期：2026/7/24
// 介绍：实现[类]Waeco的各种方法
// 联动：waeco.h
#include "waeco.h"
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <iomanip> // for std::setprecision
#include <ranges>
using namespace std;

//============================第一部分——辅助函数
//一、形成用于哈希表的键
string Waeco::make_key(const string& a, const string& b) const {
    return a + "|" + b;
}

//二、将哈希表的键拆开
vector<string> Waeco::split_key(const string &obj) const {
    int pos = obj.find('|');
    if (pos == string::npos)
        return {};
    return {obj.substr(0, pos), obj.substr(pos + 1)};
}

//三、建立各个哈希表("<名称>"->"<原排序索引>")
void Waeco::build_maps() {
    //1.清空旧映射
    Region_Map.clear();
    Crop_Map.clear();
    Veg_Map.clear();
    Res_Map.clear();

    //2.建立索引映射
    for (int i = 0; i < static_cast<int>(Region_Params_Array.size()); ++i)
        Region_Map[Region_Params_Array[i].unit_name] = i;
    for (int i = 0; i < static_cast<int>(Crop_Data_Array.size()); ++i)
        Crop_Map[Crop_Data_Array[i].crop_name] = i;
    for (int i = 0; i < static_cast<int>(Vegetation_Data_Array.size()); ++i)
        Veg_Map[Vegetation_Data_Array[i].vege_name] = i;
    for (int i = 0; i < static_cast<int>(Reservoir_Data_Array.size()); ++i)
        Res_Map[Reservoir_Data_Array[i].reservoir_name] = i;

}

//四、去除字符串首尾空格，规整化输入
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

//五、将CSV行解析为字符串向量vector<string> tokens
vector<string> split_csv_line(const string& line) {
    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

//六、检测是否成功载入数据
bool Waeco::data_loaded_check() const {
    return !Region_Params_Array.empty() && !Crop_Data_Array.empty();
}

//============================第二部分——数据加载函数
//一、读取RegionParams.csv
void Waeco::load_region_params(const string& filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return;}
    //2.读取有效的内容
    string line;
    getline(file, line); // 跳过表头
    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 32) {
            cerr << "警告：RegionParams 行字段数不足，跳过: " << line << endl;
            continue;
        }
    //3.整理读取的内容
        RegionParams r;
        r.unit_name = tokens[0];
        r.type = tokens[1];
        r.SW_avail = stod(tokens[2]);
        r.GW_avail = stod(tokens[3]);
        r.PSW = stod(tokens[4]);
        r.PGW = stod(tokens[5]);
        r.EC = stod(tokens[6]);
        r.EG = stod(tokens[7]);
        r.η_field = stod(tokens[8]);
        r.η_canal = stod(tokens[9]);
        r.CT = stod(tokens[10]);
        r.OP = stod(tokens[11]);
        r.CSO = stod(tokens[12]);
        r.CPM = stod(tokens[13]);
        r.PDC = stod(tokens[14]);
        r.STC = stod(tokens[15]);
        r.WP = stod(tokens[16]);
        r.PF = stod(tokens[17]);
        r.H_min = stod(tokens[18]);
        r.H_max = stod(tokens[19]);
        r.μ = stod(tokens[20]);
        r.φ = stod(tokens[21]);
        r.ψ = stod(tokens[22]);
        r.a = stod(tokens[23]);
        r.b = stod(tokens[24]);
        r.WT_obj = stod(tokens[25]);
        r.WT_max = stod(tokens[26]);
        r.POP = stod(tokens[27]);
        r.FD = stod(tokens[28]);
        r.Y_min = stod(tokens[29]);
        r.TAC = stod(tokens[30]);
        r.TAE = stod(tokens[31]);
        r.FA = stod(tokens[32]);

        Region_Params_Array.push_back(r);
    }
    //4.更新，善后
    file.close();
    build_maps();
}

//二、读取CropData.csv
void Waeco::load_crop_data(const string& filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return; }
    //2.读取有效的内容
    string line;
    getline(file, line); // 跳过表头
    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 22){
            cerr << "警告：CropData 行字段数不足，跳过: " << line << endl;
            continue;}
        //3.整理读取的内容
        CropData c;
        c.crop_name = tokens[0];
        c.Kc = stod(tokens[1]);
        c.HI = stod(tokens[2]);
        c.BIO = stod(tokens[3]);
        c.SWP = stod(tokens[4]);
        c.D = stod(tokens[5]);
        c.α = stod(tokens[6]);
        c.CS = stod(tokens[7]);
        c.R = stod(tokens[8]);
        c.WC = stod(tokens[9]);
        c.Price = stod(tokens[10]);
        c.CL = stod(tokens[11]);
        c.CF = stod(tokens[12]);
        c.CP = stod(tokens[13]);
        c.CAF = stod(tokens[14]);
        c.CI = stod(tokens[15]);
        c.CAM = stod(tokens[16]);
        c.UF = stod(tokens[17]);
        c.UP = stod(tokens[18]);
        c.UAF = stod(tokens[19]);
        c.AMC = stod(tokens[20]);
        c.TC = stod(tokens[21]);
        Crop_Data_Array.push_back(c);
    }
    file.close();
    build_maps();
}

//三、读取CropParams.csv
void Waeco::load_crop_params(const string& filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return;}
    //2.读取有效的内容
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 5) {
            cerr << "警告：CropData 行字段数不足，跳过: " << line << endl;
            continue;
        }
        //3.整理读取的内容
        string key = make_key(tokens[0], tokens[1]); // 区域名|作物名
        CropParams cp{};
        cp.AC_min = stod(tokens[2]);
        cp.AC_max = stod(tokens[3]);
        cp.AC_now = stod(tokens[4]);
        cp.AC_plan = 0.0; // 初始化为0
        crop_params_map[key] = cp;
    }
    file.close();
}

//四、读取VegetationData.csv
void Waeco::load_vegetation_data(const string& filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return;}
    //2.读取有效的内容
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 13) {
            cerr << "警告：VegetationData 行字段数不足，跳过: " << line << endl;
            continue;
        }
        //3.整理读取的内容
        VegetationData v;
        v.vege_name = tokens[0];
        v.Kp = stod(tokens[1]);
        v.WPE = stod(tokens[2]);
        v.APM = stod(tokens[3]);
        v.ASO = stod(tokens[4]);
        v.ACO = stod(tokens[5]);
        v.F = stod(tokens[6]);
        v.SEC = stod(tokens[7]);
        v.SEI = stod(tokens[8]);
        v.λ = stod(tokens[9]);
        v.WV = stod(tokens[10]);
        v.RCV = stod(tokens[11]);
        v.BV = stod(tokens[12]);
        Vegetation_Data_Array.push_back(v);
    }
    file.close();
    build_maps();
}

//五、读取VegetationParams.csv
void Waeco::load_vegetation_params(const string& filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return;}
    //2.读取有效的内容
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 5) {
            cerr << "警告：VegetationParams 行字段数不足，跳过: " << line << endl;
            continue;
        }
        //3.整理读取的内容
        string key = make_key(tokens[0], tokens[1]);
        VegetationParams vp{};
        vp.AE_min = stod(tokens[2]);
        vp.AE_max = stod(tokens[3]);
        vp.AE_now = stod(tokens[4]);
        vp.AE_plan = 0.0;
        veg_params_map[key] = vp;
    }
    file.close();
}

//六、读取MeteoData.csv
void Waeco::load_meteo_data(const string& filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return;}
    //2.读取有效的内容
    string line;
    getline(file, line);
    // 假设 meteo_data 已初始化为与 Region_Params_Array 大小相同
    meteo_data.clear();
    meteo_data.resize(Region_Params_Array.size());

    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 14)
            continue;
        //3.整理读取的内容
        //(1)确认该行存在——该区域再前面(Regions)里存在
        string region_name = tokens[0];
        // 去除首尾空格
        region_name.erase(0, region_name.find_first_not_of(" \t\n\r"));
        region_name.erase(region_name.find_last_not_of(" \t\n\r") + 1);

        if (Region_Map.find(region_name) == Region_Map.end()) {
            cerr << "警告：MeteoData 中的区域 " << region_name << " 在 RegionParams 中未定义，已跳过。" << endl;
            continue;}
        int idx = Region_Map[region_name];

        MeteoData m{};
        m.year = stoi(tokens[1]);
        m.month = stoi(tokens[2]);
        m.T = stod(tokens[3]);
        m.P = stod(tokens[4]);
        m.U2 = stod(tokens[5]);
        m.Rn = stod(tokens[6]);
        m.G = stod(tokens[7]);
        m.es = stod(tokens[8]);
        m.ea = stod(tokens[9]);
        m.Δ = stod(tokens[10]);
        m.E_Phi20 = stod(tokens[11]);
        m.γ = stod(tokens[12]);
        m.H_0 = stod(tokens[13]);
        meteo_data[idx].push_back(m);
    }
    file.close();
}

//七、读取ReservoirData.csv
void Waeco::load_reservoir_data(const string &filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return;}
    //2.读取有效的内容
    string line;
    getline(file, line); // 跳过表头
    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 7) {
            cerr << "警告：ReservoirData 行字段数不足，跳过: " << line << endl;
            continue;
        }
        //3.整理读取的内容
        ReservoirData r;
        r.reservoir_name = tokens[0];
        r.Vd=stod(tokens[1]);
        r.Va=stod(tokens[2]);
        r.V_min=stod(tokens[3]);
        r.V_max=stod(tokens[4]);
        r.β=stod(tokens[5]);
        r.ρ=stod(tokens[6]);

        Reservoir_Data_Array.push_back(r);
    }
    //4.更新，善后
    file.close();
    build_maps();
}

//八、读取ReservoirParams.csv
void Waeco::load_reservoir_params(const string &filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return;}
    //2.读取有效的内容
    string line;
    getline(file, line);
    // 假设 reservoir_params 已初始化为与 Reservoir_Data_Array 大小相同(同水库量)
    reservoir_params.clear();
    reservoir_params.resize(Reservoir_Data_Array.size());
    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 4)
            continue;
        //3.整理读取的内容
        //(1)确认该行存在——该区域在前面(Reservoir)里存在
        string reservoir_name = tokens[0];
        if (Res_Map.find(reservoir_name) == Res_Map.end()) {
            cerr << "警告：ReserviorParams 中的水库 " << reservoir_name << " 在 ReservoirData 中未定义，已跳过。" << endl;
            continue;}
        int idx = Res_Map[reservoir_name];

        ReservoirParams m{};
        m.year = stoi(tokens[1]);
        m.month = stoi(tokens[2]);
        m.W=stod(tokens[3]);

        reservoir_params[idx].push_back(m);
    }
    file.close();
}

//九、读取CropWaterAllocation.csv
void Waeco::load_crop_water_allocation(const string &filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return;}
    //2.初始化该三维vector
    int region_count = Region_Params_Array.size();
    int crop_count = Crop_Data_Array.size();
    if (crop_water.size() != region_count) {
        crop_water.clear();
        crop_water.resize(region_count);
    }
    for (int i = 0; i < region_count; ++i) {
        if (crop_water[i].size() != crop_count) {
            crop_water[i].clear();
            crop_water[i].resize(crop_count);
        }
    }
    //3.读取有效的内容
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 7)
            continue;
        //4.整理读取的内容
        //(1)索引i
        string region_name = tokens[0];
        if (Region_Map.find(region_name) == Region_Map.end()) {
            cerr << "警告：CropWaterAllocation 中的区域 " << region_name << " 在 RegionParams 中未定义，已跳过。" << endl;
            continue;}
        int idx_r = Region_Map[region_name];
        //(2)索引j
        string crop_name = tokens[1];
        if (Crop_Map.find(crop_name) == Crop_Map.end()) {
            cerr << "警告：CropWaterAllocation 中的区域 " << region_name << " 在 CropData 中未定义，已跳过。" << endl;
            continue;}
        int idx_c = Crop_Map[crop_name];
        //(3)读取内容
        CropWaterAllocation m{};
        m.year = stoi(tokens[2]);
        m.month = stoi(tokens[3]);
        m.WA_min=stod(tokens[4]);
        m.WA_max=stod(tokens[5]);
        m.SW=0;
        m.GW=0;
        crop_water[idx_r][idx_c].push_back(m);
    }
    file.close();
}

//十、读取VegetationWaterAllocation.csv
void Waeco::load_veg_water_allocation(const string &filename) {
    //1.鉴定其是否存在
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return;}
    //2.初始化该三维vector
    int region_count = Region_Params_Array.size();
    int veg_count = Vegetation_Data_Array.size();
    if (veg_water.size() != region_count) {
        veg_water.clear();
        veg_water.resize(region_count);
    }
    for (int i = 0; i < region_count; ++i) {
        if (veg_water[i].size() != veg_count) {
            veg_water[i].clear();
            veg_water[i].resize(veg_count);
        }
    }
    //3.读取有效的内容
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty())
            continue;
        auto tokens = split_csv_line(line);
        if (tokens.size() < 7)
            continue;
        //4.整理读取的内容
        //(1)索引i
        string region_name = tokens[0];
        if (Region_Map.find(region_name) == Region_Map.end()) {
            cerr << "警告：VegetationWaterAllocation 中的区域 " << region_name << " 在 RegionParams 中未定义，已跳过。" << endl;
            continue;}
        int idx_r = Region_Map[region_name];
        //(2)索引j
        string veg_name = tokens[1];
        if (Veg_Map.find(veg_name) == Veg_Map.end()) {
            cerr << "警告：VegetationWaterAllocation 中的植被 " << region_name << " 在 VegetationData 中未定义，已跳过。" << endl;
            continue;}
        //(3)读取内容
        int idx_v = Veg_Map[veg_name];
        VegetationWaterAllocation m{};
        m.year = stoi(tokens[2]);   // ✅ 读取年份
        m.month = stoi(tokens[3]);  // ✅ 读取月份
        m.WE_min=stod(tokens[4]);
        m.WE_max=stod(tokens[5]);
        m.SW=0;
        m.GW=0;
        veg_water[idx_r][idx_v].push_back(m);
    }

    file.close();
}

//十一、所有文件读取函数
void Waeco::load_all_data(const string &base_path) {
    cout << "正在从 " << base_path << " 加载数据..." << endl;

    // 检查目录是否存在
    ifstream dir_check(base_path + "RegionParams.csv");
    if (!dir_check.is_open()) {
        cerr << "❌ 错误：无法打开 " << base_path << "RegionParams.csv" << endl;
        cerr << "   请确保 Python 程序已先将 Excel 转换为 CSV 文件" << endl;
        return;
    }
    dir_check.close();

    load_region_params(base_path+"RegionParams.csv");
    load_meteo_data(base_path+"MeteoData.csv");
    load_crop_data(base_path+"CropData.csv");
    load_vegetation_data(base_path+"VegetationData.csv");
    load_reservoir_data(base_path+"ReservoirData.csv");
    load_crop_params(base_path+"CropParams.csv");
    load_vegetation_params(base_path+"VegetationParams.csv");
    load_reservoir_params(base_path+"ReservoirParams.csv");
    load_crop_water_allocation(base_path+"CropWaterAllocation.csv");
    load_veg_water_allocation(base_path+"VegetationWaterAllocation.csv");

    cout << "✅ 数据加载完成！" << endl;
    cout << "   区域数: " << Region_Params_Array.size() << endl;
    cout << "   作物数: " << Crop_Data_Array.size() << endl;
    cout << "   植被数: " << Vegetation_Data_Array.size() << endl;
}

//============================第三部分——计算函数
//一、计算【参考蒸腾量】ET0
double Waeco::calc_ET0(const MeteoData &m) const {
    return (0.408*(m.Rn-m.G)+m.γ*900/(m.T+273)*m.U2*(m.es-m.ea))/(m.Δ+m.γ*(1+m.U2*0.34));
}

//二、计算【协调度】F_C
double Waeco::calc_F_C(double f, double g, double h) const {
    return 3*(f*g+f*h+g*h)/pow((f+g+h),2);
}

//三、计算【发展度】F_D
double Waeco::calc_F_D(double f, double g, double h) const {
    // 理想最优状态点 B = (1, 1, 1)
    // 向量 OB = (1, 1, 1)
    double OB_length = sqrt(3.0);

    // 向量 OP = (f, g, h)
    double OP_length = sqrt(f*f + g*g + h*h);

    // 点积 OP · OB = f + g + h
    double dot_product = f + g + h;

    // cos(theta) = (OP · OB) / (|OP| * |OB|)
    double cos_theta = dot_product / (OP_length * OB_length + 1e-9);

    // F_D = (|OP| * cos_theta) / |OB| = dot_product / (|OB|^2)
    double F_D = dot_product / 3.0;  // 因为 |OB|^2 = 3

    // 截断到 [0, 1]
    if (F_D < 0.0) F_D = 0.0;
    if (F_D > 1.0) F_D = 1.0;

    return F_D;
}

//四、计算【协调发展度】F_CD
double Waeco::calc_F_CD(double f, double g, double h, double preference) const {
    return pow(calc_F_C(f,g,h),preference)*pow(calc_F_D(f,g,h),1-preference);
}

//五、计算【有效降雨量】EP
double Waeco::calc_EP(double P) const {
    if (P <=8.3 && P >=0 )
        return P*(4017-0.2*P)/4.17;
    if (P <=0 )
        return 0;
    return 4.17+0.1;
}

//六、计算【作物蒸散发量】ETc
double Waeco::calc_ETc(const MeteoData& meteo, double Kc) const {
    double ET0 = calc_ET0(meteo);
    return Kc * ET0;}

//七、【归一化】向量计算
//[*注意*]:这个方法【没调用过】。因为我的“归一化”是向量的归一化，不是各个数据的归一化
vector<double> Waeco::reform_vector(vector<double> obj) const {
    int i;
    double mod=0;
    for (i=0;i<obj.size();i++) {
        mod+=obj[i]*obj[i];
    }
    mod=sqrt(mod);
    vector<double>temp;
    for (i=0;i<obj.size();i++) {
        temp.push_back(obj[i]/mod);
    }
    return temp;
}

//八、重新配置【倾向参数】
void Waeco::set_eta(double eta) {
    this->eta = max(min(eta,1.0),0.0);
}

//九、重新配置【收敛极限】
void Waeco::set_epsilon(double eps) { epsilon_threshold = eps; }

//十、重新配置【最大迭代次数】
void Waeco::set_max_iterations(int max_iter) { max_iterations = max_iter; }

//十一、WAECO 模型主迭代引擎
void Waeco::run_iteration() {
    cout << "\n===== 开始 WAECO 模型迭代 =====" << endl;

    // ---------- 参数设置 ----------
    int iteration = 0;                  //迭代次数
    double delta = 999.0;               //

    // 获取区域数量和月份数量（动态计算）
    int num_regions = Region_Params_Array.size();
    int num_months = 0;
    for (int i = 0; i < num_regions; ++i) {
        if ((int)meteo_data[i].size() > num_months) {
            num_months = (int)meteo_data[i].size();
        }
    }
    if (num_months == 0) {
        cerr << "错误：没有气象数据！" << endl;
        return;
    }
    cout << "  区域数: " << num_regions << ", 月份数: " << num_months << endl;

    // ---------- 初始化或重置迭代结果 ----------
    iteration_results.clear();

    // ---------- 主循环 ----------
    while (iteration < max_iterations && delta > epsilon_threshold) {
        iteration++;
        cout << "  [迭代 " << iteration << "] 进行计算..." << endl;

        // --- 步骤 1: 对每个区域、每个月份计算核心指标 ---
        double f_gwb_sum = 0.0;     //
        double f_cy_sum = 0.0;      //
        double f_ews_sum = 0.0;     //

        IterationResults iter_result{};
        iter_result.Times = iteration;
        iter_result.H = 0.0;

        // --- 1a. 对每个区域进行循环 ---
        for (int i = 0; i < num_regions; i++) {
            const auto& region = Region_Params_Array[i];

            if (i >= (int)meteo_data.size() || meteo_data[i].empty()) {
                cerr << "警告: 区域 " << region.unit_name << " 无气象数据，跳过" << endl;
                continue;
            }

            double region_SW_used = 0.0;
            double region_GW_used = 0.0;
            double region_crop_yield = 0.0;
            double region_eco_satisfaction = 0.0;
            double region_water_balance = 0.0;

            // --- 1b. 对每个月进行循环 ---
            for (int t = 0; t < (int)meteo_data[i].size() && t < num_months; t++) {
                const auto& meteo = meteo_data[i][t];

                // ---- 计算有效降雨 EP (公式 28) ----
                double EP = calc_EP(meteo.P);

                // ---- 计算参考蒸散发 ET0 (公式 27) ----
                double ET0 = calc_ET0(meteo);

                // ---- 1) 农业模块 ----
                double monthly_crop_yield = 0.0;
                double monthly_crop_benefit = 0.0;
                double monthly_carbon_sink = 0.0;

                for (int j = 0; j < (int)Crop_Data_Array.size(); j++) {
                    const auto& crop = Crop_Data_Array[j];

                    string cp_key = make_key(region.unit_name, crop.crop_name);
                    auto cp_it = crop_params_map.find(cp_key);
                    if (cp_it == crop_params_map.end()) {
                        continue;
                    }
                    double area = cp_it->second.AC_now;

                    // 计算作物蒸散发 ETc (公式 26)
                    double ETc = calc_ETc(meteo, crop.Kc);

                    // ===== 修复问题1：用 ETc - EP 作为实际耗水量 =====
                    // 净灌溉需水量 IR = ETc - EP (公式 25)
                    double IR = max(0.0, ETc - EP);

                    // 实际蒸腾量 T = α * ETc (公式 13)
                    double T_actual = crop.α * ETc;

                    // 计算作物产量 (公式 11-14)
                    // 注意：产量与蒸腾量/参考蒸腾量的比值有关
                    double yield = area * crop.HI * crop.BIO * crop.SWP * (T_actual / (ET0 + 1e-9));
                    monthly_crop_yield += yield;

                    // 计算净经济效益 (公式 19-21)
                    double revenue = yield * crop.Price;
                    // 灌溉成本与净灌溉需水量 IR 相关
                    double irrigation_cost = IR * area * region.PSW * 0.01; // 粗略换算
                    double cost = area * (crop.CL + crop.CF + crop.CP + crop.CAF + crop.CI + crop.CAM)
                                + irrigation_cost;
                    monthly_crop_benefit += (revenue - cost);

                    // 计算碳汇 (公式 22-24)
                    double sequestration = area * crop.BIO * crop.CS * (1 + crop.R) * (1 - crop.WC);
                    double emission = area * (crop.UF * crop.AMC + crop.UP * 0.0 + crop.UAF * 0.0 + crop.TC);
                    monthly_carbon_sink += (sequestration - emission);

                    // ---- 记录该作物的水资源分配 ----
                    if (i < (int)crop_water.size() && j < (int)crop_water[i].size()) {
                        while ((int)crop_water[i][j].size() <= t) {
                            CropWaterAllocation dummy{0, 0, 0, 0, 0, 0};
                            dummy.year = meteo.year;
                            dummy.month = meteo.month;
                            crop_water[i][j].push_back(dummy);
                        }
                        // 实际灌溉需水量 IR 作为 SW + GW 的总和
                        // 这里的假设？
                        crop_water[i][j][t].SW = IR * area * 0.6 / 100.0;  // 假设60%来自地表水
                        crop_water[i][j][t].GW = IR * area * 0.4 / 100.0;  // 40%来自地下水
                        crop_water[i][j][t].WA_min = IR * area * 0.5 / 100.0;
                        crop_water[i][j][t].WA_max = IR * area * 1.2 / 100.0;
                    }
                }

                region_crop_yield += monthly_crop_yield;

                // ---- 2) 生态模块 ----
                double monthly_eco_water = 0.0;
                double monthly_eco_satisfaction = 0.0;
                int veg_count_for_avg = 0;

                for (int k = 0; k < (int)Vegetation_Data_Array.size(); k++) {
                    const auto& veg = Vegetation_Data_Array[k];

                    string vp_key = make_key(region.unit_name, veg.vege_name);
                    auto vp_it = veg_params_map.find(vp_key);
                    if (vp_it == veg_params_map.end()) {
                        continue;
                    }
                    double veg_area = vp_it->second.AE_now;

                    // 计算潜水蒸发 (公式 30-31)
                    double Wg = region.a * pow(max(0.0, 1 - meteo.H_0 / (region.H_max + 1e-9)), region.b) * meteo.E_Phi20;
                    double W_eco = veg_area * Wg * veg.Kp * 1e-3;
                    monthly_eco_water += W_eco;

                    // ===== 直接用 t 索引访问 =====
                    double WE_min = 0.02, WE_max = 0.5;  // 默认值
                    bool has_data = false;

                    if (i < (int)veg_water.size() && k < (int)veg_water[i].size()) {
                        if ((int)veg_water[i][k].size() > t) {
                            double read_min = veg_water[i][k][t].WE_min;
                            double read_max = veg_water[i][k][t].WE_max;
                            if (read_max > 0.001) {
                                WE_min = read_min;
                                WE_max = read_max;
                                has_data = true;
                            }
                        }
                    }

                    if (!has_data) {
                        cout << "植被调用常用值 (i=" << i << ", k=" << k << ", t=" << t << ")" << endl;
                        WE_min = 0.02;
                        WE_max = 0.5;
                    }

                    if (WE_max > WE_min + 1e-9) {
                        double satisfaction = (W_eco - WE_min) / (WE_max - WE_min);
                        satisfaction = max(0.0, min(1.0, satisfaction));
                        monthly_eco_satisfaction += satisfaction;
                        veg_count_for_avg++;
                    }

                    // ---- 记录该植被的水资源分配 ----
                    if (i < (int)veg_water.size() && k < (int)veg_water[i].size()) {
                        while ((int)veg_water[i][k].size() <= t) {
                            VegetationWaterAllocation dummy{0, 0, 0, 0, 0, 0};
                            dummy.year = meteo.year;
                            dummy.month = meteo.month;
                            veg_water[i][k].push_back(dummy);
                        }
                        veg_water[i][k][t].SW = meteo.P * 0.02 * veg_area / 100.0;
                        veg_water[i][k][t].GW = region.GW_avail * 0.01 / max(1, num_regions) / max(1, num_months);
                        // 更新 WE_min 和 WE_max（从当前记录读取或保留已有值）
                        if (veg_water[i][k][t].WE_min < 0.01) {
                            veg_water[i][k][t].WE_min = 0.02;
                        }
                        if (veg_water[i][k][t].WE_max < 0.01) {
                            veg_water[i][k][t].WE_max = 0.5;
                        }
                    }
                }

                if (veg_count_for_avg > 0) {
                    region_eco_satisfaction += monthly_eco_satisfaction / veg_count_for_avg;
                }

                // ---- 3) 水资源模块（地下水平衡） ----
                double total_water_use = region_SW_used + region_GW_used + monthly_eco_water;
                double monthly_water_balance = (region.SW_avail + region.GW_avail) / max(1, num_months) - total_water_use;
                region_water_balance += monthly_water_balance;
            }

            // ---- 汇总该区域的指标 ----
            f_gwb_sum += region_water_balance / max(1, num_months);
            f_cy_sum += region_crop_yield / max(1, num_months);
            f_ews_sum += region_eco_satisfaction / max(1, num_months);
        }

        // --- 步骤 2: 归一化三大指标 ---
        double f = f_gwb_sum / (num_regions + 1e-9);
        double h = f_ews_sum / (num_regions + 1e-9);

        // 在 run_iteration() 的步骤 2 中
        double max_possible_yield = 0.0;
        for (const auto& crop : Crop_Data_Array) {
            max_possible_yield += crop.BIO * crop.HI * crop.SWP * 12; // 粗略估算
        }
        double g = f_cy_sum / max_possible_yield;

        f = max(0.0, min(1.0, f));
        g = max(0.0, min(1.0, g));
        h = max(0.0, min(1.0, h));

        cout << "    f_GWB=" << f << ", f_CY=" << g << ", f_EWS=" << h << endl;

        // --- 步骤 3: 计算 F_C, F_D, F_CD ---
        double F_C = calc_F_C(f, g, h);
        double F_D = calc_F_D(f, g, h);
        double F_CD = calc_F_CD(f, g, h, eta);

        // --- 步骤 4: 收敛判断 ---
        if (iteration > 1 && !iteration_results.empty()) {
            double prev_F_CD = iteration_results.back().F_CD;
            delta = fabs(F_CD - prev_F_CD);
        } else {
            delta = 999.0;
        }

        iter_result.F_CD = F_CD;
        iter_result.F_C = F_C;
        iter_result.F_D = F_D;
        iter_result.fGWB = f_gwb_sum;
        iter_result.fCY = f_cy_sum;
        iter_result.fEWS = f_ews_sum;
        iter_result.ε = delta;
        iter_result.H = 0.0;

        iteration_results.push_back(iter_result);

        cout << "    F_CD = " << F_CD
             << ", F_C = " << F_C
             << ", F_D = " << F_D
             << ", Δ = " << delta << endl;

        // --- 步骤 5: 更新决策变量 ---
        double step_size = 0.1 / (1 + iteration * 0.05);

        for (auto& kv : crop_params_map) {
            auto& cp = kv.second;
            double target = (cp.AC_min + cp.AC_max) / 2.0;
            double change = step_size * (target - cp.AC_now);
            cp.AC_now += change;
            cp.AC_now = max(cp.AC_min, min(cp.AC_max, cp.AC_now));
            cp.AC_plan = cp.AC_now;
        }

        for (auto& kv : veg_params_map) {
            auto& vp = kv.second;
            double target = (vp.AE_min + vp.AE_max) / 2.0;
            double change = step_size * (target - vp.AE_now);
            vp.AE_now += change;
            vp.AE_now = max(vp.AE_min, min(vp.AE_max, vp.AE_now));
            vp.AE_plan = vp.AE_now;
        }
    }

    cout << "===== 迭代结束，共 " << iteration << " 次，最终 F_CD = "
         << (iteration_results.empty() ? 0.0 : iteration_results.back().F_CD) << " =====" << endl;
}

//=============================第四部分——写入函数
//一、写入迭代结果函数
void Waeco::save_iteration_results(const string& filename) const {
    //1.检测输出对象存在
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法写入结果文件 " << filename << endl;
        return;}
    //2.输出内容
    file << "Times/迭代次数,F_CD/协调发展程度,F_C/协调程度,F_D/发展程度,fGWB/地下水平衡,fCY/作物总产量,fEWS/生态满意度,H/迭代的潜水位深度(m),ε/收敛差值\n";
    for (const auto& res : iteration_results) {
        file << res.Times << ","
             << res.F_CD << ","
             << res.F_C << ","
             << res.F_D << ","
             << res.fGWB << ","
             << res.fCY << ","
             << res.fEWS << ","
             << res.H << ","
             << res.ε << "\n";
    }
    file.close();
}

//二、写入CropParams.csv结果的函数
void Waeco::save_crop_params(const string &filename) {
    //1.检测输出对象存在
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法写入结果文件 " << filename << endl;
        return;}
    //2.输出内容
    file << "unit_name/域名,crop_name/作物名,AC_min/种植面积下限(10⁴公顷),AC_max/种植面积上限(10⁴公顷),AC_now/当前种植面积(10⁴公顷),AC_plan/计划种植面积(10⁴公顷)\n";
    for (const auto& res : crop_params_map) {
        vector<string> tokens=split_key(res.first);
        file << tokens[0] << ","
             << tokens[1] << ","
             << res.second.AC_min << ","
             << res.second.AC_max << ","
             << res.second.AC_now << ","
             << res.second.AC_plan << "\n";
    }
    file.close();
}

//三、写入VegetationParams.csv结果的函数
void Waeco::save_veg_params(const string &filename) {
    //1.检测输出对象存在
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法写入结果文件 " << filename << endl;
        return;}
    //2.输出内容
    file << "unit_name/域名,veg_name/植被名,AE_min/最小植被面积(10⁴公顷),AE_max/最大植被面积(10⁴公顷),AE_now/当前植被面积(10⁴公顷),AE_plan/计划植被面积(10⁴公顷)\n";
    for (const auto& res : veg_params_map) {
        vector<string> tokens=split_key(res.first);
        file << tokens[0] << ","
             << tokens[1] << ","
             << res.second.AE_min << ","
             << res.second.AE_max << ","
             << res.second.AE_now << ","
             << res.second.AE_plan << "\n";
    }
    file.close();
}

//四、写入CropWaterAllocation.csv结果的函数
void Waeco::save_crop_water(const string &filename) {
    //1.检测输出对象存在
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法写入结果文件 " << filename << endl;
        return;}
    //2.输出内容
    int i,j,t;
    file << "unit_name/域名,crop_name/作物名,Year/年,Month/月,WA_min/用水需求下限(10⁸m³),WA_max/用水需求上限(10⁸m³),SW/地表水供应(10⁸m³),GW/地下水供应(10⁸m³)\n";
    for (i=0;i<crop_water.size();i++) {
        for (j=0;j<crop_water[i].size();j++) {
            for (t=0;t<crop_water[i][j].size();t++) {
                file <<  Region_Params_Array[i].unit_name << ","
                << Crop_Data_Array[j].crop_name << ","
                << crop_water[i][j][t].year << ","
                << crop_water[i][j][t].month << ","
                << crop_water[i][j][t].WA_min << ","
                << crop_water[i][j][t].WA_max << ","
                << crop_water[i][j][t].SW << ","
                << crop_water[i][j][t].GW << "\n";
            }
        }
    }
    file.close();
}

//五、写入VegetationWaterAllocation.csv结果的函数
void Waeco::save_veg_water(const string &filename) {
    //1.检测输出对象存在
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法写入结果文件 " << filename << endl;
        return;}
    //2.输出内容
    int i,j,t;
    file << "unit_name/域名,veg_name/植被名,Year/年,Month/月,WE_min/用水需求下限(10⁸m³),WE_max/用水需求上限(10⁸m³),SW/地表水供应(10⁸m³),GW/地下水供应(10⁸m³)\n";
    for (i=0;i<veg_water.size();i++) {
        for (j=0;j<veg_water[i].size();j++) {
            for (t=0;t<veg_water[i][j].size();t++) {
                file << Region_Params_Array[i].unit_name << ","
                << Vegetation_Data_Array[j].vege_name << ","
                << veg_water[i][j][t].year << ","
                << veg_water[i][j][t].month << ","
                << veg_water[i][j][t].WE_min << ","
                << veg_water[i][j][t].WE_max << ","
                << veg_water[i][j][t].SW << ","
                << veg_water[i][j][t].GW << "\n";
            }
        }
    }
    file.close();
}

//=============================第五部分——类生命周期函数
//一、无参构造
Waeco::Waeco() {  }

//二、含参构造
Waeco::Waeco(const string& filename,double preference):file_path(filename) {this->eta=preference;}

//三、无参析构
Waeco::~Waeco() {}