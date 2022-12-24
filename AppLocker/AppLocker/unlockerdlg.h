#pragma once

#include <QDialog>
#include <QMessageBox>
#include <Windows.h>
#include <QFileDialog>
#include <QDir>
#include "XHacker/xHacker.h"
#include "ui_unlockerdlg.h"

#pragma execution_character_set("utf-8")

class UnlockerDlg : QDialog
{
	Q_OBJECT

public:
	UnlockerDlg(QWidget* parent);
	static int DoModule(QWidget* parent);
	QString UnlockerDlg::getInitOpenFileDir() noexcept;

private slots:
	void on_btn_open_in_path_clicked();
	void on_btn_open_out_path_clicked();
	void on_btn_begin_unlock_clicked();

private:
	Ui::UnLockerDlgClass ui;
	bool check();
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
};