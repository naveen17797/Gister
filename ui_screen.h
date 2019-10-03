#ifndef UI_SCREEN_H
#define UI_SCREEN_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include<QSettings>
namespace Ui {
class ui_screen;
}

class ui_screen : public QMainWindow
{
    Q_OBJECT

public:
    explicit ui_screen(QWidget *parent = nullptr);
        QNetworkAccessManager* mgr;
        enum {
            TOKEN_SCREEN,
            GIST_SCREEN,
            SHARE_LINK_SCREEN
        };
    ~ui_screen();

private slots:
    void on_pushButton_clicked();
    void on_access_token_field_textChanged(const QString &arg1);
    void on_authenciation_response_arrive(QNetworkReply* reply);
     void on_create_gist_response_arrive(QNetworkReply* reply);
    void on_add_files_to_list_clicked();

    void on_clear_selected_files_clicked();

    void on_clear_single_selected_file_clicked();

    void on_create_secret_gist_clicked();

    void on_create_gist_clicked();

    void on_create_another_gist_clicked();
    void ui_screen::drawLinearGradient();

private:
    QString GITHUB_API_URL = "https://api.github.com/";
    QString API_VALIDATION_URL = GITHUB_API_URL + "user";
    QString CREATE_GIST_URL  = GITHUB_API_URL + "gists";
    Ui::ui_screen *ui;
    void check_api_authenciation(const QString& access_token);
    QString danger;
    QString safe;
    void initialise_variables();
    void initialise_conditional_variables();
    QSettings* GISTER_SETTINGS;
    QString GISTER_API_TOKEN_KEY_NAME = "GISTER_API_TOKEN";
    QString GISTER_API_TOKEN;
    void create_gist(bool is_secret);
    QString read_file_to_string(QString& filename);
};

#endif // UI_SCREEN_H
