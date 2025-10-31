# 技術分析文件

本文件整合所有技術分析內容，包含物理引擎、控制理論、熱力模擬等詳細說明。

---

## 目錄

1. [物理引擎核心: Calc_NextBeadPos 分析](#calc_nextbeadpos-分析)
2. [控制系統: Positioning_Simu 流程](#positioning_simu-流程)
3. [Gamma 阻尼係數對控制的影響](#gamma-對控制的影響)
4. [Thermal Force 熱力數據生成原理](#thermal-force-原理)
5. [主程式功能模組說明](#主程式功能模組)

---

# Calc_NextBeadPos 分析

## 函數功能

**位置**: `CALCULATION.cpp:1986-2057`

模擬微粒在磁力、熱力和阻尼作用下的運動，使用 **100 kHz 時間步長**精確計算 1/1612 秒內的位置變化。

### 函數簽名

```cpp
void CALCULATION::Calc_NextBeadPos(double PosBef_umOa[3],
                                   double PosAft_umOa[3],
                                   int THEO_or_REAL)
```

**輸入**:
- `PosBef_umOa[3]`: 當前位置 (μm, Oa 座標)
- `I1toI6_KItheo[6]`: 電流值 (成員變數, A)
- `THEO_or_REAL`: 使用理論或真實模型

**輸出**:
- `PosAft_umOa[3]`: 經過 1/1612 秒後的新位置

## 核心迴圈 (100 kHz × 62 iterations)

```cpp
for (int i=0; i<62; i++)  // 62 × 10μs = 620μs ≈ 1/1612 s
{
    // [1] 計算磁力
    Calc_FNor_FromSixCurr_KItheo(PosAft_umOa, I1toI6_KItheo);

    // [2] 計算總力 = 磁力 + 熱力 + 外力
    Fx_pNOa_k = FNor_Curr_KItheo[X]*FGain_x
              + ThmForce_X_NOa_RD[ThmF_Counter]*N2pN
              + F_Ext_x_pN_Apply[Index];

    // [3] 過阻尼 Langevin 方程: 位移 = (Δt / γ) × F
    Px_umOa_k = Px_umOa_k1
              + (SimuIntv/DampWtr_X_Oa_RD[ThmF_Counter]) * (Fx_pNOa_k) * pN2N * m2um;

    // [4] 更新位置
    PosAft_umOa[X] = Px_umOa_k;
    Px_umOa_k1 = Px_umOa_k;  // 保存為下次迭代的初值

    // [5] 遞增熱力計數器
    ThmF_Counter++;
}
```

### 關鍵物理方程

**過阻尼 Langevin 方程**:
```
γ · dx/dt = F_total

改寫: dx/dt = F_total / γ

離散化: Δx = (Δt / γ) × F_total
```

其中:
- `γ` (gamma) = 阻尼係數 (N·s/m)
- `F_total` = F_magnetic + F_thermal + F_external
- `Δt` = SimuIntv = 10 μs

### 單位轉換詳解

```
Δx [μm] = (SimuIntv [s] / DampWtr [N·s/m]) × F [pN] × pN2N × m2um

代入:
  SimuIntv = 1e-5 s
  DampWtr ≈ 4e-8 N·s/m
  F ≈ 1 pN
  pN2N = 1e-12
  m2um = 1e6

Δx = (1e-5 / 4e-8) × 1 × 1e-12 × 1e6
   = 250 × 1e-6 × 1e6
   = 0.25 μm = 250 nm (每 10μs)
```

---

# Positioning_Simu 流程

## 主迴圈結構

```
for (Index = 0; Index < 40000; Index++)  // 每次 ≈ 0.62 ms
│
├─ 階段 A: 量測模擬
│   [1] 讀取真實位置: Real[Index]
│   [2] 加入量測誤差: Meas = Real + MeasNoise
│   [3] 座標轉換: Om → Oa
│
├─ 階段 B: 誤差計算
│   [4] Error = Target - Measurement
│
├─ 階段 C: PID 控制器
│   [5] Fd = Kp × Error (期望力)
│   [6] 座標轉換: Om → Oa
│
├─ 階段 D: 逆模型
│   [8] 期望力 → 電流 (OptInvMdl_Curr_FromLS_KItheo)
│   [9] 正模型驗證
│
├─ 階段 E: 物理引擎 ⭐
│   [11] 讀取延遲位置: Real[Index + 2]
│   [12] Calc_NextBeadPos() ← 內含 62 次 100kHz 迭代
│   [13] 儲存新位置: Real[Index + 3]
│
└─ 下一次迭代
```

## 數據流向

```
Target (0,0,0)
    │
    ↓ Error
┌─────────┐
│  PID    │ Fd (期望力)
│ Kp=30   │
└────┬────┘
     ↓
┌──────────┐
│ Inverse  │ I1-I6 (電流)
│  Model   │
└────┬─────┘
     ↓
┌────────────────┐
│ Physics Engine │ Real Position
│ (100kHz × 62)  │
│ 磁力+熱力+阻尼 │─┐
└────────────────┘ │
     ↑             │
     │  +MeasNoise │
     └─────────────┘
```

## 時序關係

```
Index:    0      1      2      3      4      5
Real:    [0]    [1]    [2]    [3]    [4]    [5]
Meas:     │      │      │     [0]    [1]    [2]
          └──────┴──────┘ (延遲 2 samples ≈ 1.24 ms)
```

---

# Gamma 對控制的影響

## 檔案結構: "10sec,10sec,10sec"

**檔案**: `Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt`

**DampInc** = Damping Increasing (阻尼遞增)

### 三階段阻尼變化

| 時間區間 | γ_X (N·s/m) | γ_Y | γ_Z | 相對增幅 |
|---------|-------------|-----|-----|---------|
| **0-10s**   | 4.0×10⁻⁸ | 5.0×10⁻⁸ | 6.0×10⁻⁸ | 基準 (100%) |
| **10-20s**  | 6.0×10⁻⁸ | 7.0×10⁻⁸ | 8.0×10⁻⁸ | +40-50% |
| **20-30s**  | 8.0×10⁻⁸ | 9.0×10⁻⁸ | 1.0×10⁻⁷ | +67-100% |

**轉變時刻**:
- 第一次: 樣本 1,000,002 (精確 10.000 秒)
- 第二次: 樣本 2,000,003 (精確 20.000 秒)

## 物理影響分析

### 1. 位置噪音 ∝ 1/√γ

**推導**:
```
Δx = (Δt / γ) × F_thermal

F_thermal ~ N(0, σ_F²)  其中 σ_F ∝ √γ  (FDT)

→ σ_Δx = (Δt/γ) × √(2γkBT/Δt)
       = √(2kBT·Δt/γ)
       ∝ 1/√γ

結論: γ 增加 50% → 位置噪音減少 18%
     γ 增加 100% → 位置噪音減少 29%
```

**數值範例**:
```
0-10s  (γ=4e-8): 位置噪音 ≈ 22 nm
10-20s (γ=6e-8): 理論預測 = 22/√1.5 = 18 nm  (↓18%)
20-30s (γ=8e-8): 理論預測 = 22/√2.0 = 15.6 nm (↓29%)
```

### 2. 控制力需求 ∝ 1/√γ

**P 控制器行為**:
```
F_control = Kp × error

error ∝ 1/√γ  (如上所述)

→ F_control ∝ 1/√γ

物理解釋: γ 增加 → 粒子更穩定 → 誤差減小 → 控制力減小
```

**數值範例** (假設 Kp=30):
```
0-10s:  誤差 22 nm → 控制力 0.66 pN
10-20s: 誤差 18 nm → 控制力 0.54 pN  (↓18%)
20-30s: 誤差 15.6 nm → 控制力 0.47 pN (↓29%)
```

### 3. Fluctuation-Dissipation Theorem (FDT)

**核心公式**:
```
⟨F_thermal²⟩ = 2γkBT / Δt

熱力標準差: σ_F = √(2γkBT/Δt) ∝ √γ
```

**實際數據驗證**:
```
Segment    γ_X        理論 σ_F     實測 RMS
0-10s      4.0e-8     5.70 pN      5.70 pN  ✓
10-20s     6.0e-8     6.98 pN      6.98 pN  ✓
20-30s     8.0e-8     8.06 pN      8.06 pN  ✓
```

## 控制系統影響總結

**優點** (γ 增加):
- ✅ 位置更穩定 (噪音 ↓)
- ✅ 誤差減小 (精度 ↑)
- ✅ 控制力減小 (能耗 ↓)
- ✅ 電流需求減小

**缺點** (γ 增加):
- ❌ 響應速度變慢
- ❌ 移動需要更大的力
- ❌ 動態追蹤困難

---

# Thermal Force 原理

## 檔案內容

**格式**:
```
3000003                          ← 第1行: 總樣本數
DampX    ForceX    DampY    ForceY    DampZ    ForceZ
4.0e-8  2.03e-12  5.0e-8  -1.91e-11  6.0e-8  -5.80e-13  ← 第2行
4.0e-8  1.36e-11  5.0e-8  -4.90e-12  6.0e-8  -6.21e-13  ← 第3行
...
```

**重點**: 同時存放阻尼係數 (γ) 和熱力 (F_thermal)，成對配置。

## MATLAB 生成流程

### Step 1: 設定物理參數

```matlab
kB = 1.3806503e-23;    % 波茲曼常數 (J/K)
T = 294;                % 溫度 (K)
fs = 100000;            % 採樣率 (Hz)
dt = 1/fs;              % 時間步長 = 10 μs
N = 3000003;            % 樣本數 (30秒)
```

### Step 2: 創建階梯阻尼

```matlab
gamma_X = zeros(N, 1);
gamma_X(1:1000000)       = 4.0e-8;  % 0-10s
gamma_X(1000001:2000000) = 6.0e-8;  % 10-20s
gamma_X(2000001:end)     = 8.0e-8;  % 20-30s
```

### Step 3: 根據 FDT 計算標準差

```matlab
for i = 1:N
    sigma_F_X(i) = sqrt(2 * gamma_X(i) * kB * T / dt);
end
```

### Step 4: 生成高斯白噪音

```matlab
ThmForce_X = sigma_F_X .* randn(N, 1);
% randn(): MATLAB 標準常態分佈隨機數
% 平均值 = 0, 標準差 = 1
% 乘以 sigma_F 後: 平均值 = 0, 標準差 = sigma_F
```

### Step 5: 寫入檔案

```matlab
fid = fopen(filename, 'w');
fprintf(fid, '%d\n', N);
for i = 1:N
    fprintf(fid, '%.10e\t%.10e\t%.10e\t%.10e\t%.10e\t%.10e\n', ...
            gamma_X(i), ThmForce_X(i), ...
            gamma_Y(i), ThmForce_Y(i), ...
            gamma_Z(i), ThmForce_Z(i));
end
fclose(fid);
```

## C++ 使用方式

### 讀取階段 (啟動時)

**位置**: `CALCULATION.cpp:340-379`

```cpp
void CALCULATION::Read_ThmForce_XYZ(char* fname_ThmForceXYZ)
{
    ifstream istr_ThmForceXYZ;
    istr_ThmForceXYZ.open(fname_ThmForceXYZ);

    // 讀取第一行: 數據長度
    istr_ThmForceXYZ >> LEN_ThmForce;  // 3,000,003

    // 分配記憶體 (6 個陣列)
    DampWtr_X_Oa_RD   = new double[LEN_ThmForce];
    ThmForce_X_NOa_RD = new double[LEN_ThmForce];
    // ... Y, Z 同理

    // 逐行讀取
    for (int i = 0; i < LEN_ThmForce; i++)
    {
        istr_ThmForceXYZ >> DampWtr_X_Oa_RD[i]
                         >> ThmForce_X_NOa_RD[i]
                         >> DampWtr_Y_Oa_RD[i]
                         >> ThmForce_Y_NOa_RD[i]
                         >> DampWtr_Z_Oa_RD[i]
                         >> ThmForce_Z_NOa_RD[i];
    }
}
```

**記憶體占用**: 6 × 3,000,003 × 8 bytes ≈ 137 MB

### 使用階段 (物理引擎)

**位置**: `CALCULATION.cpp:2016-2022`

```cpp
for (int i = 0; i < 62; i++)  // 100 kHz 內層迴圈
{
    // [1] 從陣列讀取熱力
    Fx_pNOa_k = FNor_Curr_KItheo[X] * FGain_x
              + ThmForce_X_NOa_RD[ThmF_Counter] * N2pN  // ← 查表
              + F_Ext_x_pN_Apply[Index];

    // [2] 從陣列讀取阻尼
    Px_umOa_k = Px_umOa_k1
              + (SimuIntv / DampWtr_X_Oa_RD[ThmF_Counter])  // ← 查表
                * (Fx_pNOa_k) * pN2N * m2um;

    // [3] 遞增索引
    ThmF_Counter++;  // 下次讀取下一個
}
```

## 為什麼用 MATLAB 預先生成?

**優點**:
1. ✅ **可重現性**: 每次模擬用相同的噪音序列
2. ✅ **效率**: 不需要每次運行都生成隨機數
3. ✅ **驗證**: 可以預先檢查噪音統計特性
4. ✅ **成對配置**: 保證 γ 和 F_thermal 符合 FDT

**完整數據流**:
```
MATLAB (離線) → 檔案 (331 MB) → C++ 讀取 (137 MB RAM) → 模擬查表使用
```

---

# 主程式功能模組

**檔案**: `Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp`

## 模組總覽

| 模組 | 功能 | 行數 | 狀態 | 控制器 | 濾波器 |
|-----|------|------|------|--------|--------|
| 0 | 逆模型測試 | 19-38 | ❌ 註解 | - | - |
| 1 | 定位 (Positioning) | 64-93 | ❌ 註解 | P | 無 |
| 2 | 追蹤 (Tracking) | 97-123 | ✅ **運行** | PID | 無 |
| 3 | 定位 + KF | 127-176 | ❌ 註解 | P | CONST_KF |
| 4 | 追蹤 + KF | 180-216 | ❌ 註解 | PID | CONST_KF |
| 5 | 追蹤 + 自適應KF | 220-256 | ❌ 註解 | PID | ADAPT_KF |

## 模組 1: 定位模擬 (Positioning Only)

**目的**: 將微珠維持在固定點 (通常是原點)

```cpp
// 關鍵參數
NUM_Simu_Meas = 40000;           // 樣本數 ≈ 24.8 秒
Trap_Target = (0.0, 0.0, 0.0);   // 目標位置
Kp = 10.0;                        // 比例增益

// 執行
CALC.Positioning_Simu_Init(40000);
CALC.Set_PIDgain(10.0, 10.0, 10.0, 0, 0, 0, 0, 0, 0);
CALC.Positioning_Simu(THEO);
```

**輸出檔案**:
```
Trapping Simu WTR at(0.0,0.0,0.0)um Pgain(10.0,10.0,10.0) (12-24-2015).txt
```

**物理意義**: 模擬光學鑷子捕獲，測試控制器在布朗運動擾動下的定位精度。

**⚠️ Kp 穩定範圍**:
- **穩定**: Kp = 10-35 (穩態誤差 < 20 nm, 電流 < 0.1 A)
- **不穩定**: Kp ≥ 40 (數值發散, 位置誤差 > 10⁹ nm)

## 模組 2: 軌跡追蹤 (Tracking Only) ⭐ 當前運行

**目的**: 微珠沿動態軌跡移動

```cpp
// 關鍵參數
CALC.RampTracking_Init(40.0, 0.0, 0, 200, 0);
// 參數: RangeXY(μm), RangeZ(μm), UP_or_DOWN, Speed(μm/s), Angle(deg)

CALC.Set_PIDgain(50.0, 50.0, 50.0, 200, 200, 200, 0, 0, 0);
// Kp=50, Ki=200, Kd=0

CALC.Tracking_Simu(THEO);
```

**軌跡說明**:
- **0° 角度**: 沿 X 軸 -20μm → +20μm (40μm 範圍)
- **速度 200 μm/s**: 完成軌跡 0.2 秒
- **模擬持續**: 通常 20-30 秒

**輸出檔案**:
```
TRACKING Ramp (0.0 degree) Simu WTR Pgain(50.0,50.0,50.0)
Igain(200.0,200.0,200.0) Dgain(0.0,0.0,0.0) (9-21-2016).txt
```

## 模組 3-5: 卡爾曼濾波器模組

### CONST_KF (常數卡爾曼濾波器)

**特性**:
- 噪音協方差矩陣固定
- 參數: Qw (過程噪音), R (測量噪音)
- 適用於噪音統計特性穩定的情況

**輸出** (額外):
```
CONST_KF AR2 Esti WTR Rst X/Y/Z From C++ (InWater 1612Hz) ...
包含: 真實位置, 測量位置, 估測位置, 估測速度, 估測力
```

### ADAPT_KF (自適應卡爾曼濾波器)

**特性**:
- 動態調整噪音協方差矩陣
- 根據觀測殘差 (innovation) 自動更新 Qw 和 R
- 適用於噪音特性未知或時變的情況
- 計算量更大，但更 robust

## 通用元素

### 介質選擇
- **WTR** (Water): 黏滯係數較低，微珠運動較快
- **GLY** (Glycerol): 黏滯係數較高，微珠運動較慢

### 模型選擇
- **THEO**: 理論模型 (理想化磁力耦合矩陣)
- **REAL**: 實驗模型 (實際測量的磁力特性)

### 必要數據檔案
```cpp
// 主程式 lines 45-58 讀取
Read_ThmMotion_XYZ("Thermal Noise XYZ 1612SplRate (SI unit).txt");
Read_ThmForce_XYZ("Thermal Force all XYZ Water DampInc ... 100000SplRate.txt");
Read_MeasNoise_XYZ("Meas. Noise XYZ (SI unit).txt");
```

## 模組關係圖

```
複雜度遞增:

模組 0 (測試)
   ↓
模組 1 (定位)  →  模組 3 (定位 + CONST_KF)
   ↓                        ↓
模組 2 (追蹤)  →  模組 4 (追蹤 + CONST_KF)  →  模組 5 (追蹤 + ADAPT_KF)

基本功能 → 加入狀態估測 → 使用自適應濾波器
```

---

## 附錄: 重要物理常數與單位

### 常數定義 (`CALCULATION.h`)

```cpp
#define pN2N (1e-12)     // pN → N
#define N2pN (1e12)      // N → pN
#define m2um (1e6)       // m → μm
#define um2m (1e-6)      // μm → m

#define SplRate 1612     // 控制採樣率 (Hz)
#define SimuRate 100000  // 物理模擬率 (Hz)
#define R_norm 550.0     // 工作空間半徑 (μm)
#define DELAY_STEP_Meas 2  // 測量延遲 (samples)
```

### 座標系統

- **Om** (Measurement coordinate): 測量座標系
- **Oa** (Actuation coordinate): 驅動座標系
- 轉換矩陣: `id_m2a`, `id_a2m`

### 典型物理參數

```
微珠半徑: ~0.5 μm
阻尼係數: 4-10 × 10⁻⁸ N·s/m
磁力範圍: ±10 pN
熱力 RMS: ~5-8 pN (視阻尼而定)
位置噪音: ~15-25 nm (視阻尼而定)
溫度: 294 K (21°C)
```

---

*最後更新: 2025-10-31*
*整合來源: calc_nextbeadpos_analysis.md, gamma_control_relationship.md, positioning_simu_flowchart.md, thermal_force_10sec_analysis.md, thermal_force_calculation_detail.md*
