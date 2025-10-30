# VS Code 繁體中文註解編碼指南

## 問題說明

**症狀**：
- Visual Studio 2022 中新增的中文註解顯示正常
- VS Code 中部分舊的中文註解顯示為亂碼（例如：�p��ɶ�）
- 新增的中文註解（例如第 40 行「中文註解」）顯示正常

**原因**：
1. 專案原始檔案使用 **Big5 編碼**撰寫
2. Visual Studio 2022 已設定 `/utf-8` 編譯選項，新增的註解使用 UTF-8
3. VS Code 預設使用 UTF-8 讀取，但舊的 Big5 註解已經損壞
4. **檔案編碼混合**：同一個檔案中有 Big5 和 UTF-8 兩種編碼

## 解決方案

### 1. VS Code 設定（已完成）

已建立 `.vscode/settings.json`，包含以下設定：

```json
{
  "files.encoding": "utf8",
  "files.autoGuessEncoding": true,
  "[cpp]": {
    "files.encoding": "utf8"
  }
}
```

這確保 VS Code 使用 UTF-8 讀取所有 C++ 檔案。

### 2. Visual Studio 2022 設定（已完成）

專案檔已設定：
- **編譯選項**：`/utf-8`（在 `.vcxproj` 中）
- 讓 MSVC 將源文件視為 UTF-8 編碼

### 3. 舊註解亂碼處理

**已損壞的註解（無法自動恢復）**：
- Line 67: `�p��ɶ�` （原本可能是「採樣時間」）
- Line 79: `�ݭn���D���Ѽ�` （原本可能是「需要控制參數」）
- Line 85: `�s��` （原本可能是「寫入」）

**建議處理方式**：
1. **保持現狀**：舊註解已損壞，不影響編譯
2. **手動修復**：如果知道原意，可手動用 UTF-8 重寫
3. **刪除亂碼**：如果不重要，可直接刪除

## 未來新增中文註解的最佳實踐

### 在 Visual Studio 2022 中：

1. 確認檔案編碼：
   - 開啟檔案後，點選 **File → Advanced Save Options**
   - 選擇：**Unicode (UTF-8 with signature) - Codepage 65001**
   - 點選 **OK**

2. 全域設定（一勞永逸）：
   - **Tools → Options**
   - 左側：**Environment → Documents**
   - 勾選：**Save documents as Unicode when data cannot be saved in codepage**

### 在 VS Code 中：

1. 檔案已自動設定為 UTF-8（透過 `.vscode/settings.json`）
2. 可在右下角狀態列檢查編碼（應顯示 `UTF-8`）
3. 如果顯示其他編碼，點選後選擇 **Save with Encoding → UTF-8**

## 驗證方法

### 測試新增繁體中文註解：

1. 在任一 `.cpp` 或 `.h` 檔案中新增測試註解：
   ```cpp
   // 測試：這是繁體中文註解，包含特殊字元「」【】
   ```

2. 儲存檔案

3. 在 **Visual Studio 2022** 和 **VS Code** 中分別開啟，確認顯示正常

4. 如果在任一編輯器中看到亂碼，表示編碼設定有問題

## 檔案編碼狀態

目前專案檔案編碼狀態：

| 檔案 | 編碼 | 狀態 |
|------|------|------|
| `CALCULATION.cpp` | UTF-8 | 含舊 Big5 註解（亂碼） |
| `CALCULATION.h` | UTF-8 | 含舊 Big5 註解（亂碼） |
| `Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` | UTF-8 | 含舊 Big5 註解（亂碼） |

**建議**：未來所有新增的註解都使用 UTF-8，舊的亂碼註解可視需要逐步清理或修復。

## 技術細節

### 為什麼混合編碼會造成問題？

```
原始 Big5 位元組：   B1 C4 AE C9 AE C9 B6 A1
↓
UTF-8 讀取（錯誤）： �p��ɶ�
```

Big5 的雙位元組字元被 UTF-8 誤認為無效字元，顯示為 � 或亂碼。

### 為什麼新註解正常？

```
UTF-8 位元組：      E4 B8 AD E6 96 87
↓
UTF-8 讀取（正確）： 中文
```

使用相同編碼寫入和讀取，顯示正常。

## 參考資料

- Visual Studio UTF-8 設定：[Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8)
- VS Code 編碼設定：[VS Code Docs](https://code.visualstudio.com/docs/editor/codebasics#_file-encoding-support)
- 專案除錯日誌：`.claude/debug_log.md` (Issue #4)

---

*建立日期：2025-10-30*
*維護者：Claude Code*
