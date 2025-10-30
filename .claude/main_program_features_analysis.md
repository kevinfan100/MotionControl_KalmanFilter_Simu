# 主程式功能完整分析

## 檔案：Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp

這個主程式包含了**5 個主要功能模組**，目前只有一個模組在運行，其他都被註解掉。

---

## 功能模組總覽

| 模組編號 | 功能名稱 | 行數 | 狀態 | 用途 |
|---------|---------|------|------|------|
| 0 | **逆模型測試** | 19-38 | ❌ 註解 | 開發除錯用 |
| 1 | **定位模擬（Positioning Only）** | 64-93 | ❌ 註解 | 單點定位控制 |
| 2 | **軌跡追蹤（Tracking Only）** | 97-123 | ✅ **運行中** | 動態軌跡追蹤 |
| 3 | **定位 + 常數卡爾曼濾波器** | 127-176 | ❌ 註解 | 定位 + 狀態估測 |
| 4 | **軌跡追蹤 + 常數卡爾曼濾波器** | 180-216 | ❌ 註解 | 追蹤 + 常數 KF |
| 5 | **軌跡追蹤 + 自適應卡爾曼濾波器** | 220-256 | ❌ 註解 | 追蹤 + 自適應 KF |

---

## 模組 0：逆模型測試（Force Calculation Test）

**行數**：19-38
**狀態**：❌ 已註解（開發除錯用）

### 功能
測試磁力逆模型的正確性：
- 輸入：期望的力（6 pN）和位置（3, 4, 5 μm）
- 輸出：計算所需的 6 極電流
- 驗證：將電流代入正向模型，看能否得回原本的力

### 包含兩種模型測試
1. **理論模型（Theo）**：
   ```cpp
   CALC.OptInvMdl_Curr_FromLS_KItheo(FdTest_pNOa, PosTest_umOa);
   CALC.Calc_FNor_FromSixCurr_KItheo(PosTest_umOa);
   ```

2. **實驗模型（Real）**：
   ```cpp
   CALC.OptInvMdl_Curr_FromLS_KIreal(FdTest_pNOa, PosTest_umOa);
   CALC.Calc_FNor_FromSixCurr_KIreal(PosTest_umOa);
   ```

### 使用時機
- 修改逆模型算法後驗證
- 檢查 KI 矩陣是否正確載入
- Debug 力與電流轉換

---

## 模組 1：定位模擬（Positioning Only）

**行數**：64-93
**狀態**：❌ 已註解

### 功能
模擬將微珠**定位在固定點**的控制過程，不涉及軌跡追蹤。

### 工作流程
```
1. 設定目標位置（通常是原點 0,0,0）
2. 初始化模擬：CALC.Positioning_Simu_Init(40000 samples)
3. 設定 PID 增益（只有 Kp）
4. 執行定位模擬：CALC.Positioning_Simu(THEO)
5. 輸出結果檔案
```

### 關鍵參數
```cpp
NUM_Simu_Meas = 40000;           // 樣本數（約 24.8 秒 @ 1612 Hz）
Trap_Target = (0.0, 0.0, 0.0);   // 目標位置（原點）
Kp = 10.0;                        // 比例增益
```

### 輸出檔案
```
Trapping Simu WTR at(0.0,0.0,0.0)um Pgain(10.0,10.0,10.0) (12-24-2015).txt
```

### 物理意義
模擬**光學鑷子捕獲**（Optical Trapping）：
- 微珠在布朗運動下的隨機擾動
- PID 控制將微珠維持在中心點
- 驗證控制器的定位精度和穩定性

---

## 模組 2：軌跡追蹤（Tracking Only）⭐ 目前運行

**行數**：97-123
**狀態**：✅ **正在運行**

### 功能
模擬微珠**沿著動態軌跡移動**的控制過程。

### 工作流程
```
1. 設定斜坡軌跡參數：
   - RampTracking_Init(40 μm, 0 μm, UP, 200 μm/s, 0°)

2. 初始化計數器和索引

3. 設定 PID 增益（Kp 和 Ki）

4. 執行追蹤模擬：Tracking_Simu(THEO)

5. 輸出結果到檔案
```

