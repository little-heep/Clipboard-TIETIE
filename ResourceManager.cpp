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
#include <QSettings>

#include "MyDialogs.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

ResourceManager::ResourceManager(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("TIETIE");
    resize(400, 400);
    positionToTopRight();
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    // UI Setup
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainlayout = new QVBoxLayout(centralWidget);

    //上部：标题＋搜索框
    topWidget = new QWidget();
    topLayout = new QVBoxLayout(topWidget);
    titleLabel = new QLabel();
    QHBoxLayout *hlayout = new QHBoxLayout();
    serchLabel = new QLineEdit();
    serchLabel->setPlaceholderText("⌕ /t+搜索标签...");  // 提示文字
    serchLabel->setClearButtonEnabled(true);       // 启用 Qt 自带的小叉清空按钮
    connect(serchLabel, &QLineEdit::textChanged, this, &ResourceManager::onSearchTextChanged);
    settingbtn=new QPushButton();
    hlayout->addWidget(serchLabel);
    hlayout->addWidget(settingbtn);
    hlayout->setSpacing(5);
    topLayout->addWidget(titleLabel);
    topLayout->addLayout(hlayout);
    //设置菜单
    QMenu *settingsMenu = new QMenu(this);
    QAction *actionPreferences = new QAction("偏好设置", this);
    QAction *actionClearHistory = new QAction("清空历史记录", this);
    QAction *actionAbout = new QAction("关于", this);
    QAction *actionExit = new QAction("退出程序", this);
    settingsMenu->addAction(actionPreferences);
    settingsMenu->addAction(actionClearHistory);
    settingsMenu->addSeparator(); // 分割线
    settingsMenu->addAction(actionAbout);
    settingsMenu->addAction(actionExit);
    settingsMenu->setStyleSheet(R"(
    QMenu {
        background-color: white;       /* 整个菜单的背景色 */
        border: 2px solid #8c8c8c;    /* 菜单外边框 */
        padding: 5px;                  /* 菜单内边距 */
    }
    QMenu::item {
        background-color: transparent;
        padding: 6px 25px 6px 20px;   /* 增加间距，让菜单看起来不拥挤 */
        color: black;                  /* 默认文字颜色 */
        border-radius: 4px;            /* 每一项的圆角 */
    }
    /* 关键部分：鼠标悬停（选中）时的样式 */
    QMenu::item:selected {
        background-color: #f0f0f0;    /* 悬停时的背景色（浅灰色） */
        color: #000000;                /* 悬停时的文字颜色（强制黑色） */
    }
    QMenu::separator {
        height: 1px;
        background: #d0d0d0;           /* 分割线颜色 */
        margin: 5px 10px;
    }
    )");

    settingbtn->setMenu(settingsMenu);
    connect(actionClearHistory, &QAction::triggered, this, &ResourceManager::onClearAll);
    connect(actionExit, &QAction::triggered, qApp, &QApplication::quit);
    connect(actionPreferences, &QAction::triggered, this, &ResourceManager::onPreferenceUpdated);
    connect(actionAbout, &QAction::triggered, this, &ResourceManager::onAbout);

    //下部：剪贴历史记录
    historyArea = new QWidget();
    historyLayout = new QVBoxLayout(historyArea);
    listWidget = new QListWidget();
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    historyLayout->addWidget(listWidget);

    mainlayout->addWidget(topWidget);
    mainlayout->addWidget(historyArea, 1);
    setCentralWidget(centralWidget);

    // 热键
    QSettings settings;
    QString savedKey = settings.value("hotkey", "Ctrl+N").toString(); // 默认为 Ctrl+N
    hotkey = new QHotkey(QKeySequence(savedKey), true, this);
    connect(hotkey, &QHotkey::activated, this, &ResourceManager::toggleWindow);
    // 增加内容
    clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &ResourceManager::onClipboardChanged);
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
    settingbtn->setFixedWidth(26);
    settingbtn->setIcon(QIcon(":/image/setting.png"));
    settingbtn->setIconSize(QSize(16, 16));
    settingbtn->setStyleSheet(R"(
    QPushButton {
        border: 2px outset #8c8c8c;
        border-image: none;
        background-color: white;
        color: black;
        padding: 0px; /* 确保 icon 居中 */
    }

    /* 关键代码：去掉右下角的小箭头 */
    QPushButton::menu-indicator {
        width:0px;
        image: none;
    }

    QPushButton:hover {
        background-color: #f8f8f8;
    }
    )");


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
    QListWidgetItem *item = listWidget->currentItem();
    if (!item) return;

    QString text = item->data(Qt::UserRole).toString();
    QImage image = item->data(Qt::UserRole + 1).value<QImage>();
    // 如果都没有内容，直接返回
    if (text.isEmpty() && image.isNull()) return;

    // 2. 将内容放入系统剪贴板（必须屏蔽信号，防止自己的程序又把这条记录抓取一遍）
    clipboard->blockSignals(true);
    // 判断优先放入图片还是文字
    if (!image.isNull()) {
        clipboard->setImage(image); // 放入图片
    } else {
        clipboard->setText(text);   // 放入文字
    }
    clipboard->blockSignals(false);

    // 3. 隐藏当前窗口。这一步极其关键！
    // 隐藏后，Windows 会自动把焦点还给上一个处于激活状态的窗口（比如你刚才正在打字的 Word）
    this->hide();

    // 4. 模拟按下 Ctrl + V
