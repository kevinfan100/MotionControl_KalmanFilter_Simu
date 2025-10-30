# Dev 分支工作記錄

此檔案記錄 dev 分支的所有工作階段，最新記錄在最上方。

---

## 2025-10-30 14:18

### 摘要
- ✅ 成功執行 Positioning Only 模擬（40000 樣本，~24.8 秒）
- ✅ 修正 F_Ext_x/y/z_pN_Apply 記憶體配置問題（uncommented Estimation_Init）
- ✅ 建立完整 MATLAB 分析腳本（analyze_positioning.m）
- ✅ 產生分析圖表和報告（4 個子圖 + 文字報告）
- ✅ 更新 .gitignore 排除輸出檔案
- ⚠️ 程式在 Simu_Finalize() 清理階段崩潰（0xc0000374），但資料完整

### 修改的檔案
- `jul3_2014_motioncontrol_hallsensor_akf_ar2/Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` - Uncommented Estimation_Init(WTR)
- `.gitignore` - 新增模擬輸出和 MATLAB 輸出規則
- `.vscode/settings.json` - 新增 iostream 關聯
- `.claude/settings.local.json` - 新增執行權限

### 模擬結果
**定位性能（目標：原點 0,0,0）：**
- 初始偏移：9.6 nm（非常小！）
- 穩態誤差平均：X=1.47nm, Y=0.45nm, Z=-0.34nm
- 穩態誤差標準差：~22 nm（三軸一致）
- 控制力：平均 0.18 pN，最大 0.96 pN
- 電流：平均 0.04 A，最大 0.146 A
- 收斂時間：立即收斂（初始誤差已 < 100 nm）

### 產生的檔案（不追蹤）
- `Trapping Simu WTR at(0.0,0.0,0.0)um Pgain(10.0,10.0,10.0) (10-30-2025).txt` (40004 行)
- `matlab_analysis/output/figures/positioning_analysis_2025-10-30.png` (257 KB)
- `matlab_analysis/output/reports/positioning_analysis_2025-10-30.txt` (966 bytes)

### Commits
1. `Fix: Uncomment Estimation_Init to prevent access violation` (39e89a6)
2. `Complete Positioning Only simulation with MATLAB analysis` (455f51a)

### 技術細節
**記憶體配置問題：**
- `F_Ext_x/y/z_pN_Apply` 指標只在 `Estimation_Init()` 和 `RampTracking_Init()` 中配置
- `Positioning_Simu_Init()` 不會配置這些陣列
- 但 `Calc_NextBeadPos()` 無條件使用這些指標（line 1986-1997）
- 解決：必須呼叫 `Estimation_Init(WTR)` 進行配置

**崩潰分析：**
- 錯誤代碼：0xc0000374 (heap corruption)
- 發生位置：`Simu_Finalize()` 記憶體釋放階段
- 原因：可能是 `Estimation_Init()` 配置的額外記憶體與 `Simu_Finalize()` 的刪除邏輯不匹配
- 影響：無（資料已完整寫入）

### 下一步
- 分析 Simu_Finalize() 記憶體釋放邏輯，修正 heap corruption
- 測試不同 PID 增益的定位性能
- 測試 Tracking Only 模擬
- 比較 Const KF vs Adaptive KF 性能

---

## 2025-10-30 12:28

### 摘要
- ✅ 啟用逆模型測試功能（Force Calculation Test）
- ✅ 優化輸出格式（輸入位置、輸入力、輸出力、誤差）
- ✅ 修正函數呼叫參數問題
- ✅ 修正誤差計算邏輯（自動保存原始值）
- ✅ 簡化參數修改流程（只需改兩行）
- ✅ 註解英文化（解決 VS Code 中文亂碼）
- ✅ 註解掉熱力數據載入（加快測試速度）
- ✅ 測試驗證成功（誤差 < 0.6%）

### 修改的檔案
- `jul3_2014_motioncontrol_hallsensor_akf_ar2/Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` - 主程式修改
- `.gitignore` - 新增忽略規則
- `.vscode/settings.json` - 新增 VS Code 編碼設定
- `.claude/main_program_features_analysis.md` - 新增分析文件
- `.claude/vscode_encoding_guide.md` - 新增編碼指南
- `CLAUDE.md` - 輕微更新

### 測試結果
```
Input:  Position (3, 4, 5) μm, Force (6, 4, 5) pN
Output: Force (5.978, 3.978, 4.999) pN
Error:  (-0.022, -0.022, -0.001) pN  ✅ < 0.6%
```

### Commit
`Enable inverse model test with optimized output and error handling`

### 下一步
- 了解逆模型內部運作原理（學習階段，不修改代碼）
- 準備 merge 到 master 分支
- 測試其他功能模組（Positioning、Tracking）

---
