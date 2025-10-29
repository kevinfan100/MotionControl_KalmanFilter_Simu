# Master 分支工作紀錄

此檔案記錄所有在 master 分支上的工作階段。

---

## 2025-10-29 15:45 (修正大檔案問題)

### 摘要
- 修正 GitHub 推送問題：兩個熱力資料檔案超過 100MB 限制
- 將大檔案加入 .gitignore（保留本地檔案）
- 從 Git 追蹤中移除大檔案
- 更新階段紀錄檔結構

### 修改的檔案
- .gitignore（新增大檔案忽略規則）
- .claude/sessions/master.md（補充完整記錄）

### Commit
`5ede82d - Fix: Exclude large thermal data files from Git tracking`

### 說明
發現兩個 327MB 的熱力資料檔案無法推送到 GitHub（限制 100MB）。將它們從 Git 追蹤中移除但保留在本地，確保模擬程式仍可正常運作。

**受影響的檔案**：
- Thermal Force all XYZ Gly DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt (327.58 MB)
- Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt (327.58 MB)

---

## 2025-10-29 15:30 (初始化專案)

### 摘要
- 初始化 Git 儲存庫
- 建立 master 和 dev 分支結構
- 設定 GitHub 遠端連接
- 建立 4 個客製化 Claude Code 指令
- 建立階段紀錄檔結構（master.md 和 dev.md）

### 修改的檔案
- .gitignore（新增 .vs/ 忽略規則）
- .claude/commands/save-progress.md（自動保存進度並推送）
- .claude/commands/resume.md（開始工作時檢查專案狀態）
- .claude/commands/review.md（總結最近 1-2 天進度）
- .claude/commands/clean.md（識別並管理測試腳本）
- .claude/sessions/master.md（master 分支紀錄檔）
- .claude/sessions/dev.md（dev 分支紀錄檔）

### Commit
`d5c278f - Initial commit: Motion Control Kalman Filter Simulation`

### 說明
完成專案的初始設定，建立了完整的開發環境和工作流程基礎：

**分支策略**：
- master：穩定版本，只放測試過的程式碼
- dev：開發分支，日常工作都在這裡進行

**客製化指令功能**：
- `/save-progress`：自動生成摘要、更新紀錄檔、commit 並推送
- `/resume`：顯示專案狀態、最近進度、檔案驗證、工作流程規則
- `/review`：總結最近 7 天的工作進度
- `/clean`：掃描並管理 test_ 開頭的測試腳本

**階段紀錄檔設計**：
- 每個分支一個檔案（master.md、dev.md）
- 流水帳式記錄，最新的在最上方
- 便於導航和搜尋

---
