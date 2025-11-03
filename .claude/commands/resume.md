# Resume 指令

開始工作階段時快速掌握專案狀態。

## 執行步驟

### 1. 讀取專案文檔
依序讀取以下檔案以建立專案理解：
- `CLAUDE.md` - 專案總覽
- `.claude/debug_log.md` - 除錯歷史
- `.claude/claude_code.json` - 專案元數據

### 2. 同步遠端更新
執行以下指令檢查遠端狀態：
```bash
git fetch origin
git log HEAD..origin/{current_branch} --oneline  # 檢查遠端有無新 commits
git log origin/{current_branch}..HEAD --oneline  # 檢查本地有無未推送的 commits
```

**自動分析並提供建議：**

#### 情況 A：遠端有新 commits + 本地有未提交更改
1. 分析未提交檔案的類型和數量
2. 提供建議方案（例如：stash → pull → stash pop，或先 commit 再 pull）
3. 詢問使用者：「發現遠端有 X 個新 commits，本地有 Y 個未提交更改。建議方案：[具體步驟]。是否執行？」
4. 等待使用者確認後執行

#### 情況 B：遠端有新 commits + 本地乾淨
1. 列出遠端新 commits 的標題
2. 詢問使用者：「發現遠端有 X 個新 commits：[列表]。是否執行 git pull？」
3. 等待使用者確認後執行

#### 情況 C：本地有未推送的 commits
1. 顯示資訊：「ℹ️ 本地有 X 個未推送的 commits：[列表]」
2. 不自動 push（push 操作較敏感，由使用者決定）

#### 情況 D：完全同步
顯示：「✅ 本地與遠端完全同步」

### 3. 檢查 Git 狀態
執行以下指令並整理資訊：
```bash
git branch --show-current  # 目前分支
git log -1 --format="%s (%ci)"  # 最後一次 commit
git status --short  # 未提交的變更
```

### 4. 顯示專案狀態（在對話框中）
```
📋 專案：Motion Control Kalman Filter Simulation
📂 目前分支：{分支名稱}
📝 最後提交：{commit message}（{日期時間}）
⚠️  未提交的變更：{數量} 個檔案
```

### 5. 讀取最近進度
- 讀取 `.claude/sessions/{目前分支}.md`
- 顯示**最新的一筆**階段紀錄：
```
🕐 上次工作階段（{日期時間}）：
{摘要內容}
```

### 6. 驗證必要資料檔案
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

### 7. 顯示工作流程規則
```
📌 開發規則：
- 修改前一定要先讀 CLAUDE.md
- 測試腳本必須使用 test_ 或 TEST_ 前綴
- 實作前先討論計畫
- 結束工作前使用 /save-progress
- 指標必須初始化為 NULL
- 使用 UTF-8 編碼撰寫繁體中文註解
```

### 8. 就緒提示
```
✅ 準備就緒。今天要做什麼？
```

## 注意事項
- **所有資訊都顯示在對話框中**，不建立任何檔案
- 如果發現問題（例如缺少資料檔案），要明確警告使用者
- 這是純粹的資訊顯示指令，不需要使用者互動
