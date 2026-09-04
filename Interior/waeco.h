// 文件名：waeco.h
// 作者：谢荣政; DeepseekAi
// 创建日期：2026/7/20
// 最后更新日期：2026/7/25
// 介绍：负责定义最基本的[类]Waeco和相关数据结构及一些方法
// 联动：waeco.cpp

#ifndef WAECO_MODEL_WAECO_H
#define WAECO_MODEL_WAECO_H
#include <vector>
#include <string>
#include <cmath>
#include <unordered_map>
using namespace std;
//============================第一部分——各类子结构

//一、[结构]RegionParams
//对应RegionParams.csv，记录该地区最基本特征
struct RegionParams {
    string unit_name;//地区名
    string type;//地区类型
    double SW_avail;//可用地表水(10⁸m³)
    double GW_avail;//可用地下水(10⁸m³)
    double PSW;//地表水价格(CNY/m³)
    double PGW;//地下水价格(CNY/m³)
    double EC;//电力碳排放系数(kgCO₂/kWh)
    double EG;//抽水耗电系数（kWh/m³）
    double η_field;//渠系水利用系数
    double η_canal;//田间水利用系数
    double CT;//碳税(CNY/t)
    double OP;//氧气价格(CNY/t)
    double CSO;//SO2处理成本(CNY/t)
    double CPM;//PM10处理成本(CNY/t)
    double PDC;//河道清淤价格(CNY/t)
    double STC;//污水处理成本(CNY/t)
    double WP;//水价(CNY/t)
    double PF;//肥料价格(CNY/t)
    double H_min;//生态地下水位最低阈值深度(m)
    double H_max;//潜水蒸发的极限深度(m)
    double μ;//给水度
    double φ;//降水入渗补给系数
    double ψ;//渠道渗漏系数
    double a;//潜水蒸发经验乘积系数
    double b;//潜水蒸发经验指数系数
    double WT_obj;//总调水控制目标(10⁸m³)
    double WT_max;//单月最大调水量(10⁸m³)
    double POP;//人口(万人)
    double FD;//人均粮需(kg)
    double Y_min;//最低粮食产量(10⁴t)
    double TAC;//总面积(10⁴ha)
    double TAE;//生态总面积上限(10⁴ha)
    double FA;//分区面积(km²)
};

//二、[结构]MeteoData & Meteo
//对应MeteoData.csv，记录某地区某月气象特征
//由于受“域名+时间”双键锁定特性，此处定义的数据结构是内部单元
//设想：以域名为键值，寻求到一个按时间排序的数组MeteoData[]
struct MeteoData {
    int year;//起始年份
    int month;//起始月份
    double T;//空气温度(°C)
    double P;//日降水量(mm)
    double U2;//离地2米处风速(m/s)
    double Rn;//冠层表面静辐射(MJ/m²/d)
    double G;//土壤热通量(MJ/m²/d)
    double es;//饱和蒸汽压(kPa)
    double ea;//实际蒸汽压(kPa)
    double Δ;//饱和蒸汽压曲线斜率
    double E_Phi20;//Φ20蒸发皿测得蒸发量(mm)
    double γ;//干湿计常数(kPA/℃)
    double H_0;//初始潜水位深度(m)
};

//三、[结构]CropData
//对应CropData.csv，记录作物的特征
struct CropData {
    string crop_name;//作物名称
    double Kc;//作物系数
    double HI;//作物收获指数
    double BIO;//生物量(kg/ha)
    double SWP;//标准化水分生产力(kg/ha)
    double D;//生长天数(Day)
    double α;//蒸腾量与蒸散发量之比
    double CS;//碳吸收率
    double R;//根茎比
    double WC;//水分含量
    double Price;//价格(CNY/kg)
    double CL;//单位面积农田劳动力成本(CNR/公顷)
    double CF;//单位面积农田化肥成本(CNY/公顷)
    double CP;//单位面积农药成本(CNY/公顷)
    double CAF;//单位面积农田面积农膜成本(CNY/公顷)
    double CI;//灌溉成本(CNY/公顷)
    double CAM;//农用机械成本(CNY/公顷)
    double UF;//单位面积施加化肥量(kg/ha)
    double UP;//单位面积施农药量(kg/ha)
    double UAF;//单位面积用农膜量(kg/ha)
    double AMC;//农业机械单位面积的碳排放系数(kgCO2/ha)
    double TC;//耕作单位面积的碳排放系数(kgCO2/ha)
};

