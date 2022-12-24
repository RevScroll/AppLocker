#pragma once

#include <QtWidgets/QMainWindow>
#include <QProcess>
#include <vector>
#include <QSettings>
#include "ui_applocker.h"
#include "unlockerdlg.h"
#include "usertipdlg.h"

#pragma execution_character_set("utf-8")

typedef struct _IMAGE_LOCKER_WIN_HEADER
{
    BYTE Reserved1[5];
    BYTE JmpBack[5];
    BYTE Resvered[2];
    CHAR SuccessTip[64];
    CHAR SuccessTitle[64];
    CHAR FalidTitle[64];
    CHAR FalidTip[64];
    CHAR MainTitle[16];
    CHAR LogBtn[16];
    CHAR CancelBtn[16];
    CHAR AccLab[16];
    CHAR PwdLab[16];
    CHAR AccAnswer[64];
    CHAR PwdAnswer[64];
    DWORD MainWinX;
    DWORD MainWinY;
}IMAGE_LOCKER_WIN_HEADER, *PIMAGE_LOCKER_WIN_HEADER;

static LPCSTR SettingsPath = "./plugins/settings.ini";
static LPCSTR DefSettingsPath = "./plugins/default_settings.ini";
static LPCSTR LockWin32Path = "./plugins/LockWin32.exe";
static LPCSTR LockWin64Path = "./plugins/LockWin64.exe";
static LPCSTR PrevBufPath = "./plugins/preview.exe";

class AppLocker : public QMainWindow
{
    Q_OBJECT

public:
    AppLocker(QWidget *parent = Q_NULLPTR);
    QString getInitOpenFileDir() noexcept;

private slots:
    void on_btn_open_in_path_clicked();
    void on_btn_open_out_path_clicked();
    void on_btn_prev_dlg_clicked();
    void on_btn_begin_lock_clicked();
    void on_act_unlock_triggered();
    void on_act_useTip_triggered();
    void on_act_about_triggered();
    void on_act_warning_triggered();
    void on_act_restore_triggered();
    void on_act_checkUpdate_triggered();

private:
    void closeEvent(QCloseEvent* event) override;
    void saveSettings(QString iniPath);
    void readSettings(QString iniPath);
    bool check();
    bool checkFiles();
    inline bool isOutOfBound(const QString& str, __int32 bound) const noexcept
    {
        return str.toLocal8Bit().size() > bound;
    }
    inline void infomationBox(const QString& text) noexcept
    {
        QMessageBox::information(this, "Ã· æ", text);
    }
    inline void warningBox(const QString& text) noexcept
    {
        QMessageBox::warning(this, "æØ∏Ê", text);
    }
    inline void criticalBox(const QString& text) noexcept
    {
        QMessageBox::critical(this, "¥ÌŒÛ", text);
    }
    void mapDataToBuf(PIMAGE_LOCKER_WIN_HEADER header);
    Ui::AppLockerClass ui;
    QProcess* procer;
};