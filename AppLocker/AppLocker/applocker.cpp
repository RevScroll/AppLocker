#include "applocker.h"

#define ToStr(qstr) (qstr.toLocal8Bit().toStdString().c_str())

AppLocker::AppLocker(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    procer = new QProcess();
    readSettings(SettingsPath);
}


inline QString AppLocker::getInitOpenFileDir() noexcept
{
    auto path = ui.edit_in_path->text();
    if (path.isEmpty())
        path = ui.edit_out_path->text();
    QFile file(path);
    if (file.exists())
    {
        QFileInfo info(path);
        path = info.absolutePath();
    }
    else
    {
        path = QDir::currentPath();
    }
    return path;
}

/// <summary>
/// 打开输入文件
/// </summary>
void AppLocker::on_btn_open_in_path_clicked()
{
    auto file_path = QFileDialog::getOpenFileName(this, "选择文件", getInitOpenFileDir(), "应用程序(*.exe)");
    if (file_path.isEmpty()) return;
    ui.edit_in_path->setText(file_path);
    ui.edit_out_path->setText(file_path);
}

/// <summary>
/// 打开输出文件
/// </summary>
void AppLocker::on_btn_open_out_path_clicked()
{
    auto file_path = QFileDialog::getOpenFileName(this, "选择文件", getInitOpenFileDir(), "应用程序(*.exe)");
    if (file_path.isEmpty()) return;
    ui.edit_out_path->setText(file_path);
}

/// <summary>
/// 预览效果
/// </summary>
void AppLocker::on_btn_prev_dlg_clicked()
{
    if (!check()) return;

    DWORD size{};
    auto buffer = lxutil::ReadBuffer(LockWin32Path, &size);
    if (!buffer) return criticalBox(QString("无法读取") + LockWin32Path);
    lxutil::PEAnalyser32 pe(buffer, size);
    auto data = (PIMAGE_LOCKER_WIN_HEADER)(pe.rdbuf() + pe.lpFirstScnHeader->PointerToRawData);
    mapDataToBuf(data);
    lxutil::SaveBuffer(PrevBufPath, buffer, size);
    procer->start(PrevBufPath);
    if (!procer->waitForStarted()) return criticalBox("创建预览框进程失败!");
}

/// <summary>
/// 开始加锁
/// </summary>
void AppLocker::on_btn_begin_lock_clicked()
{
    try
    {
        if (!check() || !checkFiles()) return;

        DWORD targetBufSize{};
        auto targetBuf = lxutil::ReadBuffer(ToStr(ui.edit_in_path->text()), &targetBufSize);
        if (!targetBuf)
            return criticalBox("无法读取文件[" + ui.edit_in_path->text() + "]的内存!");
        LPSTR buf = nullptr;
        DWORD size{};
        bool suc{};
        if (lxutil::Is32BitSoftware(targetBuf))
        {
            lxutil::PEAnalyser32 pe(LockWin32Path);
            auto data = (PIMAGE_LOCKER_WIN_HEADER)(pe.rdbuf() + pe.lpFirstScnHeader->PointerToRawData);
            mapDataToBuf(data);
            lxutil::XHacker32 hacker{ targetBuf, targetBufSize };
            auto res = hacker.injectEntryCode(data, pe.fileSize(), 5, 0);
            suc = lxutil::SaveBuffer(ToStr(ui.edit_out_path->text()), res.rdbuf(), res.fileSize());
        }
        else
        {
            lxutil::PEAnalyser64 pe(LockWin64Path);
            auto data = (PIMAGE_LOCKER_WIN_HEADER)(pe.rdbuf() + (pe.lpFirstScnHeader + 1)->PointerToRawData);
            data->JmpBack[0] = 0xE9;
            mapDataToBuf(data);

            lxutil::PEAnalyser64 peImg(pe.fileBufToImageBuf(), pe.lpOptHeader->SizeOfImage);
            lxutil::XHacker64 hacker{ targetBuf, targetBufSize };
            auto res = hacker.injectEntryCode(
                peImg.rdbuf() + peImg.lpFirstScnHeader->VirtualAddress,
                peImg.fileSize(),
                (peImg.lpFirstScnHeader + 1)->VirtualAddress + 5 - peImg.lpFirstScnHeader->VirtualAddress,
                0);
            suc = lxutil::SaveBuffer(ToStr(ui.edit_out_path->text()), res.rdbuf(), res.fileSize());
        }
        if (!suc)
            return criticalBox("无法将加锁后的文件保存到[" + ui.edit_out_path->text() + "]!");
        else
            return infomationBox("加锁成功!");
    }
    catch (const std::exception& ex)
    {
        criticalBox(QString("加锁失败, 意外的错误: ") + ex.what());
    }
}


