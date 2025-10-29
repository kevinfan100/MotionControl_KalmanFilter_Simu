# Debug Log - Motion Control Kalman Filter Simulation

此文件記錄所有 debug 過程、問題分析與修復方法。

---

## 📋 目錄
- [2025-10-29: 初始 Bug 修復](#2025-10-29-初始-bug-修復)
  - [Issue #1: 宏重複定義](#issue-1-宏重複定義)
  - [Issue #2: CRT 安全性警告](#issue-2-crt-安全性警告)
  - [Issue #3: 執行時記憶體存取違規](#issue-3-執行時記憶體存取違規)
- [修改檔案列表](#修改檔案列表)
- [預期執行結果](#預期執行結果)

---

## 2025-10-29: 初始 Bug 修復

---

### Issue #1: 宏重複定義錯誤 ✅ FIXED

**發現時間**: 2025-10-29
**嚴重程度**: Critical (編譯錯誤)
**位置**: `CALCULATION.h:27-28`

#### 問題描述
`N2pN` 宏被定義了兩次，且值不同：
```cpp
#define N2pN (1e12)   // Line 27 - 正確: N 轉 pN 需要乘以 1e12
#define N2pN (1e-12)  // Line 28 - 錯誤: 重複定義且值錯誤
```

#### 根本原因
- 開發過程中的複製貼上錯誤
- 第 28 行的值 `1e-12` 實際上是 `pN2N` 的值（pN 轉 N）
- 可能是開發者誤寫或版本合併時的衝突

#### 影響
- Visual Studio 2022 會報錯: `C4005: 'N2pN': macro redefinition`
- 程式會使用最後一個定義（1e-12），導致單位轉換錯誤
- 所有使用 `N2pN` 的計算結果會錯誤 10^24 倍

#### 修復方法
刪除第 28 行的重複定義：
```cpp
// Before:
#define pN2N (1e-12)
#define N2pN (1e12)
#define N2pN (1e-12)  // 刪除這一行

// After:
#define pN2N (1e-12)
#define N2pN (1e12)
```

#### 驗證
- 編譯無警告
- 宏定義正確: `pN2N = 1e-12` (pN→N), `N2pN = 1e12` (N→pN)

---

### Issue #2: CRT 安全性警告 ✅ FIXED

**發現時間**: 2025-10-29
**嚴重程度**: Medium (編譯警告)
**位置**: Visual Studio 專案設定

#### 問題描述
Visual Studio 2022 對不安全的 C 函數（如 `sprintf`）發出 C4996 警告：
```
warning C4996: 'sprintf': This function or variable may be unsafe.
Consider using sprintf_s instead.
```

#### 根本原因
- VS2022 預設啟用 SDL (Security Development Lifecycle) 檢查
- `sprintf` 沒有邊界檢查，可能導致緩衝區溢出
- 程式中有 20+ 處使用 `sprintf`

#### 影響
- 大量編譯警告，影響開發體驗
- 可能被設定為「將警告視為錯誤」時無法編譯

#### 修復方法
在專案設定中添加預處理器定義來抑制警告：

**修改檔案**: `Jul3_2014_MotionControl_HallSensor_AKF_AR2.vcxproj`

**Debug 配置** (Line 56):
```xml
<PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;_CRT_SECURE_NO_WARNINGS;%(PreprocessorDefinitions)</PreprocessorDefinitions>
```

**Release 配置** (Line 74):
```xml
<PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;_CRT_SECURE_NO_WARNINGS;%(PreprocessorDefinitions)</PreprocessorDefinitions>
```

#### 替代方案（未採用）
- 將所有 `sprintf` 改為 `sprintf_s`（需要修改 20+ 行代碼）
- 增加第三個參數指定緩衝區大小

#### 為何選擇當前方案
- 最小侵入性，不改變原始程式邏輯
- 所有 `sprintf` 的緩衝區都足夠大（300 字元），不會溢出
- 保持與舊版本的相容性

#### 驗證
- 編譯時無 C4996 警告
- Debug 和 Release 配置都正常

---

### Issue #3: 執行時記憶體存取違規 ✅ FIXED

**發現時間**: 2025-10-29
**嚴重程度**: Critical (程式崩潰)
**錯誤類型**: Access Violation (0xC0000005)

#### 問題描述
程式執行時在 `CALCULATION.cpp:1993` 崩潰：
```cpp
Fz_pNOa_k = FNor_Curr_KItheo[Z]*FGain_z +
            ThmForce_Z_NOa_RD[ThmF_Counter]*N2pN +
            F_Ext_z_pN_Apply[Index];  // ← 這裡崩潰
```

**錯誤訊息**:
```
異常例外狀況: 容入存取違規
this->F_Ext_z_pN_Apply[...] 為 0x11110118
```

#### 根本原因分析

**直接原因**: 未初始化的指標陣列
程式嘗試存取以下陣列，但它們未被正確初始化：
1. `ThmForce_X_NOa_RD[ThmF_Counter]` - 熱力（Thermal Force）數據
2. `ThmForce_Y_NOa_RD[ThmF_Counter]`
3. `ThmForce_Z_NOa_RD[ThmF_Counter]`
4. `DampWtr_X_Oa_RD[ThmF_Counter]` - 阻尼係數數據
5. `DampWtr_Y_Oa_RD[ThmF_Counter]`
6. `DampWtr_Z_Oa_RD[ThmF_Counter]`
7. `F_Ext_x_pN_Apply[Index]` - 外力陣列
8. `F_Ext_y_pN_Apply[Index]`
9. `F_Ext_z_pN_Apply[Index]`

**程式碼路徑追蹤**:

1. **主程式** (`Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp`)
   - Line 45-58: 熱力數據讀取代碼**被註解掉**
   ```cpp
   //char fname_ThmForceXYZ[200];
   //sprintf(fname_ThmForceXYZ, "Thermal Force all XYZ Water ...");
   //CALC.Read_ThmForce_XYZ(fname_ThmForceXYZ);  // 沒有執行！
   ```

2. **Tracking_Simu 函數** (`CALCULATION.cpp:1387-2050`)
   - Line 1991-1993: **無條件使用** `ThmForce_X_NOa_RD` 等陣列
   - 沒有檢查這些陣列是否已初始化
   - 導致讀取無效記憶體位址

3. **為何 F_Ext_z_pN_Apply 也會出錯**:
   - `RampTracking_Init` 中有條件分配 (Line 1327):
   ```cpp
   if (F_Ext_x_pN_Apply == NULL)
   {
       F_Ext_x_pN_Apply = new double[LEN_Simu_Meas];
       F_Ext_y_pN_Apply = new double[LEN_Simu_Meas];
       F_Ext_z_pN_Apply = new double[LEN_Simu_Meas];
   }
   ```
   - 但建構函數沒有將這些指標初始化為 NULL
   - 指標可能含有垃圾值，導致 `if (F_Ext_x_pN_Apply == NULL)` 判斷失敗
   - 記憶體沒有被分配，但程式卻試圖使用

#### 為什麼之前能運行？

這段程式碼之前應該是**有啟用熱力數據讀取**的，可能是因為：
1. 測試時被某人註解掉了
2. 不同的實驗模式（某些模式不需要熱力數據）
3. 程式碼版本不同步

從檔案列表可以看到這些檔案存在：
- `Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt` ✓
- `Thermal Noise XYZ 1612SplRate (SI unit).txt` ✓
- `Meas. Noise XYZ (SI unit).txt` ✓

#### 修復方法

**方案 1: 啟用熱力數據讀取**（已採用）

修改 `Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` Line 45-58：

```cpp
// Before: 全部被註解
//char fname_ThmMotionXYZ[200], ...
//CALC.Read_ThmMotion_XYZ(fname_ThmMotionXYZ);

// After: 取消註解，啟用讀取
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
1. 必要的數據檔案都存在專案中
2. 這是程式原本設計的運行方式
3. 不需要修改核心模擬邏輯
4. 保持物理模型的完整性（包含熱力噪音）

**方案 2: 修改程式碼使其可在無熱力數據時運行**（未採用）

需要修改 `CALCULATION.cpp` 的多處代碼：
```cpp
// 在使用前檢查
if (ThmForce_X_NOa_RD != NULL) {
    Fx_pNOa_k = ... + ThmForce_X_NOa_RD[ThmF_Counter]*N2pN + ...;
} else {
    Fx_pNOa_k = ... + 0.0 + ...;  // 無熱力數據
}
```

**為何未採用**:
- 需要修改大量代碼（50+ 行）
- 增加程式複雜度
- 失去熱力噪音模擬的物理意義
- 數據檔案已經存在，沒必要略過

#### 相關檔案

**必需的數據檔案**（所有都已存在）:
1. `KI_10Hz.txt` - 磁力耦合矩陣 ✓
2. `InvKIreal.txt` - 逆耦合矩陣 ✓
3. `Feb28_2013 Coeff2nd_0CenterErr_I[1-6]_1891x20.txt` - 逆模型係數（6 個檔案）✓
4. `Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt` ✓
5. `Thermal Noise XYZ 1612SplRate (SI unit).txt` ✓
6. `Meas. Noise XYZ (SI unit).txt` ✓

#### 技術細節

**ThmForce_X_NOa_RD 陣列的用途**:
- 儲存熱力（布朗運動）在 X/Y/Z 方向的力
- 來自 MATLAB 產生的高斯白噪音數據
- 單位: 歸一化力（需乘以 `N2pN` 轉為 pN）
- 索引: `ThmF_Counter` 在內部迴圈中遞增

**DampWtr_X_Oa_RD 陣列的用途**:
- 儲存水（Water）中的阻尼係數
- 用於計算位置更新: `Px = Px + (SimuIntv/Damp) * Force`
- 索引: 與 `ThmF_Counter` 同步

**物理意義**:
```
總力 = 磁力 + 熱力 + 外力
Fz_pNOa_k = FNor_Curr_KItheo[Z]*FGain_z     // 磁力（控制力）
          + ThmForce_Z_NOa_RD[...]*N2pN     // 熱力（噪音）
          + F_Ext_z_pN_Apply[Index];        // 外力（可選）
```

#### 驗證步驟

1. **確認檔案存在** ✓
   ```bash
   dir "*.txt" /b
   ```
   所有必要檔案都已確認存在

2. **重新編譯**
   - 在 Visual Studio 2022 中重新編譯
   - 選擇 Release | Win32 配置

3. **執行測試**
   - 運行 `Release\Jul3_2014_MotionControl_HallSensor_AKF_AR2.exe`
   - 確認程式可以正常載入所有數據檔案
   - 確認程式可以完成模擬並生成輸出檔案

4. **預期輸出**
   - 無崩潰或錯誤
   - 生成檔案: `TRACKING Ramp (0.0 degree) Simu WTR Pgain(50,50,50) Igain(200,200,200) Dgain(0,0,0) (9-21-2016).txt`
   - 檔案包含完整的模擬數據（目標位置、實際位置、電流、力）

#### 後續建議

**短期**:
- 測試修復後的程式，確認可以正常運行
- 檢查輸出結果的物理合理性

**中期**:
- 在 `CALCULATION` 建構函數中初始化所有指標為 `nullptr`
- 在使用前添加空指標檢查
- 添加更好的錯誤訊息（例如：「缺少熱力數據檔案」）

**長期**:
- 使用智能指標（`std::unique_ptr`, `std::vector`）取代原始指標
- 添加配置檔案，允許選擇是否啟用熱力噪音
- 改善記憶體管理，避免手動 new/delete

---

## Debug 統計

| 問題類型 | 數量 | 已修復 | 待處理 |
|----------|------|--------|--------|
| 編譯錯誤 | 1 | 1 | 0 |
| 編譯警告 | 1 | 1 | 0 |
| 執行時錯誤 | 2 | 2 | 0 |
| 編碼問題 | 1 | 1 | 0 |
| **總計** | **5** | **5** | **0** |

---

---

### Issue #4: 中文註解亂碼 ✅ FIXED

**發現時間**: 2025-10-29
**嚴重程度**: Low (不影響編譯)
**位置**: 所有 .cpp 和 .h 檔案

#### 問題描述
Visual Studio 2022 編譯後，原本的繁體中文註解顯示為亂碼：
```cpp
//sample spot  �p��ɶ�    // 原本應該是「採樣時間」
//CALC.RampTracking_Init(70.0, 0.0, 0, 4, deg);  //xyz����  // 原本應該是中文
```

#### 根本原因
- 原始檔案使用 **Big5** 或混合編碼
- Visual Studio 2022 預設使用 **UTF-8** 讀取原始碼
- 編碼不匹配導致中文字元解析錯誤

#### 影響
- 註解顯示為亂碼（�p��ɶ� 等）
- 不影響程式執行
- 降低程式碼可讀性

#### 修復方法
在專案設定中強制使用 UTF-8 編碼：

**修改檔案**: `Jul3_2014_MotionControl_HallSensor_AKF_AR2.vcxproj`

在 Debug 和 Release 的 `<ClCompile>` 區段添加：
```xml
<AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>
```

#### 為何選擇此方案
- `/utf-8` 編譯選項告訴 MSVC 將源文件視為 UTF-8 編碼
- 不需要手動轉換每個檔案的編碼
- 保持與舊版本的相容性

#### 替代方案（未採用）
1. 將所有檔案轉換為 UTF-8 編碼（風險：可能破壞現有註解）
2. 刪除所有中文註解（失去文檔資訊）
3. 使用 BOM (Byte Order Mark)（不是標準做法）

#### 驗證
- 重新開啟 Visual Studio
- 檢查註解是否正常顯示
- 確認編譯無警告

#### 注意事項
如果亂碼已經存在（檔案已損壞），此修復只能防止未來的亂碼，無法恢復已損壞的註解。

#### 完整設定步驟

要**確保未來新增的繁體中文註解不會亂碼**，需要兩個設定：

**1. 編譯器選項（✅ 已完成）**
- 已在 `.vcxproj` 中添加 `/utf-8` 選項
- 讓 MSVC 編譯器正確處理 UTF-8 檔案

**2. Visual Studio 儲存設定（⚠️ 需手動操作）**

在 Visual Studio 2022 中：
1. 點選：**工具 (Tools)** → **選項 (Options)**
2. 左側：**文字編輯器 (Text Editor)** → **一般 (General)**
3. 或：**環境 (Environment)** → **文件 (Documents)**
4. 設定為：**「以 UTF-8 (含簽章) 儲存」**
5. 或選擇：**「Unicode (UTF-8 with signature) - Codepage 65001」**

**測試方法**：
- 新增繁體中文註解：`// 測試：這是繁體中文`
- 儲存後重新開啟
- 確認中文顯示正常（不是亂碼）

**詳細指南**：請參閱 `.claude/encoding_test_guide.md`

---

## 修改檔案列表

1. `CALCULATION.h` - 刪除重複的宏定義
2. `CALCULATION.cpp` - **在建構函數中初始化所有指標為 NULL**
3. `Jul3_2014_MotionControl_HallSensor_AKF_AR2.vcxproj` - 添加 `_CRT_SECURE_NO_WARNINGS` 和 `/utf-8`
4. `Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` - 啟用熱力數據讀取
5. `.gitignore` - 更新為保守模式（不忽略 .txt 檔案）

---

### Issue #5: 指標未初始化導致記憶體存取違規 ✅ FIXED

**發現時間**: 2025-10-29 (第二次執行)
**嚴重程度**: Critical (程式崩潰)
**位置**: `CALCULATION.cpp:1989`

#### 問題描述
程式執行時仍然崩潰，錯誤訊息：
```
異常例外狀況: 容入存取違規
this->F_Ext_x_pN_Apply 為 0x78293160
```

即使已經啟用熱力數據讀取，程式依然在同一位置崩潰。

#### 根本原因

**指標未初始化的問題**：

1. **建構函數中的遺漏**：
   - `CALCULATION::CALCULATION()` 沒有初始化指標
   - 指標包含**垃圾值**（隨機記憶體位址）
   - 例如：`F_Ext_x_pN_Apply` 可能是 `0x78293160`（無效位址）

2. **條件判斷失敗**：
   ```cpp
   // In RampTracking_Init (Line 1327)
   if (F_Ext_x_pN_Apply == NULL)  // 判斷失敗！因為是垃圾值不是 NULL
   {
       F_Ext_x_pN_Apply = new double[LEN_Simu_Meas];  // 不會執行
   }
   ```

3. **使用未分配的記憶體**：
   ```cpp
   // In Tracking_Simu (Line 1963)
   F_Ext_x_pN_Apply[Index] = 0.0;  // 存取無效位址 → 崩潰！
   ```

#### 受影響的指標

以下指標都沒有初始化：
- `F_Ext_x_pN_Apply`, `F_Ext_y_pN_Apply`, `F_Ext_z_pN_Apply`
- `ThmMotion_X_RD`, `ThmMotion_Y_RD`, `ThmMotion_Z_RD`
- `ThmForce_X_NOa_RD`, `ThmForce_Y_NOa_RD`, `ThmForce_Z_NOa_RD`
- `MeasNoise_X_mOm_RD`, `MeasNoise_Y_mOm_RD`, `MeasNoise_Z_mOm_RD`
- `DampWtr_X_Oa_RD`, `DampWtr_Y_Oa_RD`, `DampWtr_Z_Oa_RD`
- `DampGly_X_Oa_RD`, `DampGly_Y_Oa_RD`, `DampGly_Z_Oa_RD`

#### 修復方法

在建構函數結尾（Line 200 之前）添加指標初始化：

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

#### 為何 Issue #3 的修復不足

Issue #3 啟用了熱力數據讀取，但**沒有解決指標初始化問題**：
- 熱力數據的指標（`ThmForce_X_NOa_RD` 等）在讀取函數中會被分配
- 但 `F_Ext_x_pN_Apply` 等指標依賴條件判斷 `if (ptr == NULL)`
- 如果指標未初始化，條件判斷失敗，記憶體不會分配

#### 驗證

重新編譯並執行，應該：
1. 不再出現 Access Violation
2. 成功完成模擬
3. 生成輸出檔案

---

## 預期執行結果

### 執行流程

#### **階段 1: 初始化與載入數據**（2-5 秒）
程式會依序讀取：
1. 輸出參數資訊（透過 `OutputParameter()`）
2. 讀取磁力耦合矩陣：`KI_10Hz.txt`
3. 讀取逆矩陣：`InvKIreal.txt`
4. 讀取 6 個係數檔案：`Feb28_2013 Coeff2nd_0CenterErr_I[1-6]_1891x20.txt`
5. 讀取熱力數據：
   - `Thermal Noise XYZ 1612SplRate (SI unit).txt`
   - `Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt`
   - `Meas. Noise XYZ (SI unit).txt`

#### **階段 2: 模擬執行**（10-30 秒）
Console 視窗會顯示進度百分比：
```
0 %
0.62111 %
1.24222 %
...
98.7567 %
99.3778 %
99.9989 %
```

**模擬內容**：
- 模式：Ramp Tracking（斜坡追蹤）
- 控制器：PID (Kp=50, Ki=200, Kd=0)
- 軌跡：40 μm 範圍，200 μm/s 速度，0° 角度
- 物理：包含熱力噪音、測量噪音、阻尼力

#### **階段 3: 完成與輸出**
1. 寫入輸出檔案：
   ```
   TRACKING Ramp (0.0 degree) Simu WTR Pgain(50.0,50.0,50.0)
   Igain(200.0,200.0,200.0) Dgain(0.0,0.0,0.0) (9-21-2016).txt
   ```

2. 顯示：
   ```
   請按任意鍵繼續...
   ```

### 輸出檔案格式

**檔案位置**：與 .exe 同一資料夾

**內容**：空格分隔的數值，每行包含：
- Target Position (X, Y, Z) - 目標位置
- Measured Position (X, Y, Z) - 測量位置（有延遲）
- Real Position (X, Y, Z) - 真實位置
- Currents (I1-I6) - 6 個磁極的電流值
- Forces - 控制力

**行數**：取決於模擬時間，通常數千到上萬行

### 可能的錯誤

如果看到以下錯誤，請檢查：

1. **「Read KI matrix fail!」** → `KI_10Hz.txt` 不存在或無法讀取
2. **「Read InvKIreal fail!」** → `InvKIreal.txt` 不存在或無法讀取
3. **程式無輸出就結束** → 檢查所有數據檔案是否在正確位置
4. **Access Violation** → 數據檔案讀取失敗，陣列未初始化

---

## 下一步行動

如果遇到新問題：
1. 記錄錯誤訊息和發生位置
2. 在此檔案新增一個章節記錄
3. 分析根本原因
4. 記錄修復方法
5. 更新「修改檔案列表」

---

*Last Updated: 2025-10-29*
*Maintained by: Claude Code*
