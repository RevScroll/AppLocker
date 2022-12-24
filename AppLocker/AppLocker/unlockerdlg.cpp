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
    auto file_path = QFileDialog::getOpenFileName(this, "ѡ���ļ�", getInitOpenFileDir(), "Ӧ�ó���(*.exe)");
    if (file_path.isEmpty()) return;
    ui.edit_in_path->setText(file_path);
    ui.edit_out_path->setText(file_path);
}


void UnlockerDlg::on_btn_open_out_path_clicked()
{
    auto file_path = QFileDialog::getOpenFileName(this, "ѡ���ļ�", getInitOpenFileDir(), "Ӧ�ó���(*.exe)");
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
            return criticalBox("�޷���ȡ�ļ�[" + ui.edit_in_path->text() + "]���ڴ�!");

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
            return criticalBox("�޷�����������ļ����浽[" + ui.edit_out_path->text() + "]!");
        else
            return infomationBox("�����ɹ�!");
    }
    catch (const std::exception& ex)
    {
        criticalBox(QString("����ʧ��, ����Ĵ���: ") + ex.what());
    }
    catch (const int ex)
    {
        switch (ex)
        {
        case lxutil::NO_INJECTED_CODE_ERROR:
        {
            warningBox("�ļ�[" + ui.edit_in_path->text() + "] û�б��ӹ���, ���Ǳ��������!");
            break;
        }
        case lxutil::LAST_SCN_CHANGED_ERROR:
        {
            criticalBox("�ļ�[" + ui.edit_in_path->text() + "] ��������δ֪���޸�, ����ʧ��!");
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
        criticalBox("·��[" + ui.edit_in_path->text() + "]������!");
        return false;
    }
    QFile file2(ui.edit_out_path->text());
    if (!file2.exists())
    {
        criticalBox("·��[" + ui.edit_out_path->text() + "]������!");
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