void AppLocker::on_act_unlock_triggered()
{
    UnlockerDlg::DoModule(this);
}


void AppLocker::on_act_useTip_triggered()
{
    UserTipDlg::DoModule(this);
}


void AppLocker::on_act_about_triggered()
{
    LPCSTR text =
        "名称: AppLocker\n"
        "版本: 1.0.0.0\n"
        "作者: 琉璃胧曦\n\n"
        "bilibili UID:169527696\n"
        "有事或者有bug可以找我(乛 з乛)";
    QMessageBox::information(this, "关于", text);
}


void AppLocker::on_act_warning_triggered()
{
    LPCSTR text =
        "1. 正在运行中的程序无法加锁\n\n"
        "2. 部分程序加锁后可能无法正常运行,\n"
        "这是因为其采用了较高级别的保护程序\n"
        "会自检, 检测出自身有未知代码(加锁框)\n"
        "不过这种程序很少就是了\n\n"
        "3. 如果不想再加锁了, 点击选项\"工具\",\n"
        "再点击\"去除加锁框\"";
    QMessageBox::information(this, "注意事项", text);
}


void AppLocker::on_act_restore_triggered()
{
    readSettings(DefSettingsPath);
}


void AppLocker::on_act_checkUpdate_triggered()
{

}


void AppLocker::closeEvent(QCloseEvent* event)
{
    saveSettings(SettingsPath);
}


void AppLocker::saveSettings(QString iniPath)
{
    QSettings set{ iniPath , QSettings::IniFormat };
    set.setValue("MainWinTitle", ui.edit_mainwin_title->text());
    set.setValue("CX", ui.spinBox_mainX->value());
    set.setValue("CY", ui.spinBox_mainY->value());
    set.setValue("FmtEditName", ui.edit_fmtedit_name->text());
    set.setValue("PwdEditName", ui.edit_pwdedit_name->text());
    set.setValue("FmtEditAns", ui.edit_fmtedit_answer->text());
    set.setValue("PwdEditAns", ui.edit_pwdedit_answer->text());
    set.setValue("OKBtnName", ui.edit_ok_btn_name->text());
    set.setValue("ExitBtnName", ui.edit_exit_btn_name->text());
    set.setValue("ScsTitle", ui.edit_scsdlg_title->text());
    set.setValue("ScsText", ui.edit_scsdlg_text->text());
    set.setValue("FalidTitle", ui.edit_faliddlg_title->text());
    set.setValue("FalidText", ui.edit_faliddlg_text->text());
}


void AppLocker::readSettings(QString iniPath)
{
    QFile file(iniPath);
    if (!file.exists()) return criticalBox("配置文件[" + iniPath + "]不存在!");
    QSettings set{ iniPath , QSettings::IniFormat };
    ui.edit_mainwin_title->setText(set.value("MainWinTitle").toString());
    ui.spinBox_mainX->setValue(set.value("CX").toInt());
    ui.spinBox_mainY->setValue(set.value("CY").toInt());
    ui.edit_fmtedit_name->setText(set.value("FmtEditName").toString());
    ui.edit_pwdedit_name->setText(set.value("PwdEditName").toString());
    ui.edit_fmtedit_answer->setText(set.value("FmtEditAns").toString());
    ui.edit_pwdedit_answer->setText(set.value("PwdEditAns").toString());
    ui.edit_ok_btn_name->setText(set.value("OKBtnName").toString());
    ui.edit_exit_btn_name->setText(set.value("ExitBtnName").toString());
    ui.edit_scsdlg_title->setText(set.value("ScsTitle").toString());
    ui.edit_scsdlg_text->setText(set.value("ScsText").toString());
    ui.edit_faliddlg_title->setText(set.value("FalidTitle").toString());
    ui.edit_faliddlg_text->setText(set.value("FalidText").toString());
}


