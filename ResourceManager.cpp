//
// Created by Micheal on 2026/3/1.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ResourceManager.h" resolved

#include "ResourceManager.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

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
    serchLabel = new QLineEdit();
    serchLabel->setPlaceholderText("🔍 /t+搜索标签...");  // 提示文字
    serchLabel->setClearButtonEnabled(true);       // 启用 Qt 自带的小叉清空按钮
    topLayout->addWidget(titleLabel);
    topLayout->addWidget(serchLabel);

    //下部：剪贴历史记录
    historyArea = new QWidget();
    historyLayout = new QVBoxLayout(historyArea);
    listWidget = new QListWidget();
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    historyLayout->addWidget(listWidget);

    mainlayout->addWidget(topWidget);
    mainlayout->addWidget(historyArea, 1);
    setCentralWidget(centralWidget);

    // 拖拽
    setAcceptDrops(true);
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
}

ResourceManager::~ResourceManager() {

}

void ResourceManager::setstyle() {
    this->setStyleSheet("QWidget { border: 1.5px solid black;background-color: white;}");

    topWidget->setMinimumHeight(70);
    topWidget->setStyleSheet("border-image: url(:/image/title.png) 0 stretch stretch; background-color:none;");
    QGraphicsDropShadowEffect *topshadow = new QGraphicsDropShadowEffect(topWidget);
    topshadow->setBlurRadius(12);          // 模糊半径：越大越柔和（推荐 8~15）
    topshadow->setXOffset(3);              // 水平偏移：正数向右
    topshadow->setYOffset(4);              // 垂直偏移：正数向下（模拟光源在上方）
    topshadow->setColor(QColor(0, 0, 0, 80));  // 黑色半透明（80/255 透明度，建议 60~120）
    topWidget->setGraphicsEffect(topshadow);

    topLayout->setSpacing(0);
    topLayout->setContentsMargins(5,0,5,5);
    serchLabel->setStyleSheet(R"(
    QLineEdit {
        border-image:none;
        background: rgba(255,255,255,255);
        border: 2px solid black;
        border-radius: 20px;
        font-size: 13px;
    }

    /* 模拟内阴影/内描边 */
    QLineEdit {
        border: 2px inset #ddd;  /* inset 边框模拟轻微内凹 */
    }
    )");
    titleLabel->setStyleSheet("border-image: url(:/image/word.png) 0 stretch stretch;");
    historyArea->setStyleSheet("border-image: url(:/image/background.png) 0 stretch stretch; background-color:none;");
    QGraphicsDropShadowEffect *bottonshadow = new QGraphicsDropShadowEffect(historyArea);
    bottonshadow->setBlurRadius(12);          // 模糊半径：越大越柔和（推荐 8~15）
    bottonshadow->setXOffset(3);              // 水平偏移：正数向右
    bottonshadow->setYOffset(4);              // 垂直偏移：正数向下（模拟光源在上方）
    bottonshadow->setColor(QColor(0, 0, 0, 80));  // 黑色半透明（80/255 透明度，建议 60~120）
    historyArea->setGraphicsEffect(bottonshadow);
    historyLayout->setContentsMargins(8, 13, 8, 8);     // 四周留点边距，看起来更像卡片列表
    historyLayout->setSpacing(15);  // 项之间的垂直间距
    listWidget->setStyleSheet("border-image:none;background-color: rgb(255, 255, 255);");

}

void ResourceManager::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
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

void ResourceManager::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                addResource(url.toLocalFile());
            }
        }
        event->acceptProposedAction();
    }
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
    if (!clipboard->text().isEmpty()) {
        addHistoryItem(clipboard->text(),QImage());
    }
    if (!clipboard->image().isNull()) {
        addHistoryItem(nullptr,clipboard->image());
    }
}

void ResourceManager::addResource(const QString &text) {

}

void ResourceManager::addResource(const QImage &image) {
    //todo:存入数据库
    listWidget->addItem("Image");
    images.append(image);
}

void ResourceManager::showContextMenu(const QPoint &pos) {
    QMenu contextMenu;
    QAction *editAction = new QAction("Edit", &contextMenu);
    connect(editAction, &QAction::triggered, this, &ResourceManager::editResource);
    contextMenu.addAction(editAction);
    contextMenu.exec(listWidget->mapToGlobal(pos));
}

void ResourceManager::editResource() {
    // Edit functionality: You can implement more complex editing here
    // For now, let's just print the selected resource
    QListWidgetItem *item = listWidget->currentItem();
    if (item) {
        qDebug() << "Editing resource:" << item->text();
    }
}

void ResourceManager::addHistoryItem(const QString &text,const QImage &image)
{
    ClipboardItemWidget *itemWidget= new ClipboardItemWidget(text,image, listWidget);
    QListWidgetItem *item = new QListWidgetItem(listWidget);
    item->setSizeHint(itemWidget->sizeHint());  // 必须！
    listWidget->setItemWidget(item, itemWidget);
    // 可选：item->setFlags(item->flags() | Qt::ItemIsSelectable);
}
