# Save Progress 指令

自動保存工作進度並推送到 GitHub。

## 執行步驟

### 1. 檢查 Git 狀態
請執行 `git status` 並顯示修改的檔案清單。

### 2. 生成工作摘要
根據本次對話內容，自動生成工作摘要，格式如下：
- 做了什麼（條列式）
- 修改了哪些檔案
- 解決了什麼問題

**暫停並詢問使用者**：「這個摘要正確嗎？需要修改嗎？」

### 3. 偵測目前分支
執行 `git branch --show-current` 取得目前分支名稱（master 或 dev）。

### 4. 更新階段紀錄檔
- 讀取 `.claude/sessions/{分支名稱}.md`
- 在檔案**最上方**插入新的階段紀錄：
  ```markdown
  ---

  ## YYYY-MM-DD HH:MM

  ### 摘要
  - [條列式摘要]

  ### 修改的檔案
  - [檔案清單]

  ### Commit
  `[待填入的 commit message]`

  ### 下一步
  - [如果有的話]
  ```

### 5. 生成 Commit Message
根據摘要自動生成簡潔的 commit message。

**暫停並詢問使用者**：「要用這個 commit message 嗎？」

### 6. 執行 Git 操作
```bash
git add .
git commit -m "[使用者確認的 commit message]"
git push origin {目前分支}
```

### 7. 更新階段紀錄檔中的 Commit 欄位
將實際使用的 commit message 填入階段紀錄檔的 Commit 欄位。

### 8. 顯示完成訊息
```
✅ 進度已保存並推送到 GitHub
📝 階段紀錄已更新：.claude/sessions/{分支名稱}.md
```

## 注意事項
- 每次執行都會在對應分支的紀錄檔**最上方**插入新紀錄
- 舊的紀錄會自動往下推，形成流水帳式的歷史記錄
- 如果推送失敗，要顯示錯誤訊息並停止
