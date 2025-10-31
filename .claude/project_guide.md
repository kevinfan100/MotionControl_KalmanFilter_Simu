# 專案指南

本文件整合專案管理、除錯記錄、開發規範等內容。

---

## 目錄

1. [專案概覽](#專案概覽)
2. [除錯記錄](#除錯記錄)
3. [編碼與環境設定](#編碼與環境設定)
4. [工作流程規則](#工作流程規則)

---

# 專案概覽

## 基本資訊

**專案名稱**: Motion Control with Kalman Filter Simulation

**技術架構**:
- 語言: C++ (Visual Studio 2022)
- 平台: Win32
- 程式碼規模: ~3,800 行
- 控制頻率: 1,612 Hz
- 物理模擬頻率: 100,000 Hz

**核心功能**:
- 光學鑷子磁力控制模擬
- 6 極磁力耦合系統
- PID / 滑模控制演算法
- 卡爾曼濾波狀態估測
- 布朗運動與熱噪音模擬

## 關鍵檔案

### 原始碼
- `CALCULATION.cpp` / `CALCULATION.h` - 主模擬引擎 (3,100+ 行)
- `Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` - 主程式入口 (260 行)

### 必要數據檔案
```
jul3_2014_motioncontrol_hallsensor_akf_ar2/
├── KI_10Hz.txt                           # 磁力耦合矩陣 (6×6)
├── InvKIreal.txt                         # 逆耦合矩陣 (6×6)
├── Feb28_2013 Coeff2nd_0CenterErr_I1_1891x20.txt  # 逆模型係數 I1
├── ... (I2 到 I6, 共 6 個檔案)
├── Thermal Noise XYZ 1612SplRate (SI unit).txt
├── Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt  [331 MB]
└── Meas. Noise XYZ (SI unit).txt
```

**⚠️ 注意**: 大型檔案 (>100MB) 使用 Git LFS 管理，不要直接 commit 原始二進位檔。

## 建置與執行

### 建置
```bash
# 在 Visual Studio 2022 中
# 選擇: Release | Win32
# 按 F7 建置
```

可執行檔位置: `Release\Jul3_2014_MotionControl_HallSensor_AKF_AR2.exe`

### 執行
```bash
cd Release
.\Jul3_2014_MotionControl_HallSensor_AKF_AR2.exe
```

**預期輸出**:
1. "Reading Thermal Force: ..." (進度 0-100%)
2. 模擬進度 0-99.9999%
3. 生成輸出檔案
4. "請按任意鍵繼續 . . ."

### 輸出檔案格式

空格分隔的文字檔，每行包含:
```
Time | Target_X/Y/Z | Measured_X/Y/Z | Real_X/Y/Z | I1-I6 | Force_X/Y/Z | ...
```

典型檔案大小: 數 MB ~ 數十 MB (取決於模擬時間)

---

# 除錯記錄

## 已解決問題總覽

| 問題類型 | 數量 | 狀態 |
|---------|------|------|
| 編譯錯誤 | 1 | ✅ 已修復 |
| 編譯警告 | 1 | ✅ 已修復 |
| 執行時錯誤 | 2 | ✅ 已修復 |
| 編碼問題 | 1 | ✅ 已修復 |
| **總計** | **5** | **✅ 全部修復** |

---

## Issue #1: 宏重複定義 ✅ FIXED

**時間**: 2025-10-29
**位置**: `CALCULATION.h:27-28`

### 問題描述
```cpp
#define N2pN (1e12)   // Line 27 - 正確
#define N2pN (1e-12)  // Line 28 - 重複且值錯誤
```

### 根本原因
- 複製貼上錯誤
- Line 28 的值實際上是 `pN2N` 的值

### 修復方法
刪除 Line 28 的重複定義。

### 影響
- 使用錯誤值會導致單位轉換錯誤 10²⁴ 倍
- 所有力的計算會完全錯誤

---

## Issue #2: CRT 安全性警告 ✅ FIXED

**時間**: 2025-10-29
**嚴重程度**: Medium

### 問題描述
Visual Studio 2022 對 `sprintf` 等不安全函數發出 C4996 警告。

### 修復方法
在專案設定中添加預處理器定義:

**檔案**: `Jul3_2014_MotionControl_HallSensor_AKF_AR2.vcxproj`

```xml
<!-- Debug 配置 (Line 56) -->
<PreprocessorDefinitions>
  WIN32;_DEBUG;_CONSOLE;_CRT_SECURE_NO_WARNINGS;%(PreprocessorDefinitions)
</PreprocessorDefinitions>

<!-- Release 配置 (Line 74) -->
<PreprocessorDefinitions>
  WIN32;NDEBUG;_CONSOLE;_CRT_SECURE_NO_WARNINGS;%(PreprocessorDefinitions)
</PreprocessorDefinitions>
```

### 為何選擇此方案
- 最小侵入性
- 所有 `sprintf` 的緩衝區都足夠大 (300 字元)
- 保持與舊版本相容性

### 替代方案 (未採用)
將所有 `sprintf` 改為 `sprintf_s` (需修改 20+ 處)

---

## Issue #3: 執行時記憶體存取違規 ✅ FIXED

**時間**: 2025-10-29
**錯誤代碼**: 0xC0000005 (Access Violation)
**位置**: `CALCULATION.cpp:1993`

### 問題描述
```cpp
Fz_pNOa_k = FNor_Curr_KItheo[Z]*FGain_z +
            ThmForce_Z_NOa_RD[ThmF_Counter]*N2pN +
            F_Ext_z_pN_Apply[Index];  // ← 崩潰
```

### 根本原因

**直接原因**: 未初始化的指標陣列

程式嘗試存取以下陣列但未正確初始化:
- `ThmForce_X/Y/Z_NOa_RD[]` - 熱力數據
- `DampWtr_X/Y/Z_Oa_RD[]` - 阻尼係數數據
- `F_Ext_x/y/z_pN_Apply[]` - 外力陣列

**程式碼路徑追蹤**:

1. **主程式** (`Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` Lines 45-58)
   熱力數據讀取代碼**被註解掉**:
   ```cpp
   //char fname_ThmForceXYZ[200];
   //sprintf(fname_ThmForceXYZ, "Thermal Force all XYZ Water ...");
   //CALC.Read_ThmForce_XYZ(fname_ThmForceXYZ);  // 沒有執行!
   ```

2. **Tracking_Simu 函數** (`CALCULATION.cpp:1387-2050`)
   **無條件使用** `ThmForce_X_NOa_RD` 等陣列，沒有檢查是否已初始化。

3. **為何 F_Ext_z_pN_Apply 也會出錯**:
   `RampTracking_Init` 中有條件分配:
   ```cpp
   if (F_Ext_x_pN_Apply == NULL)  // 判斷失敗!
   {
       F_Ext_x_pN_Apply = new double[LEN_Simu_Meas];
       // ...
   }
   ```
   建構函數沒有初始化指標為 NULL → 指標含垃圾值 → 條件判斷失敗 → 記憶體未分配。

### 修復方法

**方案 1**: 啟用熱力數據讀取 (已採用)

修改主程式 Lines 45-58，取消註解:

```cpp
char fname_ThmMotionXYZ[200],
     fname_MeasNoiseXYZ[200],
     fname_ThmForceXYZ[200];

sprintf(fname_ThmMotionXYZ, "Thermal Noise XYZ 1612SplRate (SI unit).txt");
sprintf(fname_MeasNoiseXYZ, "Meas. Noise XYZ (SI unit).txt");
sprintf(fname_ThmForceXYZ,  "Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt");

CALC.Read_ThmMotion_XYZ(fname_ThmMotionXYZ);
CALC.Read_ThmForce_XYZ (fname_ThmForceXYZ);
CALC.Read_MeasNoise_XYZ(fname_MeasNoiseXYZ);
```

**為何選擇此方案**:
- 必要數據檔案都存在
- 這是程式原本設計的運行方式
- 保持物理模型完整性

**方案 2**: 修改程式碼使其可在無熱力數據時運行 (未採用)
需要修改大量代碼，增加複雜度，且失去熱力噪音模擬意義。

---

## Issue #4: 中文註解亂碼 ✅ FIXED

**時間**: 2025-10-29
**嚴重程度**: Low

### 問題描述
Visual Studio 2022 編譯後，繁體中文註解顯示為亂碼:
```cpp
//sample spot  �p��ɶ�    // 原本應該是「採樣時間」
```

### 根本原因
- 原始檔案使用 **Big5** 或混合編碼
- Visual Studio 2022 預設使用 **UTF-8** 讀取
- 編碼不匹配導致解析錯誤

### 修復方法

在專案設定中強制使用 UTF-8:

**檔案**: `Jul3_2014_MotionControl_HallSensor_AKF_AR2.vcxproj`

```xml
<!-- Debug 和 Release 的 <ClCompile> 區段添加 -->
<AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>
```

### 為何選擇此方案
- `/utf-8` 告訴 MSVC 將源文件視為 UTF-8
- 不需手動轉換每個檔案
- 保持相容性

### 注意事項
此修復只能防止未來的亂碼，**無法恢復已損壞的舊註解**。

### Visual Studio 2022 完整設定步驟

**1. 編譯器選項** (✅ 已完成):
- `.vcxproj` 中已添加 `/utf-8`

**2. Visual Studio 儲存設定** (⚠️ 需手動操作):
1. **工具 (Tools)** → **選項 (Options)**
2. **環境 (Environment)** → **文件 (Documents)**
3. 設定為: **「以 UTF-8 (含簽章) 儲存」**

**測試方法**:
```cpp
// 測試：這是繁體中文註解，包含特殊字元「」【】
```
儲存後重新開啟，確認顯示正常。

---

## Issue #5: 指標未初始化 ✅ FIXED

**時間**: 2025-10-29
**位置**: `CALCULATION.cpp:1989`

### 問題描述
即使啟用熱力數據讀取，程式仍崩潰。

### 根本原因

**指標未初始化**:
```cpp
// 建構函數中未初始化
CALCULATION::CALCULATION()
{
    // ... 其他初始化 ...
    // ❌ 沒有初始化指標!
}
```

### 修復方法

在建構函數結尾 (Line 200 前) 添加:

```cpp
// Initialize all pointers to NULL to prevent access violation
F_Ext_x_pN_Apply = NULL;
F_Ext_y_pN_Apply = NULL;
F_Ext_z_pN_Apply = NULL;

ThmMotion_X_RD = NULL;
ThmMotion_Y_RD = NULL;
ThmMotion_Z_RD = NULL;

ThmForce_X_NOa_RD = NULL;
ThmForce_Y_NOa_RD = NULL;
ThmForce_Z_NOa_RD = NULL;

MeasNoise_X_mOm_RD = NULL;
MeasNoise_Y_mOm_RD = NULL;
MeasNoise_Z_mOm_RD = NULL;

DampWtr_X_Oa_RD = NULL;
DampWtr_Y_Oa_RD = NULL;
DampWtr_Z_Oa_RD = NULL;

DampGly_X_Oa_RD = NULL;
DampGly_Y_Oa_RD = NULL;
DampGly_Z_Oa_RD = NULL;
```

### 為何 Issue #3 的修復不足

Issue #3 啟用了熱力數據讀取，但**沒有解決指標初始化問題**:
- 熱力數據的指標在讀取函數中會被分配
- 但 `F_Ext_x_pN_Apply` 等指標依賴 `if (ptr == NULL)` 判斷
- 未初始化的指標含垃圾值 → 條件判斷失敗 → 記憶體不會分配

---

# 編碼與環境設定

## VS Code 設定

**檔案**: `.vscode/settings.json` (已建立)

```json
{
  "files.encoding": "utf8",
  "files.autoGuessEncoding": true,
  "[cpp]": {
    "files.encoding": "utf8"
  }
}
```

確保 VS Code 使用 UTF-8 讀取所有 C++ 檔案。

## Visual Studio 2022 專案設定

### 編譯選項
```xml
<AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>
```

### 預處理器定義
```xml
<PreprocessorDefinitions>
  WIN32;_DEBUG;_CONSOLE;_CRT_SECURE_NO_WARNINGS;%(PreprocessorDefinitions)
</PreprocessorDefinitions>
```

### 平台與工具組
- **平台**: Win32 (32-bit)
- **工具組**: v143 (Visual Studio 2022)
- **字元集**: Unicode

## 檔案編碼狀態

| 檔案 | 編碼 | 狀態 |
|------|------|------|
| `CALCULATION.cpp` | UTF-8 | 含舊 Big5 註解 (亂碼) |
| `CALCULATION.h` | UTF-8 | 含舊 Big5 註解 (亂碼) |
| `Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` | UTF-8 | 含舊 Big5 註解 (亂碼) |

**建議**: 未來所有新註解使用 UTF-8，舊亂碼可視需要逐步清理。

## 編碼技術細節

### 為什麼混合編碼會造成問題?

**Big5 → UTF-8 誤讀**:
```
原始 Big5 位元組:   B1 C4 AE C9 AE C9 B6 A1
↓ (UTF-8 誤讀)
顯示結果:          �p��ɶ�
```

**UTF-8 → UTF-8 正常**:
```
UTF-8 位元組:      E4 B8 AD E6 96 87
↓ (UTF-8 正確讀取)
顯示結果:          中文
```

---

# 工作流程規則

## 核心規則

### 1. 開始工作前
- **必須執行** `/resume` 指令
- 閱讀 `CLAUDE.md` 了解專案概況
- 檢查 `.claude/debug_log.md` 了解已知問題
- 確認所有必要資料檔案存在

### 2. 實作前討論
- **任何修改前必須先討論計畫**
- 說明為什麼要做這個修改
- 討論可能的實作方式
- 確認修改影響範圍
- 得到確認後才開始實作

### 3. 程式碼修改規範

**指標初始化**:
```cpp
// ✅ 正確
CALCULATION::CALCULATION() {
    ptr = NULL;  // 或 nullptr (C++11)
}

// ❌ 錯誤
CALCULATION::CALCULATION() {
    // 未初始化 ptr
}
```

**編碼格式**:
- 使用 UTF-8 編碼
- 繁體中文註解需測試顯示正常

**命名慣例**:
- `_pN` - 皮牛頓 (piconewtons)
- `_umOm` - 微米, 測量座標
- `_umOa` - 微米, 驅動座標
- `_NOa` - 牛頓, 驅動座標
- `_RD` - 讀取 (Read)

**避免使用 Emoji**: 除非使用者明確要求

### 4. 測試腳本規範

**命名規則**: 必須使用 `test_` 或 `TEST_` 前綴

範例:
- `test_pid_gains.m` (MATLAB)
- `TEST_thermal_noise.py` (Python)
- `test_output.txt` (文字檔)

**清理**: 使用完畢後記得刪除 (或使用 `/clean` 指令)

### 5. 結束工作時
- **必須執行** `/save-progress` 指令
- 確認所有變更已提交
- 不要留下未追蹤的重要檔案

## Git 分支策略

### Master 分支
- **用途**: 穩定的發布版本
- **規則**:
  - 只包含測試過且穩定的程式碼
  - 不直接在 master 上開發
  - 只透過 merge dev 分支更新

### Dev 分支
- **用途**: 日常開發工作
- **規則**:
  - 所有開發工作都在這個分支進行
  - 可以包含實驗性程式碼
  - 經過測試穩定後才 merge 到 master

### 工作流程
```bash
# 開始工作
git checkout dev
/resume

# 開發過程...

# 結束工作
/save-progress  # 自動 commit 並 push 到 dev

# 準備發布穩定版本
git checkout master
git merge dev
/save-progress
```

## 階段紀錄檔

### 結構
- `sessions/master.md` - master 分支工作紀錄
- `sessions/dev.md` - dev 分支工作紀錄
- 格式: 流水帳式，最新記錄在最上方

### 記錄內容
每次使用 `/save-progress` 自動記錄:
- 日期時間
- 工作摘要
- 修改的檔案
- Commit message
- 下一步計畫

## 客製化指令

### `/resume` - 開始工作
快速掌握專案狀態

**顯示**:
- 目前分支和最後一次 commit
- 未提交的變更
- 最近工作階段進度
- 必要資料檔案驗證
- 工作流程規則提醒

**使用時機**: 每次開始工作時

---

### `/save-progress` - 保存進度
自動整理並推送進度到 GitHub

**執行步驟**:
1. 檢查 Git 狀態
2. 生成工作摘要 (需確認)
3. 更新階段紀錄檔
4. 生成 commit message (需確認)
5. 執行 git add、commit、push

**使用時機**: 結束工作或完成重要階段時

---

### `/review` - 進度回顧
總結最近 1-2 天工作進度

**顯示**:
- 最近 7 天工作階段
- Git 提交記錄
- 統計總結
- 主要成就

**使用時機**: 需要回顧進度或準備報告時

---

### `/clean` - 清理測試腳本
識別並管理測試腳本

**功能**:
- 掃描所有 `test_*` 和 `TEST_*` 檔案
- 按年齡和大小分類
- 提供多種刪除選項
- 安全確認機制

**使用時機**: 定期清理或專案變雜亂時

## 重要注意事項

### 資料檔案
以下資料檔案**必須存在**才能執行模擬:
- `KI_10Hz.txt`
- `InvKIreal.txt`
- `Feb28_2013 Coeff2nd_0CenterErr_I1-6_1891x20.txt` (6 個檔案)
- 熱雜訊資料檔案 (如果註解掉讀取，程式會 crash)

### 大型檔案
以下檔案使用 **Git LFS** 管理 (已排除直接 commit):
- `Thermal Force all XYZ Gly DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt` (331 MB)
- `Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt` (331 MB)

**⚠️ 重要**: 這些檔案超過 GitHub 100MB 限制，但模擬需要。**不要刪除本地檔案!**

### 修改檔案列表 (2025-10-29 修復)

1. `CALCULATION.h` - 刪除重複宏定義
2. `CALCULATION.cpp` - 在建構函數中初始化所有指標為 NULL
3. `Jul3_2014_MotionControl_HallSensor_AKF_AR2.vcxproj` - 添加 `_CRT_SECURE_NO_WARNINGS` 和 `/utf-8`
4. `Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` - 啟用熱力數據讀取
5. `.gitignore` - 更新為保守模式

---

## 相關文檔

- `CLAUDE.md` - 專案總覽和技術細節
- `README.md` - 使用者文檔
- `technical_analysis.md` - 技術分析 (物理引擎、控制理論)
- `sessions/master.md` - Master 分支工作記錄
- `sessions/dev.md` - Dev 分支工作記錄

---

*最後更新: 2025-10-31*
*整合來源: debug_log.md, main_program_features_analysis.md, vscode_encoding_guide.md, workflow_rules.md*
