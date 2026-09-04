# Python/translator.py
# 文件名：translator.py
# 作者：谢荣政; DeepseekAi
# 创建日期：2026/7/22
# 最后更新日期：2026/7/25
# 介绍：负责.csv(便于程序阅读)和.xls格式(便于人类阅读)互转
# 联动：entry.py
import numpy as np
import pandas as pd
import os
from datetime import datetime, timedelta
import random
from pathlib import Path

class Translator:
    """负责 Excel 和 CSV 之间的转换"""

    # 定义所有需要的 Sheet 名称
    SHEET_NAMES = [
        "RegionParams",
        "MeteoData",
        "CropData",
        "CropParams",
        "VegetationData",
        "VegetationParams",
        "ReservoirData",
        "ReservoirParams",
        "CropWaterAllocation",
        "VegetationWaterAllocation"
    ]

    def __init__(self, data_dir="Data", docs_dir="Docs"):
        self.data_dir = Path(data_dir)
        self.docs_dir = Path(docs_dir)
        self.input_dir = self.data_dir / "input"
        self.output_dir = self.data_dir / "output"

        # 确保目录存在
        self.input_dir.mkdir(parents=True, exist_ok=True)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        (self.docs_dir / "Input").mkdir(parents=True, exist_ok=True)
        (self.docs_dir / "Output").mkdir(parents=True, exist_ok=True)

    def excel_to_csv(self, excel_path: str) -> bool:
        """将 Excel 文件转换为多个 CSV 文件"""
        try:
            excel_file = pd.ExcelFile(excel_path)
            for sheet_name in self.SHEET_NAMES:
                if sheet_name in excel_file.sheet_names:
                    df = pd.read_excel(excel_file, sheet_name=sheet_name)
                    csv_path = self.input_dir / f"{sheet_name}.csv"
                    df.to_csv(csv_path, index=False, encoding='utf-8-sig')
                    print(f"  ✅ 已转换: {sheet_name}")
                else:
                    print(f"  ⚠️ 警告: 工作表 {sheet_name} 不存在，跳过")
            return True
        except Exception as e:
            print(f"  ❌ 转换失败: {e}")
            return False

    def csv_to_excel(self, result_name: str = "Results") -> str:
        """将输出的 CSV 文件合并为一个 Excel 文件"""
        output_excel_path = self.docs_dir / "Output" / f"{result_name}.xlsx"

        try:
            with pd.ExcelWriter(output_excel_path, engine='openpyxl') as writer:
                for csv_file in self.output_dir.glob("*.csv"):
                    sheet_name = csv_file.stem
                    df = pd.read_csv(csv_file, encoding='utf-8-sig')
                    df.to_excel(writer, sheet_name=sheet_name, index=False)
                    print(f"  ✅ 已写入: {sheet_name}")
            return str(output_excel_path)
        except Exception as e:
            print(f"  ❌ 导出失败: {e}")
            return ""

    def list_available_excel_files(self) -> list:
        """列出 Docs/Input 目录下所有 Excel 文件"""
        input_dir = self.docs_dir / "Input"
        files = []
        for ext in ['*.xlsx', '*.xls']:
            files.extend(input_dir.glob(ext))
        return files

    def generate_random_data(self, num_regions=3, num_crops=6, num_veg=5, num_reservoirs=3, num_months=12):
        """
        生成随机测试数据

        参数:
            num_regions: 区域数量 (默认3: MILM, WILM, MILD)
            num_crops: 作物数量 (默认6: 小麦, 玉米, 油料, 瓜类, 蔬菜, 苜蓿)
            num_veg: 植被数量 (默认5: 乔木, 灌木, 疏林, 其他林地, 高覆盖草地)
            num_reservoirs: 水库数量 (默认3)
            num_months: 时间长度(月) (默认12)

        返回:
            dict: 包含所有 DataFrame 的字典
        """

        # ---------- 基础名称生成 ----------
        region_names = [f"Region_{i+1}" for i in range(num_regions)]
        crop_names = [f"Crop_{j+1}" for j in range(num_crops)]
        veg_names = [f"Veg_{k+1}" for k in range(num_veg)]
        res_names = [f"Reservoir_{r+1}" for r in range(num_reservoirs)]

        # 用 for 循环和简单的加减法来生成年月，避免使用复杂的日期库导致逻辑混乱
        years = []
        months = []
        year = 2020
        month = 1
        for _ in range(num_months):
            years.append(year)
            months.append(month)
            month += 1
            if month > 12:
                month = 1
                year += 1

        result = {}

        # ============================================================
        # 1. RegionParams
        # ============================================================
        region_data = []
        for i, name in enumerate(region_names):
            # 使用随机但合理的值
            region_data.append({
            "unit_name/域名": name,
            "type/类型": f"Type_{i+1}",
            "SW_avail/可用地表水(10⁸m³)": round(random.uniform(1.0, 8.0), 3),
            "GW_avail/可用地下水(10⁸m³)": round(random.uniform(0.5, 4.0), 3),
            "PSW/地表水价格(CNY/m³)": 0.266,
            "PGW/地下水价格(CNY/m³)": 0.342,
            "EC/电力碳排放系数(kgCO₂/kWh)": 0.85,
            "EG/抽水耗电系数(kWh/m³)": 0.5,
            "η_field/(田间水利用系数)": round(random.uniform(0.5, 0.8), 3),
            "η_canal/(渠系水利用系数)": round(random.uniform(0.6, 0.9), 3),
            "CT/(CNY/t)": round(random.uniform(50, 150), 2),
            "OP/(CNY/t)": round(random.uniform(200, 400), 2),
            "CSO/SO2处理成本(CNY/t)": round(random.uniform(500, 1500), 2),
            "CPM/PM10处理成本(CNY/t)": round(random.uniform(300, 800), 2),
            "PDC/河道清淤价格(CNY/t)": round(random.uniform(100, 300), 2),
            "STC/污水处理成本(CNY/t)": round(random.uniform(400, 1000), 2),
            "WP/水价(CNY/t)": round(random.uniform(2, 5), 2),
            "PF/肥料价格(CNY/t)": round(random.uniform(2000, 4000), 2),
            "H_min/生态地下水位最低阈值深度(m)": round(random.uniform(2, 5), 2),
            "H_max/潜水蒸发的极限深度(m)": round(random.uniform(6, 12), 2),
            "μ/给水度": round(random.uniform(0.05, 0.15), 4),
            "φ/降水入渗补给系数": round(random.uniform(0.1, 0.3), 3),
            "ψ/渠道渗漏系数": round(random.uniform(0.05, 0.2), 3),
            "a/潜水蒸发经验乘积系数": round(random.uniform(0.3, 0.7), 3),
            "b/潜水蒸发经验指数系数": round(random.uniform(0.5, 1.5), 3),
            "WT_obj/总调水控制目标(10⁸m³)": round(random.uniform(0.5, 2.0), 3),
            "WT_max/单月最大调水量(10⁸m³)": round(random.uniform(0.2, 0.8), 3),
            "POP/人口(万人)": round(random.uniform(10, 100), 1),
            "FD/人均粮需(kg)": 400,
            "Y_min/最低粮食产量(10⁴t)": round(random.uniform(50, 200), 1),
            "TAC/作物总种植面积(10⁴ha)": round(random.uniform(2, 10), 2),
            "TAE/生态总面积上限(10⁴ha)": round(random.uniform(1, 5), 2),
            "FA/分区面积(km²)": round(random.uniform(500, 3000), 1)
            })
        result["RegionParams"] = pd.DataFrame(region_data)

        # ============================================================
        # 2. MeteoData
        # ============================================================
        meteo_data = []
        for region in region_names:
            base_temp = random.uniform(5, 25)
            for idx in range(num_months):
                # 模拟季节性变化
                seasonal = 10 * np.sin(2 * np.pi * idx / 12)  # 简单的正弦波
                meteo_data.append({
                "unit_name/域名": region,
                "year/年": years[idx],
                "month/月": months[idx],
                "T/空气温度(°C)": round(base_temp + seasonal + random.uniform(-3, 3), 2),
                "P/日降水量(mm)": round(max(0, random.uniform(0, 80)), 2),
                "U2/离地2米处风速(m/s)": round(random.uniform(1.5, 4.5), 2),
                "Rn/冠层表面静辐射(MJ/m²/d)": round(random.uniform(5, 20), 2),
                "G/土壤热通量(MJ/m²/d)": round(random.uniform(0.5, 3.0), 2),
                "es/饱和蒸汽压(kPa)": round(random.uniform(0.5, 3.5), 3),
                "ea/实际蒸汽压(kPa)": round(random.uniform(0.3, 2.5), 3),
                "Δ/饱和蒸汽压曲线斜率": round(random.uniform(0.05, 0.2), 4),
                "E_Phi20/Φ20蒸发皿测得蒸发量(mm)": round(random.uniform(30, 120), 2),
                "γ/干湿计常数(kPA/℃)": 0.066,
                "H_0/初始潜水位深度(m)": round(random.uniform(3, 8), 2)
                })
            result["MeteoData"] = pd.DataFrame(meteo_data)

        # ============================================================
        # 3. CropData
        # ============================================================
        crop_data = []
        for name in crop_names:
            crop_data.append({
            "crop_name/作物名": name,
            "Kc/作物系数": round(random.uniform(0.6, 1.4), 3),
            "HI/作物收获指数": round(random.uniform(0.3, 0.6), 3),
            "BIO/生物量(kg/ha)": round(random.uniform(8000, 22000), 1),
            "SWP/标准化水分生产力(kg/ha)": round(random.uniform(15, 30), 2),
            "D/生长天数(Day)": random.randint(90, 150),
            "α/蒸腾量与蒸散发量之比": round(random.uniform(0.7, 0.95), 3),
            "CS/碳吸收率": round(random.uniform(0.3, 0.6), 3),
            "R/根茎比": round(random.uniform(0.2, 0.5), 3),
            "WC/水分含量": round(random.uniform(0.08, 0.25), 3),
            "Price/价格(CNY/kg)": round(random.uniform(1.5, 4.0), 2),
            "CL/单位面积农田劳动力成本(CNR/公顷)": round(random.uniform(2000, 4000), 1),
            "CF/单位面积农田化肥成本(CNY/公顷)": round(random.uniform(1500, 3500), 1),
            "CP/单位面积农药成本(CNY/公顷)": round(random.uniform(500, 1200), 1),
            "CAF/单位面积农田面积农膜成本(CNY/公顷)": round(random.uniform(400, 900), 1),
            "CI/灌溉成本(CNY/公顷)": round(random.uniform(1000, 2000), 1),
            "CAM/农用机械成本(CNY/公顷)": round(random.uniform(1500, 2800), 1),
            "UF/单位面积施加化肥量(kg/ha)": round(random.uniform(300, 600), 1),
            "UP/单位面积施农药量(kg/ha)": round(random.uniform(10, 30), 1),
            "UAF/单位面积用农膜量(kg/ha)": round(random.uniform(80, 160), 1),
            "AMC/农业机械单位面积的碳排放系数(kgCO2/ha)": 17.78,
            "TC/耕作单位面积的碳排放系数(kgCO2/ha)": 312.60
            })
        result["CropData"] = pd.DataFrame(crop_data)

        # ============================================================
        # 4. CropParams
        # ============================================================
        crop_params_data = []
        for region in region_names:
            for crop in crop_names:
                base_area = random.uniform(0.2, 1.5)
                crop_params_data.append({
                "unit_name/域名": region,
                "crop_name/作物名": crop,
                "AC_min/种植面积下限(10⁴公顷)": round(base_area * 0.5, 2),
                "AC_max/种植面积上限(10⁴公顷)": round(base_area * 1.5, 2),
                "AC_now/当前种植面积(10⁴公顷)": round(base_area, 2),
                "AC_plan/计划种植面积(10⁴公顷)": 0.0
                })
        result["CropParams"] = pd.DataFrame(crop_params_data)

        # ============================================================
        # 5. VegetationData
        # ============================================================
        veg_data = []
        for name in veg_names:
            veg_data.append({
                "vege_name/植被名": name,
                "Kp/植被系数": round(random.uniform(0.5, 1.2), 3),
                "WPE/水净化能力": round(random.uniform(0.2, 0.6), 3),
                "APM/PM10吸收能力": round(random.uniform(1.0, 3.0), 3),
                "ASO/SO2吸收能力": round(random.uniform(0.1, 0.3), 3),
                "ACO/CO2吸收能力": round(random.uniform(2.0, 5.0), 3),
                "F/土壤元素与肥料比值(CNY/t)": round(random.uniform(0.1, 0.5), 3),
                "SEC/土壤元素含量(CNY/t)": round(random.uniform(0.5, 2.0), 3),
                "SEI/土壤侵蚀指数": round(random.uniform(0.2, 0.8), 3),
                "λ/水涵养系数": round(random.uniform(0.1, 0.4), 3),
                "WV/水资源保护价值(CNY/t)": round(random.uniform(2, 8), 2),
                "RCV/休闲文化价值(CNY/公顷)": round(random.uniform(1000, 5000), 1),
                "BV/生物多样性价值(CNY/公顷)": round(random.uniform(2000, 8000), 1)
            })
        result["VegetationData"] = pd.DataFrame(veg_data)

        # ============================================================
        # 6. VegetationParams
        # ============================================================
        veg_params_data = []
        for region in region_names:
            for veg in veg_names:
                base_area = random.uniform(0.1, 0.8)
                veg_params_data.append({
                "unit_name/域名": region,
                "vege_name/植被名": veg,
                "AE_min/最小植被面积(10⁴公顷)": round(base_area * 0.5, 2),
                "AE_max/最大植被面积(10⁴公顷)": round(base_area * 1.8, 2),
                "AE_now/当前植被面积(10⁴公顷)": round(base_area, 2),
                "AE_plan/计划植被面积(10⁴公顷)": 0.0
                })
        result["VegetationParams"] = pd.DataFrame(veg_params_data)

        # ============================================================
        # 7. ReservoirData
        # ============================================================
        res_data = []
        for name in res_names:
            res_data.append({
            "reservoir_name/水库名": name,
            "Vd/死库容(10⁸m³)": round(random.uniform(0.1, 0.5), 3),
            "Va/有效库容(10⁸m³)": round(random.uniform(0.5, 2.0), 3),
            "V_min/储水容量下限(10⁸m³)": round(random.uniform(0.05, 0.2), 3),
            "V_max/储水容量上限(10⁸m³)": round(random.uniform(0.8, 2.5), 3),
            "β/库容系数": round(random.uniform(0.1, 0.4), 3),
            "ρ/水损失率(%)": round(random.uniform(0.02, 0.08), 4)
            })
        result["ReservoirData"] = pd.DataFrame(res_data)

        # ============================================================
        # 8. ReservoirParams (逐月来水)
        # ============================================================
        res_params_data = []
        for res in res_names:
            base_flow = random.uniform(0.05, 0.3)
            for idx in range(num_months):
                seasonal = 0.15 * np.sin(2 * np.pi * (idx - 3) / 12)  # 春季融雪高峰
                res_params_data.append({
                "reservoir_name/水库名": res,
                "year/年": years[idx],
                "month/月": months[idx],
                "W/来水量(10⁸m³)": round(max(0.01, base_flow + seasonal + random.uniform(-0.05, 0.05)), 4)
                })
        result["ReservoirParams"] = pd.DataFrame(res_params_data)

        # ============================================================
        # 9. CropWaterAllocation
        # ============================================================
        crop_water_data = []
        for region in region_names:
            for crop in crop_names:
                for idx in range(num_months):
                    base_water = random.uniform(0.01, 0.1)
                    crop_water_data.append({
                    "unit_name/域名": region,
                    "crop_name/作物名": crop,
                    "year/年": years[idx],
                    "month/月": months[idx],
                    "WA_min/用水需求下限(10⁸m³)": round(base_water * 0.5, 4),
                    "WA_max/用水需求上限(10⁸m³)": round(base_water * 1.8, 4),
                    "SW/地表水供应(10⁸m³)": 0.0,
                    "GW/地下水供应(10⁸m³)": 0.0
                    })
        result["CropWaterAllocation"] = pd.DataFrame(crop_water_data)

        # ============================================================
        # 10. VegetationWaterAllocation
        # ============================================================
        veg_water_data = []
        for region in region_names:
            for veg in veg_names:
                for idx in range(num_months):
                    base_water = random.uniform(0.005, 0.05)
                    veg_water_data.append({
                    "unit_name/域名": region,
                    "vege_name/植被名": veg,
                    "year/年": years[idx],
                    "month/月": months[idx],
                    "WE_min": round(base_water * 0.4, 4),
                    "WE_max": round(base_water * 2.0, 4),
                    "SW/地表水供应(10⁸m³)": 0.0,
                    "GW/地表下供应(10⁸m³)": 0.0
                    })
        result["VegetationWaterAllocation"] = pd.DataFrame(veg_water_data)

        return result

    def save_random_data_to_excel(self, filename, num_regions=3, num_crops=6, num_veg=5, num_reservoirs=3, num_months=12):
        """生成随机数据并保存到 Excel"""
        data = self.generate_random_data(num_regions, num_crops, num_veg, num_reservoirs, num_months)

        with pd.ExcelWriter(filename, engine='openpyxl') as writer:
            for sheet_name, df in data.items():
                df.to_excel(writer, sheet_name=sheet_name, index=False)

        print(f"✅ 随机数据已生成: {filename}")
        print(f"   - {num_regions} 个区域")
        print(f"   - {num_crops} 种作物")
        print(f"   - {num_veg} 种植被")
        print(f"   - {num_reservoirs} 个水库")
        print(f"   - {num_months} 个月")
        return filename