//四、[结构]CropParams
//对应CropParams.csv，记录各区域作物种植情况
//由于受“域名+作物”双键锁定特性，此处定义的数据结构是内部单元
//设想：创建一个哈希表CropParams[]，键值为"<域名>|<作物名>这一组合键"
struct CropParams {
    double AC_min;//种植面积下限(10⁴公顷)
    double AC_max;//种植面积上限(10⁴公顷)
    double AC_now;//当前种植面积(10⁴公顷)
    double AC_plan;//计划种植面积(10⁴公顷)(输出对象)
};

//五、[结构]VegetationData
//对应VegetationData.csv，记录植被的特征
struct VegetationData {
    string vege_name;
    double Kp;//植被系数
    double WPE;//水净化能力
    double APM;//PM10吸收能力
    double ASO;//SO2吸收能力
    double ACO;//CO2吸收能力
    double F;//土壤元素与肥料比值(CNY/t)
    double SEC;//土壤元素含量(CNY/t)
    double SEI;//土壤侵蚀指数
    double λ;//水涵养系数
    double WV;//水资源保护价值(元/吨)
    double RCV;//休闲文化价值(元/公顷)
    double BV;//生物多样性价值(元/公顷)
};


//六、[结构]VegetationParams
//对应VegetationParams.csv，记录作物的特征
//由于受“域名+作物”双键锁定特性，此处定义的数据结构是内部单元
//设想：创建一个哈希表CropParams[]，键值为"<域名>|<作物名>这一组合键"
struct VegetationParams {
    double AE_min;//种植面积下限(10⁴公顷)
    double AE_max;//种植面积上限(10⁴公顷)
    double AE_now;//当前种植面积(10⁴公顷)
    double AE_plan;//计划种植面积(10⁴公顷)(输出对象)
};

//七、[结构]ReservoirData
//对应ReservoirData.csv，记录水库的特征
struct ReservoirData {
    string reservoir_name;//水库名称
    double Vd;//死库容(10⁸m³)
    double Va;//有效库容(10⁸m³)
    double V_min;//储水容量下限(10⁸m³)
    double V_max;//储水容量上限(10⁸m³)
    double β;//库容系数
    double ρ;//水损失率(%)
};

//八、[结构]ReservoirParams
//对应ReservoirParams.csv，记录水库的特征
//由于受“库名+时间”双键锁定特性，此处定义的数据结构是内部单元
//由于受“域名+时间”双键锁定特性，此处定义的数据结构是内部单元
//设想：以域名为键值，寻求到一个按时间排序的数组MeteoData[]
struct ReservoirParams {
    int year;//年份
    int month;//月份
    double W;//来水量
};

//九、[结构]CropWaterAllocation & CropWater
//对应CropWaterAllocation.csv，存储各地区各作物水资源分配
//由于受“域名+作物类型+时间”三键锁定特性，此处定义的数据结构是内部单元
//设想：[i][j][t],内部存实际的时期，其余的交给对应的数组
struct CropWaterAllocation {
    int year;//年份
    int month;//月份
    double WA_min;//用水需求下限(10⁸m³)
    double WA_max;//用水需求上限(10⁸m³)
    double SW;//地表水供应(10⁸m³)
    double GW;//地下水供应(10⁸m³)
};

//十、[结构]VegetationWaterAllocation & VegetationWater
//对应VegetationWaterAllocation.csv，存储各地区各作物水资源分配
//由于受“域名+作物类型+时间”三键锁定特性，此处定义的数据结构是内部单元
struct VegetationWaterAllocation {
    int year;//起始年份
    int month;//起始月份
    double WE_max;//用水需求上限(10⁸m³)
    double WE_min;//用水需求下限(10⁸m³)
    double SW;//地表水供应(10⁸m³)
    double GW;//地下水供应(10⁸m³)
};

//十一、[结构]IterationResults
//对应IterationResults.csv，记录预算中迭代结果
struct IterationResults {
    int Times;//迭代次数
    double F_CD;//协调发展程度
    double F_C;//协调程度
    double F_D;//发展程度
    double fGWB;//地下水平衡
    double fCY;//作物总产量
    double fEWS;//生态满意度
    double H;//迭代的潜水位深度(m)
    double ε;//收敛差值
};

