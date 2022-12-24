#include "usertipdlg.h"

UserTipDlg::UserTipDlg(QWidget* parent) :
	QDialog(parent)
{
	ui.setupUi(this);
}

int UserTipDlg::DoModule(QWidget* parent)
{
	UserTipDlg dlg{ parent };
	return dlg.exec();
}
