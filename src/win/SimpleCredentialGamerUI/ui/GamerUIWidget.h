#pragma once

#include <QWidget>
#include "SimpleCredentialProvider.hpp"

class QPaintEvent;

namespace Ui {
class GamerUIWidget;
}

class GamerUIWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GamerUIWidget(QWidget *parent = nullptr);
    ~GamerUIWidget();
protected:
    void changeEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent * event) override;
    void closeEvent(QCloseEvent * event) override;
    /// Simply quits the app with the code 0 that tellsthat tells the
    /// credential provider to login with built-in Gamer user
    void allowUserToLogin();
    static bool saveCRMUserName(const QString & userName);
private slots:
    void switchLanguage();
    void initLanguageLabel();
    void onLogonClicked(bool toggled);
protected slots:
    void onForgotPasswordClicked(bool toggled);
    void onRegisterClicked(bool toggled);
    void onLogonByRegisterClicked(bool toggled);
private:
    Ui::GamerUIWidget *ui;
};

