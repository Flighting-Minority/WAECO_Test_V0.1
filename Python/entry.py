# 文件名：entry.py
# 作者：谢荣政; DeepseekAi
# 创建日期：2026/7/22
# 最后更新日期：2026/7/25
# 介绍：作为首要接入口，负责识别文件，载入文件
# 联动：translator.py

import subprocess
import sys
import io
import os
import time
from pathlib import Path
from translator import Translator

# 强制标准输出使用 UTF-8 编码
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', errors='replace')

"""WAECO 模型的启动器"""
class WaecoLauncher:

    #一、构造函数
    def __init__(self):
        self.translator = Translator()              #[类]“翻译器”工具
        self.current_excel = None                   #目前操作文件为空
        self.bin_dir = Path("Bin")                  #
        self.exe_path = self.bin_dir / "WAECO_model.exe"  #可执行文件路径

        # 检查 C++ 程序是否存在
        if not self.exe_path.exists():
            print("⚠️ 警告: C++ 程序未编译，请先编译 WAECO_model.exe")

    #菜单展示函数
    def show_menu(self):
        """显示主菜单"""
        print("\n" + "=" * 50)
        print("   🌾 WAECO 水-农业-生态协同优化模型")
        print("=" * 50)
        print("  [1] 载入已有项目 (从 Docs/Input 选择)")
        print("  [2] 导入外部 Excel 文件 (指定路径)")
        print("  [3] 新建空白项目")
        print("  [4] 生成随机测试数据")
        print("  [5] 运行 C++ 核心计算")
        print("  [6] 导出结果为 Excel")
        print("  [7] 论文链接/模型出处")
        print("  [0] 退出")
        print("-" * 50)

        #目前操作文件展示
        if self.current_excel:
            print(f"  当前项目: {self.current_excel}")
        else:
            print("  当前项目: (未载入)")
        print("=" * 50)

    #载入已有项目
    def load_existing_project(self):
        files = self.translator.list_available_excel_files()
        if not files:
            print("❌ Docs/Input 目录下没有 Excel 文件")
            print("   请先将 Excel 文件放入 Docs/Input/ 目录")
            return False

        #可用文件菜单
        print("\n  可用的项目文件:")
        for i, f in enumerate(files):
            print(f"    [{i+1}] {f.name}")

        try:
            choice = int(input("\n  请选择编号 (0 返回): "))
            if choice == 0:
                return False
            if 1 <= choice <= len(files):
                self.current_excel = str(files[choice-1])
                print(f"✅ 已选择: {self.current_excel}")
                return self._load_excel()
            else:
                print("❌ 无效编号")
                return False
        except ValueError:
            print("❌ 请输入数字")
            return False

    #导入外部 Excel 文件
    def import_external_file(self):

        path = input("  请输入 Excel 文件的完整路径: ").strip()
        # 去除可能的引号
        path = path.strip('"').strip("'").strip("“").strip("”").strip("‘").strip("’")

        if not os.path.exists(path):
            print(f"❌ 文件不存在: {path}")
            return False

        self.current_excel = path
        print(f"✅ 已选择: {self.current_excel}")
        return self._load_excel()

    #新建空白项目
    def create_new_project(self):
        name = input("  请输入新项目名称 (不含扩展名): ").strip()
        if not name:
            print("❌ 名称不能为空")
            return False

        # 创建空 Excel 文件模板
        output_path = self.translator.docs_dir / "Input" / f"{name}.xlsx"
        if output_path.exists():
            overwrite = input(f"  ⚠️ 文件 {name}.xlsx 已存在，覆盖？(y/n): ")
            if overwrite.lower() != 'y':
                return False

        # 用 pandas 创建空的模板 Excel
        import pandas as pd
        with pd.ExcelWriter(output_path, engine='openpyxl') as writer:
            for sheet in self.translator.SHEET_NAMES:
                # 创建空 DataFrame，只包含表头提示
                df = pd.DataFrame({"提示": [f"请填入 {sheet} 的数据"]})
                df.to_excel(writer, sheet_name=sheet, index=False)

        self.current_excel = str(output_path)
        print(f"✅ 已创建空白项目: {output_path}")
        return self._load_excel()

    #内部方法：加载 Excel 并转换为 CSV
    def _load_excel(self):

        print(f"  正在转换 {os.path.basename(self.current_excel)} ...")
        if self.translator.excel_to_csv(self.current_excel):
            print("✅ 数据加载成功！")
            return True
        return False

    def generate_random_data(self):
        """生成随机测试数据"""
        print("\n📊 生成随机测试数据")
        print("-" * 40)

        try:
            num_regions = int(input("  区域数量 (默认3): ") or "3")
            num_crops = int(input("  作物数量 (默认6): ") or "6")
            num_veg = int(input("  植被数量 (默认5): ") or "5")
            num_reservoirs = int(input("  水库数量 (默认3): ") or "3")
            num_months = int(input("  时间长度/月 (默认12): ") or "12")
        except ValueError:
            print("❌ 请输入数字")
            return False

        timestamp = time.strftime("%Y%m%d_%H%M%S")
        default_name = f"TestData_{timestamp}"

        name = input(f"  项目名称 (不含扩展名, 默认: {default_name}): ") or default_name

        # 保存到 Docs/Input
        output_path = self.translator.docs_dir / "Input" / f"{name}.xlsx"

        if output_path.exists():
            overwrite = input(f"  ⚠️ 文件 {name}.xlsx 已存在，覆盖？(y/n): ")
            if overwrite.lower() != 'y':
                return False

        self.translator.save_random_data_to_excel(
            str(output_path),
            num_regions, num_crops, num_veg, num_reservoirs, num_months
        )

        self.current_excel = str(output_path)
        return self._load_excel()

    def run_cpp(self):
        """运行 C++ 核心计算"""
        if not self.exe_path.exists():
            print(f"❌ C++ 程序未找到: {self.exe_path}")
            return

        # 检查输入 CSV 是否存在
        input_dir = self.translator.input_dir
        required_files = [f"{name}.csv" for name in self.translator.SHEET_NAMES]
        missing = []
        for f in required_files:
            if not (input_dir / f).exists():
                missing.append(f)

        if missing:
            print("⚠️ 缺少以下输入文件:")
            for f in missing:
                print(f"    - {f}")
            print("请先载入一个有效的数据集")
            return

        print("🚀 正在启动 C++ 核心计算...")

        # ----- 获取 eta -----
        eta = getattr(self, 'eta', 0.5)
        eta_input = input(f"  请输入偏好因子 eta (默认 {eta}, 0~1): ").strip()
        if eta_input:
            try:
                eta = float(eta_input)
                eta = max(0.0, min(1.0, eta))
            except:
                print(f"  无效输入，使用默认 eta = {eta}")

        # ----- 获取 epsilon 收敛阈值 -----
        print("\n  【收敛阈值选择】")
        print("  收敛阈值越小，迭代越精确，但耗时越长")
        print("  [1] 宽松 (0.01)   - 快速收敛")
        print("  [2] 标准 (0.001)  - 平衡精度与速度 (推荐)")
        print("  [3] 精确 (0.0001) - 高精度计算")
        print("  [4] 自定义")
        eps_choice = input("  请选择 (默认 2): ").strip()

        if eps_choice == '1':
            epsilon = 0.01
        elif eps_choice == '3':
            epsilon = 0.0001
        elif eps_choice == '4':
            custom_eps = input("  请输入自定义阈值 (如 0.0005): ").strip()
            try:
                epsilon = float(custom_eps)
                if epsilon <= 0:
                    print("  阈值必须大于 0，使用默认值 0.001")
                    epsilon = 0.001
            except:
                print("  格式错误，使用默认值 0.001")
                epsilon = 0.001
        else:
            epsilon = 0.001  # 默认

        # ----- 获取最大迭代次数 -----
        print("\n  【最大迭代次数选择】")
        print("  [1] 快速 (10次)")
        print("  [2] 标准 (100次) (推荐)")
        print("  [3] 深度 (500次)")
        print("  [4] 自定义")
        iter_choice = input("  请选择 (默认 2): ").strip()

        if iter_choice == '1':
            maxiter = 10
        elif iter_choice == '3':
            maxiter = 500
        elif iter_choice == '4':
            custom_iter = input("  请输入最大迭代次数 (如 200): ").strip()
            try:
                maxiter = int(custom_iter)
                if maxiter < 1:
                    print("  迭代次数必须大于 0，使用默认值 100")
                    maxiter = 100
            except:
                print("  格式错误，使用默认值 100")
                maxiter = 100
        else:
            maxiter = 100

        print(f"\n  参数确认: eta={eta}, epsilon={epsilon}, maxiter={maxiter}")

        try:
            result = subprocess.run(
                [str(self.exe_path), "--eta", str(eta), "--epsilon", str(epsilon), "--maxiter", str(maxiter)],
                cwd=os.getcwd(),
                capture_output=True,
                text=True,
                encoding='utf-8',
                errors='replace',
                timeout=600  # 延长到 10 分钟
            )

            if result.stdout:
                print("--- C++ 程序输出 ---")
                print(result.stdout)
            if result.stderr:
                print("--- C++ 错误输出 ---")
                print(result.stderr)

            if result.returncode == 0:
                print("✅ C++ 计算完成！")
                self._convert_results_to_excel()
            else:
                print(f"❌ C++ 程序异常退出 (代码: {result.returncode})")

        except subprocess.TimeoutExpired:
            print("❌ C++ 程序运行超时（超过10分钟）")
        except Exception as e:
            print(f"❌ 运行失败: {e}")

    def _convert_results_to_excel(self):
        print("📊 正在导出结果为 Excel...")
        # 检查是否存在结果文件
        output_dir = self.translator.output_dir
        csv_files = list(output_dir.glob("*.csv"))
        if not csv_files:
            print("⚠️ 没有在Data/output下找到可导出的结果文件，请先运行 C++ 核心计算 (选项5)并确保有值。")
            return
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        output_path = self.translator.csv_to_excel("Results"+timestamp)
        if output_path:
            print(f"✅ 结果已导出到: {output_path}")

    def _paper_introduction(self):
        """输出论文来源"""
        print("\nAgricultural Water Management 312 (2025) 109408")
        print("标题:【Improving synergy of the water-agriculture-ecology system")
        print("            in arid areas using a novel co-optimization model】")
        print("作者们:Xingyu Zhu;Xiaoling Su;Vijay P. Singh;Haijiang Wu;Jiping Niu;Lianzhou Wu;Jiangdong Chu")
        print("链接1 :journal homepage: www.elsevier.com/locate/agwat")
        print("链接2 :https://www.sciencedirect.com/science/article/pii/S0378377425001222?via%3Dihub")

    def run(self):
        """主循环"""
        while True:
            self.show_menu()
            choice = input("  请输入操作编号: ").strip()
            #[0] 退出
            if choice == '0':
                print("👋 再见！")
                break
            #[1] 载入已有项目 (从 Docs/Input 选择)
            elif choice == '1':
                self.load_existing_project()
            #[2] 导入外部 Excel 文件 (指定路径)
            elif choice == '2':
                self.import_external_file()
            #[3] 新建空白项目
            elif choice == '3':
                self.create_new_project()
            #[4] 生成随机测试数据
            elif choice == '4':
                self.generate_random_data()
            #[5] 运行 C++ 核心计算
            elif choice == '5':
                self.run_cpp()
            #[6] 导出结果为 Excel
            elif choice == '6':
                self._convert_results_to_excel()
            #[7] 论文内容征询
            elif choice == '7':
                self._paper_introduction()
            #[TMC] 菜单兼水印
            elif choice == 'TMC':
                print("(・∀・===")
                time.sleep(1)
                print("(・∀・)...")
                time.sleep(3)
                print("呀......没办法呢了")
                print("喝啊！！！(左勾拳)")
                time.sleep(1)
                print("哈啊！！！(踢腿)")
                time.sleep(1)
                print("UFO之力！！！")
                time.sleep(1)
                print("既然你都发现了，那我就告诉你——")
                time.sleep(1.5)
                print("这是谢荣政在Deepseek(AI)协助下搓出来的一个垃圾，不许你冒领！")
                time.sleep(0.5)
                print("开发的话倒是没问题......")
                print("你给陆大有~~~ε=ε=ε=(~￣▽￣)~")
                time.sleep(3)
            #   其他选择
            else:
                print("❌ 无效选项，请重新输入")

            input("\n按 Enter 键继续...")

if __name__ == "__main__":
    launcher = WaecoLauncher()
    launcher.run()