### 關鍵參數
```cpp
RangeXY = 40.0 μm;         // XY 平面運動範圍
RangeZ  = 0.0 μm;          // Z 方向無運動
Speed   = 200 μm/s;        // 軌跡速度
Angle   = 0.0°;            // 軌跡角度（0° = 沿 X 軸）

// PID 增益
Kp = (50, 50, 50);         // 比例增益
Ki = (200, 200, 200);      // 積分增益
Kd = (0, 0, 0);            // 微分增益（未使用）
```

### 斜坡軌跡說明
「Ramp Tracking」是指微珠沿著線性路徑移動：
- **0° 角度**：沿 X 軸從 -20 μm → +20 μm（40 μm 範圍）
- **速度 200 μm/s**：完成軌跡需要 0.2 秒
- **但模擬會持續更長時間**（通常 20-30 秒），軌跡可能重複或延伸

### 輸出檔案
```
TRACKING Ramp (0.0 degree) Simu WTR Pgain(50.0,50.0,50.0)
Igain(200.0,200.0,200.0) Dgain(0.0,0.0,0.0) (9-21-2016).txt
```

### 物理意義
- 測試控制器的**動態追蹤能力**
- 評估在有熱噪音、測量噪音下的追蹤誤差
- 比定位更困難，因為目標不斷移動

---

## 模組 3：定位 + 常數卡爾曼濾波器（Positioning & Const KF）

**行數**：127-176
**狀態**：❌ 已註解

### 功能
與模組 1 相似，但加入**卡爾曼濾波器**進行狀態估測。

### 工作流程
```
1. 設定 PID 增益（Kp = 30）

2. 設定目標位置（原點）

3. 初始化模擬（48000 samples ≈ 29.8 秒）

4. 初始化卡爾曼濾波器：Estimation_Init(WTR)

5. 執行定位模擬（帶狀態估測）：
   Positioning_Simu_WithEsti(THEO, CONST_KF)

6. 輸出兩種結果檔案：
   - 模擬結果（位置、電流、力）
   - 估測結果（位置、速度、力）
```

### 卡爾曼濾波器類型
**CONST_KF**（常數卡爾曼濾波器）：
- 噪音協方差矩陣固定不變
- 參數：`Qw`（過程噪音）、`R`（測量噪音）
- 適用於噪音統計特性已知且穩定的情況

### 關鍵參數
```cpp
NUM_Simu_Meas = 48000;           // 約 29.8 秒
Kp = 30.0;                        // 比例增益
Estimation_Init(WTR);             // 水介質參數
```

### 輸出檔案
1. **模擬結果**：
   ```
   Trapping Simu WTR at(0.0,0.0,0.0)um with PID gain(30.0,30.0,30.0) (1-1-2014).txt
   ```

2. **估測結果**（X, Y, Z 三個檔案）：
   ```
   CONST_KF AR2 Esti WTR WithFixFgain Rst X From C++ (InWater 1612Hz) PID gain(30.0,30.0,30.0) (1-1-2016).txt
   CONST_KF AR2 Esti WTR WithFixFgain Rst Y From C++ ...
   CONST_KF AR2 Esti WTR WithFixFgain Rst Z From C++ ...
   ```

### 可切換介質
程式碼中有註解的甘油（Glycerol）版本：
```cpp
//CALC.Estimation_Init(GLY);  // 甘油介質（黏滯係數更高）
```

### 物理意義
- **Sensor Fusion**：結合測量值與物理模型
- 濾除測量噪音，得到更平滑的位置和速度估計
- 估計**無法直接測量的狀態**（例如作用在微珠上的力）

---

## 模組 4：軌跡追蹤 + 常數卡爾曼濾波器（Tracking & Const KF）

**行數**：180-216
**狀態**：❌ 已註解

### 功能
結合**動態軌跡追蹤**和**卡爾曼濾波器狀態估測**。