//============================第二部分——主类的建立
class Waeco {
    private:
    string file_path;//文件路径
    double eta;//偏好数[0,1]
    double epsilon_threshold = 0.001;   // 收敛阈值
    int max_iterations = 100;           // 最大迭代次数

    vector<RegionParams> Region_Params_Array;//对应RegionParams.csv，记录各地区最基本特征
    vector<CropData> Crop_Data_Array;//对应CropData.csv，记录各作物的特征
    vector<VegetationData> Vegetation_Data_Array;//对应VegetationData.csv，记录植被的特征
    vector<ReservoirData> Reservoir_Data_Array;//对应ReservoirData.csv，记录水库的特征

    unordered_map<string, int> Region_Map;   // "<域名>" → <在Region_Params_Array的索引>
    unordered_map<string, int> Crop_Map;     // "<作物名>" → <在Crop_Data_Array的索引>
    unordered_map<string, int> Veg_Map;      // "<植被名>" → <在Vegetation_Data_Array的索引>
    unordered_map<string, int> Res_Map;      // "<水库名>" → <在Reservoir_Data_Array的索引>

    // ---------- 带组合键的二维数据（用哈希表模拟） ----------
    // 键："区域名|作物名"；值：CropParams
    unordered_map<string, CropParams> crop_params_map;
    // 键："区域名|植被名"；值：VegetationParams
    unordered_map<string, VegetationParams> veg_params_map;

    // ---------- 时间序列数据（直接存 vector，附带年月） ----------
    // 每个区域一个 vector，存储该区域所有月份的气象数据
    vector<vector<MeteoData>> meteo_data; //[<区域序号i>][<时期序号t>]
    // 每个水库一个 vector
    vector<vector<ReservoirParams>> reservoir_params;//[<水库序号n>][<时期序号t>]

    // 每个区域一个 vector，内部再按作物分
    vector<vector<vector<CropWaterAllocation>>> crop_water;      // [<区域序号i>][<作物编号j>][<时期序号t>]
    vector<vector<vector<VegetationWaterAllocation>>> veg_water; // [<区域序号i>][<植被编号k>][<时期序号t>]

    // ---------- 迭代结果 ----------
    vector<IterationResults> iteration_results;

    // ---------- 私有辅助函数（查找与初始化） ----------
    string make_key(const string& a, const string& b) const;
    vector<string> split_key(const string& obj) const;
    void build_maps();

public:
    Waeco();
    explicit Waeco(const string& path,double preference=0.5);
    ~Waeco();

    // ---------- 数据加载接口 ----------
    void load_all_data(const string& base_path);
    void load_region_params(const string& filename);
    void load_meteo_data(const string& filename);
    void load_crop_data(const string& filename);
    void load_crop_params(const string& filename);
    void load_vegetation_data(const string& filename);
    void load_vegetation_params(const string& filename);
    void load_reservoir_data(const string& filename);
    void load_reservoir_params(const string& filename);
    void load_crop_water_allocation(const string& filename);
    void load_veg_water_allocation(const string& filename);

    // ---------- 核心计算接口（以后你填充） ----------
    double calc_ET0(const MeteoData& m) const;
    double calc_EP(double P) const;  // 新增
    double calc_ETc(const MeteoData& meteo, double Kc) const;   // 新增
    double calc_F_CD(double f,double g,double h,double preference) const;
    double calc_F_C(double f,double g,double h) const;
    double calc_F_D(double f,double g,double h) const;
    vector<double> reform_vector(vector<double> obj) const;
    void run_iteration();
    void set_eta(double eta);
    void set_epsilon(double eps);
    void set_max_iterations(int maxiter);

    // ---------- 保存内容 ----------
    // void save();
    void save_iteration_results(const string& filename) const;
    void save_crop_params(const string& filename);
    void save_veg_params(const string& filename);
    void save_crop_water(const string& filename);
    void save_veg_water(const string& filename);

    // ---------- 调试接口 ----------
    // void print_summary() const;

    bool data_loaded_check() const;
};

#endif //WAECO_MODEL_WAECO_H