#ifdef Q_OS_WIN
    // 稍微延时一下，等待 Windows 完成焦点切换（50~100毫秒一般足够）
    Sleep(80);

    // 模拟按下 Ctrl 键
    keybd_event(VK_CONTROL, 0, 0, 0);
    // 模拟按下 V 键
    keybd_event('V', 0, 0, 0);
    // 模拟释放 V 键
    keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
    // 模拟释放 Ctrl 键
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
#endif

    // 取消选中状态，方便下次点击
    listWidget->clearSelection();
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
    item->setData(Qt::UserRole, text);
    item->setData(Qt::UserRole + 1, image);
    item->setData(Qt::UserRole + 2, tag);
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
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        auto *widget = qobject_cast<ClipboardItemWidget*>(listWidget->itemWidget(item));

        if (widget && widget->getRecordId() == recordId) {
            // 找到对应的 item，更新它的标签数据
            item->setData(Qt::UserRole + 2, newTag);
            break;
        }
    }
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

void ResourceManager::onClearAll() {
    listWidget->clear();
    DatabaseManager::instance().clearAll();
}
void ResourceManager::onPreferenceUpdated() {
    SettingsDialog dlg(this);
    // 先显示当前热键
    dlg.keyEdit->setText(hotkey->shortcut().toString());
    // 初始化开机自启 CheckBox 的勾选状态
    dlg.autoStartCheckBox->setChecked(this->checkAutoStart());

    if (dlg.exec() == QDialog::Accepted) {
        QString newKey = dlg.keyEdit->text();
        if (newKey.isEmpty()) return;

        // 1. 更新 QHotkey
        hotkey->setShortcut(QKeySequence(newKey), true);

        // 2. 保存到注册表/配置文件
        QSettings settings;
        settings.setValue("hotkey", newKey);

        qDebug() << "热键已更新为:" << newKey;

        // 保存开机自启设置
        bool isAutoStart = dlg.autoStartCheckBox->isChecked();
        this->setAutoStart(isAutoStart);
    }
}
void ResourceManager::onAbout() {
    AboutDialog dlg(this);
    dlg.exec();
}
void ResourceManager::mousePressEvent(QMouseEvent *event)
{
    // 只有鼠标左键按下时才允许拖动
    if (event->button() == Qt::LeftButton) {
        // 判断鼠标按下的位置是否在顶部标题栏区域内 (例如 topWidget 的高度范围内)
        if (event->pos().y() <= topWidget->height()) {
            m_isDragging = true;
            // 计算鼠标点击位置与窗口左上角的相对偏移量
            m_dragPosition = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
            event->accept();
        }
    }
}

void ResourceManager::mouseMoveEvent(QMouseEvent *event)
{
    // 如果处于拖动状态，且鼠标左键是按下的
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        // 将窗口移动到鼠标当前位置减去最初的偏移量
        this->move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void ResourceManager::mouseReleaseEvent(QMouseEvent *event)
{
    // 鼠标松开时，停止拖动
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
}

void ResourceManager::setAutoStart(bool enable) {
    // 注册表启动项的路径
    QString regPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    QSettings settings(regPath, QSettings::NativeFormat);

    // 你程序的唯一标识名称（英文字母即可）
    QString appName = "TIETIE_App";

    if (enable) {
        // 获取当前 exe 的绝对路径
        QString appPath = QCoreApplication::applicationFilePath();
        // 将路径中的 '/' 替换为 Windows 标准的 '\'
        appPath = QDir::toNativeSeparators(appPath);

        // 关键：如果路径中带有空格，注册表必须要用双引号把路径括起来，否则开机会报错
        QString value = "\"" + appPath + "\" --silent";

        // 写入注册表
        settings.setValue(appName, value);
        qDebug() << "已开启开机自启:" << value;
    } else {
        // 从注册表中移除
        settings.remove(appName);
        qDebug() << "已关闭开机自启";
    }
}

bool ResourceManager::checkAutoStart() {
    QString regPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    QSettings settings(regPath, QSettings::NativeFormat);

    QString appName = "TIETIE_App";

    // 检查注册表中是否存在这个键值
    return settings.contains(appName);
}

void ResourceManager::onSearchTextChanged(const QString &searchText)
{
    // 判断是否是纯标签搜索 (以 "/t" 开头)
    bool isTagSearch = searchText.startsWith("/t", Qt::CaseInsensitive);
    QString keyword = searchText;

    if (isTagSearch) {
        // 去掉 "/t" 前缀，并去除前后空格
        keyword = searchText.mid(2).trimmed();
    } else {
        keyword = searchText.trimmed();
    }

    // 遍历所有的列表项
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);

        // 取出我们之前绑定的文本和标签
        QString itemText = item->data(Qt::UserRole).toString();
        QString itemTag = item->data(Qt::UserRole + 2).toString();

        bool match = false;

        // 如果搜索框为空，全部显示
        if (keyword.isEmpty()) {
            match = true;
        }
        else if (isTagSearch) {
            // 如果是标签搜索，只匹配标签
            match = itemTag.contains(keyword, Qt::CaseInsensitive);
        }
        else {
            // 普通搜索，内容或标签包含关键字都算匹配
            match = itemText.contains(keyword, Qt::CaseInsensitive) ||
                    itemTag.contains(keyword, Qt::CaseInsensitive);
        }

        // 核心：如果不匹配就隐藏，匹配就显示
        item->setHidden(!match);
    }
}