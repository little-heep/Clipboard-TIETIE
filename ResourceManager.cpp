//
// Created by Micheal on 2026/3/1.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ResourceManager.h" resolved

#include "ResourceManager.h"
#include "DatabaseManager.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QScrollBar>

ResourceManager::ResourceManager(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Resource Manager");
    resize(400, 400);
    positionToTopRight();
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);  // Always on top

    // UI Setup
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainlayout = new QVBoxLayout(centralWidget);

    //上部：标题＋搜索框
    topWidget = new QWidget();
    topLayout = new QVBoxLayout(topWidget);
    titleLabel = new QLabel();
    QHBoxLayout *hlayout = new QHBoxLayout();
    serchLabel = new QLineEdit();
    serchLabel->setPlaceholderText("🔍 /t+搜索标签...");  // 提示文字
    serchLabel->setClearButtonEnabled(true);       // 启用 Qt 自带的小叉清空按钮
    deleteallbtn=new QPushButton("🗑");
    hlayout->addWidget(serchLabel);
    hlayout->addWidget(deleteallbtn);
    hlayout->setSpacing(5);
    topLayout->addWidget(titleLabel);
    topLayout->addLayout(hlayout);

    //下部：剪贴历史记录
    historyArea = new QWidget();
    historyLayout = new QVBoxLayout(historyArea);
    listWidget = new QListWidget();
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    historyLayout->addWidget(listWidget);

    mainlayout->addWidget(topWidget);
    mainlayout->addWidget(historyArea, 1);
    setCentralWidget(centralWidget);

    // 热键 (Ctrl+N)
    QHotkey *hotkey = new QHotkey(QKeySequence("Ctrl+N"), true, this);
    connect(hotkey, &QHotkey::activated, this, &ResourceManager::toggleWindow);
    // 增加内容
    clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &ResourceManager::onClipboardChanged);
    //左键双击展开预览
    //connect(listWidget, &QListWidget::itemDoubleClicked, this, &ResourceManager::showContextMenu);
    //左键单击粘贴
    connect(listWidget, &QListWidget::itemClicked, this, &ResourceManager::copy);

    setstyle();
    loadHistory();
}

ResourceManager::~ResourceManager() {

}

void ResourceManager::setstyle() {
    this->setStyleSheet("border: 1.5px solid black;background-color: white;");

    topWidget->setMinimumHeight(70);
    topWidget->setStyleSheet("border-image: url(:/image/title.png) 0 stretch stretch; background-color:none;");
    QGraphicsDropShadowEffect *topshadow = new QGraphicsDropShadowEffect(topWidget);
    topshadow->setBlurRadius(12);          // 模糊半径：越大越柔和（推荐 8~15）
    topshadow->setXOffset(3);              // 水平偏移：正数向右
    topshadow->setYOffset(4);              // 垂直偏移：正数向下（模拟光源在上方）
    topshadow->setColor(QColor(0, 0, 0, 200));  // 黑色半透明（80/255 透明度，建议 60~120）
    topWidget->setGraphicsEffect(topshadow);

    topLayout->setSpacing(0);
    topLayout->setContentsMargins(5,0,5,5);
    serchLabel->setStyleSheet(R"(
    QLineEdit {
        border-image:none;
        background: rgba(255,255,255,255);
        border: 2px outset #8c8c8c;
        border-radius: 20px;
        font-size: 13px;
    }
    )");
    titleLabel->setStyleSheet("border-image: url(:/image/word.png) 0 stretch stretch;");
    deleteallbtn->setStyleSheet("border: 2px outset #8c8c8c; ;border-image:none;background-color:white;color:black");

    historyArea->setStyleSheet("border-image: url(:/image/background.png) 0 stretch stretch; background-color:none;");
    QGraphicsDropShadowEffect *bottonshadow = new QGraphicsDropShadowEffect(historyArea);
    bottonshadow->setBlurRadius(12);          // 模糊半径：越大越柔和（推荐 8~15）
    bottonshadow->setXOffset(3);              // 水平偏移：正数向右
    bottonshadow->setYOffset(4);              // 垂直偏移：正数向下（模拟光源在上方）
    bottonshadow->setColor(QColor(0, 0, 0, 200));  // 黑色半透明（80/255 透明度，建议 60~120）
    historyArea->setGraphicsEffect(bottonshadow);
    historyLayout->setContentsMargins(8, 13, 8, 8);     // 四周留点边距，看起来更像卡片列表
    historyLayout->setSpacing(15);  // 项之间的垂直间距
    listWidget->setStyleSheet("border-image:none;background-color: rgb(255, 255, 255);");
    listWidget->setSpacing(10);
    listWidget->verticalScrollBar()->setStyleSheet(
    // 1. 整体轨道背景：设置为透明或极浅灰，宽度 8px
    "QScrollBar:vertical {"
    "    background: transparent;"
    "    width: 15px;"
    "    margin: 3px 3px 3px 3px;"
    "}"
    // 2. 滑块（中间那个条）：圆角 4px，深灰色
    "QScrollBar::handle:vertical {"
    "    background: #000000;"
    "    min-height: 30px;"
    "    border-radius: 4px;"
    "}"
    // 3. 鼠标悬停在滑块上时：颜色加深
    "QScrollBar::handle:vertical:hover {"
    "    background: #a0a0a0;"
    "}"
    // 4. 隐藏顶部的“向上箭头”按钮
    "QScrollBar::sub-line:vertical {"
    "    height: 0px;"
    "}"
    // 5. 隐藏底部的“向下箭头”按钮
    "QScrollBar::add-line:vertical {"
    "    height: 0px;"
    "}"
    // 6. 轨道剩余部分（滑块上方和下方）：透明
    "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
    "    background: transparent;"
    "}"
    );
}

