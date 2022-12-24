#pragma once

#include <QDialog>
#include "ui_usertipdlg.h"

class UserTipDlg : QDialog
{
	Q_OBJECT

public:
	UserTipDlg(QWidget* parent);
	static int DoModule(QWidget* parent);

private:
	Ui::UserTipDlgClass ui;
};