### 工作流程
```
1. 設定 PID 增益（Kp = 40）

2. 設定斜坡軌跡：RampTracking_Init(40 μm, 0, 0, 200 μm/s, 0°)

3. 初始化卡爾曼濾波器：Estimation_Init(WTR)

4. 執行追蹤模擬（帶估測）：
   Tracking_Simu_WithEsti(THEO, CONST_KF)

5. 輸出兩種結果檔案
```

### 關鍵參數
```cpp
Kp = 40.0;                        // 比例增益（比模組 3 高）
RampTracking_Init(40, 0, 0, 200, 0);
```

### 輸出檔案
1. **軌跡追蹤結果**：
   ```
   Tracking Simu with PID gain(40.0,40.0,40.0) (7-3-2014) NoDistRej.txt
   ```

2. **估測結果**（X, Y, Z 三個檔案）：
   ```
   CONST_KF AR2 WTR Esti Rst X From C++ (InWater 1612Hz) PID gain(40.0,40.0,40.0) (7-3-2014) Tracking NoDistRej.txt
   ...
   ```

### 額外功能（已註解）
程式碼中有 Hall Sensor 電壓控制的註解：
```cpp
//CALC.Set_PIDgain_Vhall(10.0, 100, 0.0);  // 直接控制 Hall Sensor 電壓
//CALC.Calc_Ctrl_Coeff_Vhall();
```

### 物理意義
- 在動態追蹤過程中進行狀態估測
- 比靜態定位更具挑戰性（目標、狀態都在變化）
- 可評估濾波器在非穩態條件下的性能

---

## 模組 5：軌跡追蹤 + 自適應卡爾曼濾波器（Tracking & Adapt KF）

**行數**：220-256
**狀態**：❌ 已註解

### 功能
與模組 4 類似，但使用**自適應卡爾曼濾波器**。

### 工作流程
```
1. 設定 PID 增益（Kp = 40）

2. 設定斜坡軌跡：RampTracking_Init(80 μm, 0, 0, 100 μm/s, 0°)
   （注意：範圍更大，速度較慢）

3. 初始化卡爾曼濾波器：Estimation_Init(WTR)

4. 執行追蹤模擬（自適應濾波）：
   Tracking_Simu_WithEsti(THEO, ADAPT_KF)

5. 輸出結果
```

### 自適應卡爾曼濾波器（ADAPT_KF）
與 CONST_KF 的差異：
- **動態調整噪音協方差矩陣**
- 根據觀測殘差（innovation）自動更新 `Qw` 和 `R`
- 適用於噪音特性未知或時變的情況
- 計算量更大，但更 robust

### 關鍵參數
```cpp
Kp = 40.0;
RampTracking_Init(80, 0, 0, 100, 0);  // 更大範圍、較慢速度
ADAPT_KF;                              // 自適應濾波器
```

### 輸出檔案
```
ADAPT_KF AR2 WTR Esti Rst X From C++ (InWater 1612Hz) PID gain(40.0,40.0,40.0) (7-3-2014) Tracking NoDistRej.txt
...
```

### 物理意義
- 測試濾波器對**未知或時變噪音**的適應能力
- 比 CONST_KF 更通用，但需要更多計算資源
- 適用於實際實驗中噪音特性難以預先標定的情況

---

## 通用元素（所有模組共享）

### 1. 資料載入（Lines 42-61）
所有模組都需要這些數據：
```cpp
// 熱噪音（布朗運動）
Read_ThmMotion_XYZ("Thermal Noise XYZ 1612SplRate (SI unit).txt");

// 熱力（隨機力）
Read_ThmForce_XYZ("Thermal Force all XYZ Water DampInc ... 100000SplRate.txt");

// 測量噪音（感測器誤差）
Read_MeasNoise_XYZ("Meas. Noise XYZ (SI unit).txt");
```

### 2. 介質選擇
程式碼中可選擇兩種介質：
- **WTR**（Water，水）：黏滯係數較低，微珠運動較快
- **GLY**（Glycerol，甘油）：黏滯係數較高，微珠運動較慢

