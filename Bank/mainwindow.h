#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "admin.h"
#include <vector>
#include "listaadminow.h"
#include "listaklientow.h"
#include "listakont.h"
#include <QListWidgetItem>
#include "sesja.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    sesja *watek;

public slots:
    void on_zmiana_czas(int);

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_logout_adm_clicked();

    void on_pushButton_change_pass_adm_clicked();

    void on_pushButton_submit_pass_adm_clicked();

    void on_pushButton_add_person_clicked();

    void on_pushButton_add_admin_clicked();

    void on_pushButton_add_adm_clicked();

    void on_pushButton_edit_adm_clicked();

    void on_pushButton_change_acc_clicked();

    void on_pushButton_filtr_adm_clicked();

    void on_tableWidget_admins_cellDoubleClicked(int row, int column);

    void on_pushButton_edit_adm_submit_clicked();

    void on_pushButton_delete_adm_clicked();

    void on_pushButton_add_client_clicked();

    void on_pushButton_create_cl_clicked();

    void on_pushButton_cancel_tmp_clicked();

    void on_pushButton_login_tmp_clicked();

    void on_pushButton_logout_clicked();

    void on_pushButton_unlock_acc_clicked();

    void on_tableWidget_locked_acc_cellDoubleClicked(int row, int column);

    void on_pushButton_filtr_lock_acc_clicked();

    void on_pushButton_new_tmp_pw_clicked();

    void on_pushButton_change_pw_cl_clicked();

    void on_pushButton_account_clicked();

    void on_pushButton_create_b_acc_clicked();

    void on_pushButton_account_adm_clicked();

    void on_pushButton_manage_acc_clicked();

    void on_pushButton_filtr_lcl_acc__clicked();

    void on_tableWidget_cl_acc_cellDoubleClicked(int row, int column);

    void on_pushButton_b_acc_create_clicked();

    void on_pushButton_add_rem_bal_to_acc_clicked();

    void on_pushButton_back_sel_cl_clicked();

    void on_listWidget_accounts_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_edit_bal_clicked();

    void on_pushButton_manage_lokaty_clicked();

    void on_pushButton_create_lokata_clicked();

    void on_pushButton_submit_lok_clicked();

    void on_calendarWidget_clicked(const QDate &date);


    void on_pushButton_transfer_clicked();

    void on_listWidget_cl_acc_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_sub_trans_clicked();

    void on_pushButton_credit_sell_clicked();

    void on_pushButton_create_credit_clicked();

    void on_calendarWidget_2_currentPageChanged(int year, int month);

    void on_pushButton_cr_submit_clicked();

    void on_pushButton_cr_accept_clicked();

    void on_pushButton_acc_clicked();

    void on_pushButton_credit_clicked();

    void on_pushButton_loka_clicked();

    void on_pushButton_cycl_trans_clicked();

    void on_tableWidget_cl_credits_cellDoubleClicked(int row, int column);

    void on_pushButton_history_clicked();

    void on_listWidget_tr_hist_itemClicked(QListWidgetItem *item);

    void on_pushButton_session_2_clicked();

    void on_pushButton_session_clicked();

  //  void on_pushButton_workers_clicked();

  //  void on_pushButton_filtr_admins_clicked();

    void on_pushButton_edit_client_clicked();

    void on_pushButton_edit_cl_submit_clicked();

    void on_pushButton_workers_clicked();

    void on_pushButton_delete_cl_clicked();

    void on_pushButton_delt_b_acc_clicked();

    void on_pushButton_delete_lokata_clicked();

    void on_listWidget_lok_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_credit_del_clicked();

    void on_pushButton_exchange_clicked();

    void on_tableWidget_exchange_cellDoubleClicked(int row, int column);

    void on_pushButton_cur_sum_clicked();

    void on_pushButton_cur_submit_clicked();

private:
    Ui::MainWindow *ui;
    std::string id; //kto jest zalogowany
    ListaAdminow l_admini;
    ListaKlientow l_klienci;
    ListaKont l_konta;

};
#endif // MAINWINDOW_H
