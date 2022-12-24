#include "unlockerdlg.h"

#define ToStr(qstr) (qstr.toLocal8Bit().toStdString().c_str())

UnlockerDlg::UnlockerDlg(QWidget* parent) :
	QDialog(parent)
{
	ui.setupUi(this);
}


int UnlockerDlg::DoModule(QWidget* parent)
{
	UnlockerDlg dlg{ parent };
	return dlg.exec();
}


void UnlockerDlg::on_btn_open_in_path_clicked()
{
    auto file_path = QFileDialog::getOpenFileName(this, "选择文件", getInitOpenFileDir(), "应用程序(*.exe)");
    if (file_path.isEmpty()) return;
    ui.edit_in_path->setText(file_path);
    ui.edit_out_path->setText(file_path);
}


void UnlockerDlg::on_btn_open_out_path_clicked()
{
    auto file_path = QFileDialog::getOpenFileName(this, "选择文件", getInitOpenFileDir(), "应用程序(*.exe)");
    if (file_path.isEmpty()) return;
    ui.edit_out_path->setText(file_path);
}


void UnlockerDlg::on_btn_begin_unlock_clicked()
{
    try
    {
        if (!check()) return;
        DWORD targetSize;
        auto targetBuf = lxutil::ReadBuffer(ToStr(ui.edit_in_path->text()), &targetSize);
        if (!targetBuf)
            return criticalBox("无法读取文件[" + ui.edit_in_path->text() + "]的内存!");

        bool suc{};
        if (lxutil::Is32BitSoftware(targetBuf))
        {
            lxutil::XHacker32 hacker{ targetBuf, targetSize };
            auto resPE = hacker.UninjectEntryCode();
            suc = lxutil::SaveBuffer(ToStr(ui.edit_out_path->text()), resPE.rdbuf(), resPE.fileSize());
        }
        else
        {
            lxutil::XHacker64 hacker{ targetBuf, targetSize };
            auto resPE = hacker.UninjectEntryCode();
            suc = lxutil::SaveBuffer(ToStr(ui.edit_out_path->text()), resPE.rdbuf(), resPE.fileSize());
        }

        if (!suc)
            return criticalBox("无法将解锁后的文件保存到[" + ui.edit_out_path->text() + "]!");
        else
            return infomationBox("解锁成功!");
    }
    catch (const std::exception& ex)
    {
        criticalBox(QString("解锁失败, 意外的错误: ") + ex.what());
    }
    catch (const int ex)
    {
        switch (ex)
        {
        case lxutil::NO_INJECTED_CODE_ERROR:
        {
            warningBox("文件[" + ui.edit_in_path->text() + "] 没有被加过锁, 或不是本程序的锁!");
            break;
        }
        case lxutil::LAST_SCN_CHANGED_ERROR:
        {
            criticalBox("文件[" + ui.edit_in_path->text() + "] 被进行了未知的修改, 解锁失败!");
            break;
        }
        }
    }
}


bool UnlockerDlg::check()
{
    QFile file1(ui.edit_in_path->text());
    if (!file1.exists())
    {
        criticalBox("路径[" + ui.edit_in_path->text() + "]不存在!");
        return false;
    }
    QFile file2(ui.edit_out_path->text());
    if (!file2.exists())
    {
        criticalBox("路径[" + ui.edit_out_path->text() + "]不存在!");
        return false;
    }
    return true;
}


inline QString UnlockerDlg::getInitOpenFileDir() noexcept
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