### 3. 模型選擇
- **THEO**：理論模型（基於理想化的磁力耦合矩陣）
- **REAL**：實驗模型（基於實際測量的磁力特性）

---

## 模組之間的關係

```
複雜度遞增：

模組 0 (測試)
   ↓
模組 1 (定位)  →  模組 3 (定位 + CONST_KF)
   ↓                        ↓
模組 2 (追蹤)  →  模組 4 (追蹤 + CONST_KF)  →  模組 5 (追蹤 + ADAPT_KF)

基本功能 → 加入狀態估測 → 使用自適應濾波器
```

---

## 功能模組使用建議

### 開發/除錯階段
1. **模組 0**：驗證逆模型
2. **模組 1**：測試定位控制器
3. **模組 2**：測試軌跡追蹤

### 研究階段
4. **模組 3**：評估濾波器在穩態條件下的性能
5. **模組 4**：評估濾波器在動態條件下的性能
6. **模組 5**：測試自適應濾波器的魯棒性

---

## 切換模組的方法

### 方法 1：手動註解/取消註解
目前的做法，需要手動調整程式碼。

### 方法 2：使用條件編譯（建議）
可以改用 `#define` 來切換：
```cpp
#define RUN_MODULE 2  // 設定要運行的模組編號

#if RUN_MODULE == 1
    // 模組 1 程式碼
#elif RUN_MODULE == 2
    // 模組 2 程式碼
#endif
```

### 方法 3：命令列參數（最彈性）
接受使用者輸入或命令列參數：
```cpp
int main(int argc, char* argv[]) {
    int module = 2;  // 預設模組
    if (argc > 1) {
        module = atoi(argv[1]);
    }
    // 根據 module 執行對應程式碼
}
```

---

## 輸出檔案格式

所有模組的輸出檔案都包含以下欄位（以空格分隔）：

### 1. 模擬結果檔案（`Write_SimuRst_Target_Meas_Real_Curr_Force`）
```
Time | Target_X | Target_Y | Target_Z |
     | Measured_X | Measured_Y | Measured_Z |
     | Real_X | Real_Y | Real_Z |
     | Current_I1 | Current_I2 | ... | Current_I6 |
     | Force_X | Force_Y | Force_Z
```

### 2. 估測結果檔案（`Write_EstiRst_xyz`）
每個軸獨立輸出：
```
Time | Real_Position | Measured_Position |
     | Estimated_Position | Estimated_Velocity | Estimated_Force
```

---

## 程式碼設計模式

### 優點
- **模組化**：每個功能獨立，便於測試
- **靈活性**：可快速切換不同模式
- **可重複性**：參數都寫死在程式碼中，方便重現結果

### 缺點
- **維護困難**：大量註解容易混亂
- **重複代碼**：很多相似的程式碼片段
- **不靈活**：參數變更需重新編譯

### 改進建議
1. 將模組封裝成函數（例如 `RunModule1()`, `RunModule2()`）
2. 使用配置檔案讀取參數
3. 實作模組選擇機制（命令列參數或互動式選單）

---

## 總結

這個主程式設計是一個**研究型模擬平台**，包含：

| 功能類型 | 模組數量 | 當前使用 |
|---------|---------|---------|
| 除錯工具 | 1 | ❌ |
| 基礎控制 | 2 (定位 + 追蹤) | ✅ 追蹤 |
| 進階控制 | 3 (+ 不同濾波器) | ❌ |

**核心能力**：
- 光學鑷子控制模擬（定位 & 追蹤）
- 卡爾曼濾波器狀態估測（常數 & 自適應）
- 多種物理效應建模（熱噪音、測量噪音、阻尼力）
- 理論與實驗模型對比

**應用場景**：
- 控制器參數調優
- 濾波器性能評估
- 物理模型驗證
- 演算法研究與開發

---

*分析完成日期：2025-10-30*
*維護者：Claude Code*
