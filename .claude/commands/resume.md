# Resume 指令

開始工作階段時快速掌握專案狀態。

## 執行步驟

### 1. 讀取專案文檔
依序讀取以下檔案以建立專案理解：
- `CLAUDE.md` - 專案總覽
- `.claude/debug_log.md` - 除錯歷史
- `.claude/claude_code.json` - 專案元數據

### 2. 檢查 Git 狀態
執行以下指令並整理資訊：
```bash
git branch --show-current  # 目前分支
git log -1 --format="%s (%ci)"  # 最後一次 commit
git status --short  # 未提交的變更
```

### 3. 顯示專案狀態（在對話框中）
```
📋 專案：Motion Control Kalman Filter Simulation
📂 目前分支：{分支名稱}
📝 最後提交：{commit message}（{日期時間}）
⚠️  未提交的變更：{數量} 個檔案
```

### 4. 讀取最近進度
- 讀取 `.claude/sessions/{目前分支}.md`
- 顯示**最新的一筆**階段紀錄：
```
🕐 上次工作階段（{日期時間}）：
{摘要內容}
```

### 5. 驗證必要資料檔案
檢查以下檔案是否存在：
- `KI_10Hz.txt`
- `InvKIreal.txt`
- `Feb28_2013 Coeff2nd*.txt`（6 個檔案）
- 熱雜訊資料檔案（3 個）

顯示檢查結果：
```
✅ 必要檔案都存在：
- KI_10Hz.txt
- InvKIreal.txt
- Feb28_2013 Coeff2nd 檔案（6/6 個）
⚠️  熱雜訊資料檔案存在
```

### 6. 顯示工作流程規則
```
📌 開發規則：
- 修改前一定要先讀 CLAUDE.md
- 測試腳本必須使用 test_ 或 TEST_ 前綴
- 實作前先討論計畫
- 結束工作前使用 /save-progress
- 指標必須初始化為 NULL
- 使用 UTF-8 編碼撰寫繁體中文註解
```

### 7. 就緒提示
```
✅ 準備就緒。今天要做什麼？
```

## 注意事項
- **所有資訊都顯示在對話框中**，不建立任何檔案
- 如果發現問題（例如缺少資料檔案），要明確警告使用者
- 這是純粹的資訊顯示指令，不需要使用者互動
