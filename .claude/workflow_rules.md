# 工作流程規則

本文檔定義專案的開發工作流程和規則，適用於所有開發工作。

## 📋 核心規則

### 1. 開始工作前
- **必須先執行** `/resume` 指令
- 閱讀 [CLAUDE.md](../CLAUDE.md) 了解專案概況
- 檢查 `.claude/debug_log.md` 了解已知問題
- 確認所有必要資料檔案存在

### 2. 實作前的討論
- **任何修改前必須先討論計畫**
- 說明為什麼要做這個修改
- 討論可能的實作方式
- 確認修改的影響範圍
- 得到使用者確認後才開始實作

### 3. 程式碼修改規範
- **指標初始化**：所有指標必須初始化為 `NULL`
- **編碼格式**：使用 UTF-8 編碼撰寫繁體中文註解
- **命名慣例**：
  - Om（Measurement）座標系統
  - Oa（Actuation）座標系統
  - 單位後綴：`_pN`（皮牛頓）、`_mOm`（毫米）、`_NOa`（牛頓）
- **避免使用表情符號**：除非使用者明確要求

### 4. 測試腳本規範
- **命名規則**：必須使用 `test_` 或 `TEST_` 前綴
- **範例**：
  - `test_pid_gains.m`
  - `TEST_thermal_noise.py`
  - `test_output.txt`
- **清理**：使用完畢後記得刪除（或使用 `/clean` 指令）
- **跨語言通用**：不限定程式語言或檔案類型

### 5. 結束工作時
- **必須執行** `/save-progress` 指令
- 確認所有變更都已提交
- 不要留下未追蹤的重要檔案

## 🌿 Git 分支策略

### Master 分支
- **用途**：穩定的發布版本
- **規則**：
  - 只包含測試過且穩定的程式碼
  - 不直接在 master 上開發
  - 只透過 merge dev 分支來更新

### Dev 分支
- **用途**：日常開發工作
- **規則**：
  - 所有開發工作都在這個分支進行
  - 可以包含實驗性的程式碼
  - 經過測試穩定後才 merge 到 master

### 工作流程
```bash
# 開始工作
git checkout dev
/resume

# 開發過程...

# 結束工作
/save-progress  # 會自動 commit 並 push 到 dev

# 準備發布穩定版本
git checkout master
git merge dev
/save-progress  # 會推送到 master
```

## 📝 階段紀錄檔

### 結構
- **master.md**：master 分支的所有工作紀錄
- **dev.md**：dev 分支的所有工作紀錄
- **格式**：流水帳式，最新記錄在最上方

### 記錄內容
每次使用 `/save-progress` 時會自動記錄：
- 日期時間
- 工作摘要
- 修改的檔案
- Commit message
- 下一步計畫（如果有）

## 🛠️ 客製化指令

### `/resume` - 開始工作
**用途**：快速掌握專案狀態

**會顯示**：
- 目前分支和最後一次 commit
- 未提交的變更
- 最近一次工作階段的進度
- 必要資料檔案驗證
- 工作流程規則提醒

**何時使用**：每次開始工作時

---

### `/save-progress` - 保存進度
**用途**：自動整理並推送進度到 GitHub

**執行步驟**：
1. 檢查 Git 狀態
2. 生成工作摘要（需確認）
3. 更新階段紀錄檔
4. 生成 commit message（需確認）
5. 執行 git add、commit、push

**何時使用**：結束工作時或完成重要階段時

---

### `/review` - 進度回顧
**用途**：總結最近 1-2 天的工作進度

**會顯示**：
- 最近 7 天的工作階段
- Git 提交記錄
- 統計總結
- 主要成就

**何時使用**：需要回顧進度或準備報告時

---

### `/clean` - 清理測試腳本
**用途**：識別並管理專案中的測試腳本

**功能**：
- 掃描所有 `test_*` 和 `TEST_*` 檔案
- 按年齡和大小分類
- 提供多種刪除選項
- 安全確認機制

**何時使用**：定期清理或專案變得雜亂時

## ⚠️ 重要注意事項

### 資料檔案
以下資料檔案**必須存在**才能執行模擬：
- `KI_10Hz.txt`
- `InvKIreal.txt`
- `Feb28_2013 Coeff2nd_0CenterErr_I1-6_1891x20.txt`（6 個檔案）
- 熱雜訊資料檔案（可選，但註解的話程式會 crash）

### 大型檔案
以下檔案已從 Git 排除（保留在本地）：
- `Thermal Force all XYZ Gly DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt` (327MB)
- `Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt` (327MB)

這些檔案超過 GitHub 100MB 限制，但模擬需要使用。**不要刪除本地檔案！**

### Visual Studio 設定
- **平台工具組**：v143 (Visual Studio 2022)
- **編碼**：使用 `/utf-8` 編譯旗標
- **CRT 安全性**：已停用 `_CRT_SECURE_NO_WARNINGS`

## 📚 相關文檔

- [CLAUDE.md](../CLAUDE.md) - 專案總覽和技術細節
- [README.md](../README.md) - 使用者文檔
- [debug_log.md](debug_log.md) - 除錯歷史記錄
- [sessions/master.md](sessions/master.md) - Master 分支工作記錄
- [sessions/dev.md](sessions/dev.md) - Dev 分支工作記錄

## 🔄 持續改進

這份文檔會隨著專案發展持續更新。如果發現新的模式或最佳實踐，請更新此文檔。