void ResourceManager::positionToTopRight()
{
    // 获取窗口所属屏幕（多屏情况下更准确）
    QScreen *screen = this->screen();
    if (!screen) return;

    // 屏幕可用区域（排除任务栏、 docked 面板等）
    QRect available = screen->availableGeometry();

    // 窗口自身大小
    int w = this->width();
    int h = this->height();

    // 计算右上角坐标
    int x = available.right()  - w + 1;    // right() 是包含边界的值
    int y = available.top();               // top() 通常为 0（除非有顶部任务栏）

    // 移动窗口（注意：最好在窗口已经 resize 好之后调用）
    this->move(x, y);
}

void ResourceManager::toggleWindow() {
    if (isVisible()) {
        hide();
    } else {
        show();
    }
}

void ResourceManager::copy() {
    if (listWidget->currentItem()) {
        QString text = listWidget->currentItem()->text();
        QApplication::clipboard()->setText(text);
    }
}


void ResourceManager::onClipboardChanged() {
    // 剪贴板新增内容若不为空加入listwidget
    if (clipboard->text().isEmpty() && clipboard->image().isNull()) {
        return;
    }
    //去重
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        if (item->text() == clipboard->text()) {
            listWidget->takeItem(i);
            break;
        }
    }
    //添加到剪贴板
    addHistoryItem(clipboard->text(), clipboard->image());
}

void ResourceManager::displayExistingRecord(int id, const QString &text, const QImage &image, const QDateTime &time, const QString &tag)
{
    auto *item = new QListWidgetItem();
    // 历史记录加载建议：因为 SQL 是 ASC 排序，新解析出来的放在最上面
    listWidget->insertItem(0, item);

    auto *itemWidget = new ClipboardItemWidget(id, text, image, time, tag);
    connect(itemWidget, &ClipboardItemWidget::requestDelete, this, &ResourceManager::onDeleteRecord);
    connect(itemWidget, &ClipboardItemWidget::tagChanged, this, &ResourceManager::onTagUpdated);

    item->setSizeHint(itemWidget->sizeHint());
    listWidget->setItemWidget(item, itemWidget);
    listWidget->scrollToTop();
}

void ResourceManager::addHistoryItem(const QString &text,const QImage &image)
{
    QDateTime now = QDateTime::currentDateTime();
    QString initialTag = "无";

    // 1. 存入数据库
    int recordId = DatabaseManager::instance().addRecord(text, image, now, initialTag);
    if (recordId == -1) return;

    // 2. 调用展示逻辑
    displayExistingRecord(recordId, text, image, now, initialTag);
}

void ResourceManager::onDeleteRecord(int recordId)
{
    if (DatabaseManager::instance().deleteRecord(recordId)) {
        // 遍历找到对应的 Item 并删除
        for (int i = 0; i < listWidget->count(); ++i) {
            QListWidgetItem *item = listWidget->item(i);
            // 获取关联的 Widget
            auto *widget = qobject_cast<ClipboardItemWidget*>(listWidget->itemWidget(item));
            if (widget && widget->getRecordId() == recordId) {
                delete listWidget->takeItem(i); // 真正从列表移除并释放内存
                break;
            }
        }
    }
}

void ResourceManager::onTagUpdated(int recordId, const QString &newTag)
{
    DatabaseManager::instance().updateTag(recordId, newTag);
}

void ResourceManager::loadHistory()
{
    // 1. 从数据库取出所有数据
    QList<ClipboardRecord> history = DatabaseManager::instance().getAllRecords();

    // 2. 遍历并显示到界面
    for (const auto &rec : history) {
        displayExistingRecord(rec.id, rec.text, rec.image, rec.time, rec.tag);
    }
}