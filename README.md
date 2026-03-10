# 📋 贴贴TIETIE (Clipboard TIETIE)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Platform: Windows](https://img.shields.io/badge/Platform-Windows-0078D6.svg?logo=windows&logoColor=white)]()
[![Framework: Qt](https://img.shields.io/badge/Framework-Qt-41CD52.svg?logo=qt&logoColor=white)](https://www.qt.io/)

> 一款基于 C++ 和 Qt 开发的轻量级剪贴板管理工具。 / A lightweight clipboard management tool built with C++ and Qt.

[中文介绍](#-中文介绍) | [English Version](#-english-version)

---

## 🇨🇳 中文介绍

**贴贴TIETIE** 旨在解决日常办公和开发中频繁复制粘贴带来的不便。它不仅能记录你的剪贴板历史，还能通过标签、搜索等功能帮你快速找回以前复制过的内容。

### 📸 软件截图
<img width="400" height="804" alt="image" src="https://github.com/user-attachments/assets/6ee7dde0-8672-4a17-ac7b-40ce9d6ee655" />


### ✨ 核心功能
*   🗄️ **永久化存储:** 采用 SQLite 数据库，安全、持久地保存您的剪贴板历史记录，重启不丢失。
*   🏷️ **自定义标签:** 支持为每条剪贴板记录添加并编辑标签，方便分类与归档。
*   🔍 **实时检索:** 提供极速搜索框，支持按“文本内容”或“标签”进行实时过滤查找。
*   ⚙️ **便捷控制:** 支持自定义全局热键（一键显示/隐藏主界面），并支持设置开机自启动。
*   ⏱️ **时间戳显示:** 精确记录并显示每一条内容的复制时间。

### ⚠️ 操作必读 (重要)
> **💡 如何进行复制/粘贴操作？**
> 由于本软件特殊的 UI 交互设计，当您想要复用（粘贴）某一条历史记录时，**请务必使用鼠标点击该条记录的【顶部位置】**！

### 📥 下载与运行
前往本仓库的 [**Releases**](https://github.com/little-heep/Clipboard-TIETIE/releases) 页面下载最新版本。我们提供两种版本供选择：

1.  📦 **绿色压缩包版 (强烈推荐 ⭐)**
    *   包含所需的所有 Qt 依赖库。下载后**必须完整解压**整个文件夹，然后运行主程序 `exe`。
    *   **优势**：启动极快，运行稳定，**绝不会被杀毒软件误报**。
2.  📄 **单文件独立版**
    *   只有一个 `.exe` 文件，双击直接运行，方便携带。
    *   **警告**：由于使用了第三方封包技术（Enigma Virtual Box），**极易被 Windows Defender、360 等杀毒软件误报为病毒**并拦截。如遇拦截，请添加白名单或改用上述的“压缩包版”。

---

## 🇬🇧 English Version

**TIETIE** is designed to solve the inconvenience of frequent copying and pasting in daily work and development. It not only records your clipboard history but also helps you quickly retrieve previously copied content through tags and real-time search.

### 📸 Screenshots
<img width="400" height="804" alt="image" src="https://github.com/user-attachments/assets/6ee7dde0-8672-4a17-ac7b-40ce9d6ee655" />

### ✨ Core Features
*   🗄️ **Persistent Storage:** Uses an SQLite database to permanently and safely save your clipboard history. Data is not lost after a reboot.
*   🏷️ **Editable Tags:** Add and edit custom tags for each clipboard record for easy sorting and archiving.
*   🔍 **Real-time Search:** Instant search functionality filtering by either "text content" or "tags".
*   ⚙️ **Convenient Control:** Customizable global hotkeys (to instantly show/hide the UI) and support for launch-on-boot.
*   ⏱️ **Timestamps:** Accurately records and displays the exact time each item was copied.

### ⚠️ Important Usage Note
> **💡 How to Paste / Reuse a record?**
> Due to the specific UI design of this software, when you want to paste or reuse a history record, **you MUST click on the [TOP AREA] of that specific record's block**!

### 📥 Download & Run
Go to the [**Releases**](https://github.com/little-heep/Clipboard-TIETIE/releases) page to download the latest version. Two editions are available:

1.  📦 **Portable ZIP Version (Highly Recommended ⭐)**
    *   Includes all necessary Qt dependencies. **You must extract the entire ZIP archive** to a folder before running the main `.exe`.
    *   **Pros:** Fast startup, highly stable, and **will NOT trigger false positives from Antivirus software**.
2.  📄 **Standalone Single EXE Version**
    *   A single `.exe` file. Just double-click to run. Great for USB drives.
    *   **Warning:** Because it uses a third-party executable packer (Enigma Virtual Box), **it is highly likely to trigger false positives from Antivirus software** (like Windows Defender). If blocked, please whitelist it or use the ZIP version above instead.

---

## 🛠️ 编译说明 / Build Instructions

如果您是开发者并希望自行编译此项目：
If you are a developer and wish to build this project from source:

1.  **环境 / Requirements:** C++ Compiler (MSVC), Qt 6.
2.  **克隆 / Clone:**
    ```bash
    git clone https://github.com/little-heep/Clipboard-TIETIE.git
    ```
3.  **构建 / Build:** 使用 Clion 打开项目文件，配置您的构建套件 (Kit) 并运行编译。 / Open the project in Clion , configure your Kit, and build.

## 📄 开源协议 / License

本项目基于 [MIT License](LICENSE) 协议开源。
This project is licensed under the [MIT License](LICENSE).

---
💖 **如果你觉得这个工具好用，请点个 Star ⭐️ 支持一下！**
💖 **If you find this tool helpful, please give it a Star ⭐️!**