bool AppLocker::check()
{
    if (
        isOutOfBound(ui.edit_mainwin_title->text(), 15) ||
        isOutOfBound(ui.edit_fmtedit_answer->text(), 63) ||
        isOutOfBound(ui.edit_pwdedit_answer->text(), 63) ||
        isOutOfBound(ui.edit_scsdlg_text->text(), 63) ||
        isOutOfBound(ui.edit_scsdlg_title->text(), 63) ||
        isOutOfBound(ui.edit_faliddlg_text->text(), 63) ||
        isOutOfBound(ui.edit_faliddlg_title->text(), 63) ||
        isOutOfBound(ui.edit_fmtedit_name->text(), 15) ||
        isOutOfBound(ui.edit_pwdedit_name->text(), 15) ||
        isOutOfBound(ui.edit_ok_btn_name->text(), 15) ||
        isOutOfBound(ui.edit_exit_btn_name->text(), 15)
        )
    {
        criticalBox("[加锁框参数]有一个或多个项的字符大小超过了规定字节!");
        return false;
    }
    return true;
}


bool AppLocker::checkFiles()
{
    QFile file1(ui.edit_in_path->text());
    if (!file1.exists())
    {
        criticalBox("路径[" + ui.edit_in_path->text() + "]不存在!");
        return false;
    }
    try
    {
        lxutil::PEAnalyser32 pe1{ LockWin32Path };
    }
    catch (const int ex)
    {
        switch (ex)
        {
        case lxutil::READ_BUFFER_ERROR:
        {
            criticalBox(QString("无法打开[") + LockWin32Path + "]进行内存读取!");
            break;
        }
        case lxutil::LOAD_STRUCT_ERROR:
        {
            criticalBox(QString("文件[") + LockWin32Path + "]疑似损坏!");
            break;
        }
        }
        return false;
    }
    try
    {
        lxutil::PEAnalyser64 pe1{ LockWin64Path };
    }
    catch (const int ex)
    {
        switch (ex)
        {
        case lxutil::READ_BUFFER_ERROR:
        {
            criticalBox(QString("无法打开[") + LockWin64Path + "]进行内存读取!");
            break;
        }
        case lxutil::LOAD_STRUCT_ERROR:
        {
            criticalBox(QString("文件[") + LockWin64Path + "]疑似损坏!");
            break;
        }
        }
        return false;
    }
    return true;
}


void AppLocker::mapDataToBuf(PIMAGE_LOCKER_WIN_HEADER header)
{
    auto temp = ui.edit_mainwin_title->text().toLocal8Bit();
    memcpy(header->MainTitle, temp.data(), temp.size());
    temp = ui.edit_fmtedit_answer->text().toLocal8Bit();
    memcpy(header->AccAnswer, temp.data(), temp.size());
    temp = ui.edit_fmtedit_name->text().toLocal8Bit();
    memcpy(header->AccLab, temp.data(), temp.size());
    temp = ui.edit_pwdedit_answer->text().toLocal8Bit();
    memcpy(header->PwdAnswer, temp.data(), temp.size());
    temp = ui.edit_pwdedit_name->text().toLocal8Bit();
    memcpy(header->PwdLab, temp.data(), temp.size());
    temp = ui.edit_ok_btn_name->text().toLocal8Bit();
    memcpy(header->LogBtn, temp.data(), temp.size());
    temp = ui.edit_exit_btn_name->text().toLocal8Bit();
    memcpy(header->CancelBtn, temp.data(), temp.size());
    temp = ui.edit_scsdlg_text->text().toLocal8Bit();
    memcpy(header->SuccessTip, temp.data(), temp.size());
    temp = ui.edit_scsdlg_title->text().toLocal8Bit();
    memcpy(header->SuccessTitle, temp.data(), temp.size());
    temp = ui.edit_faliddlg_text->text().toLocal8Bit();
    memcpy(header->FalidTip, temp.data(), temp.size());
    temp = ui.edit_faliddlg_title->text().toLocal8Bit();
    memcpy(header->FalidTitle, temp.data(), temp.size());
    header->MainWinX = ui.spinBox_mainX->value();
    header->MainWinY = ui.spinBox_mainY->value();
}
