#include "ui_screen.h"
#include "ui_ui_screen.h"
#include<iostream>
#include <QNetworkReply>
#include<QFileDialog>
#include<QDir>
#include <QMap>
#include <QFileInfo>
#include<iostream>
#include<QFile>
#include <QClipboard>

ui_screen::ui_screen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ui_screen)
{

    ui->setupUi(this);
    initialise_variables();
    initialise_conditional_variables();

}

ui_screen::~ui_screen()
{
    delete ui;
    delete mgr;
    delete GISTER_SETTINGS;
}

void ui_screen::initialise_variables() {
    mgr = new QNetworkAccessManager(this);
    GISTER_SETTINGS = new QSettings(QDir::currentPath() + "/gister.ini", QSettings::IniFormat);
    ui->authenciation_progress->setVisible(false);
    ui->create_gist_progress->setVisible(false);
    ui->create_gist_progress_label->setVisible(false);
     danger = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );border-bottom-right-radius: 5px;border-bottom-left-radius: 5px;border: .px solid black;}";
     safe= "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #78d,stop: 0.4999 #46a,stop: 0.5 #45a,stop: 1 #238 );border-bottom-right-radius: 7px;border-bottom-left-radius: 7px;border: 1px solid black;}";
    ui->authenciation_progress->setStyleSheet(safe);
    ui->authenciation_progress_label->setVisible(false);

}

void ui_screen::initialise_conditional_variables() {
    //check for all conditional variables and alter the ui elements
    bool token_exists = GISTER_SETTINGS->contains(GISTER_API_TOKEN_KEY_NAME);
    if(token_exists){
        GISTER_API_TOKEN = GISTER_SETTINGS->value(GISTER_API_TOKEN_KEY_NAME).toString();
        ui->multiScreen->setCurrentIndex(ui_screen::GIST_SCREEN);
    }
    else {
        ui->multiScreen->setCurrentIndex(ui_screen::TOKEN_SCREEN);
    }
}

void ui_screen::on_pushButton_clicked()
{
    QString access_token = ui->access_token_field->text();
    if (!access_token.isNull() && !access_token.isEmpty()) {
        //show Progress Bar
        ui->authenciation_progress_label->setVisible(true);
        ui->authenciation_progress->setVisible(true);
        //make a Loading effect
        ui->authenciation_progress->setRange(0,0);
        //make a network request to the github API
        check_api_authenciation(access_token);
    }
}

//sends request to github API
void ui_screen::check_api_authenciation(const QString& access_token) {
    //construct the api url
    GISTER_API_TOKEN = access_token;
    QUrl api_validation_url(this->API_VALIDATION_URL + "?access_token=" + access_token);
    QNetworkRequest api_validation_request(api_validation_url);
    api_validation_request.setHeader(QNetworkRequest::ServerHeader, "Authorization: token " + access_token);
   mgr->get(api_validation_request);
    connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(on_authenciation_response_arrive(QNetworkReply*)));


}

void ui_screen::on_access_token_field_textChanged(const QString &arg1)
{
    ui->pushButton->setEnabled(true);
}

void ui_screen::on_authenciation_response_arrive(QNetworkReply* reply) {

    QByteArray bts = reply->readAll();
    QString str(bts);
    QJsonDocument d = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObject = d.object();
    if (jsonObject.contains("login")) {
        ui->authenciation_progress->setStyleSheet(safe);
        ui->authenciation_progress_label->setText("The token is valid");
        GISTER_SETTINGS->setValue(GISTER_API_TOKEN_KEY_NAME, GISTER_API_TOKEN);
        ui->multiScreen->setCurrentIndex(ui_screen::GIST_SCREEN);
    }
    else{
        ui->authenciation_progress->setStyleSheet(danger);
        ui->authenciation_progress_label->setText("The token is not valid");
    }

    ui->authenciation_progress->setRange(0,100);
    ui->authenciation_progress->setValue(100);

}



void ui_screen::on_add_files_to_list_clicked()
{
    QStringList choosen_files = QFileDialog::getOpenFileNames(this, "Choose Files to create gist", QDir::currentPath());

    ui->selected_gist_files_list->addItems(choosen_files);
}

void ui_screen::on_clear_selected_files_clicked()
{
    ui->selected_gist_files_list->clear();
}

void ui_screen::on_clear_single_selected_file_clicked()
{
   QList<QListWidgetItem*> items = ui->selected_gist_files_list->selectedItems();
   foreach(QListWidgetItem* item, items) {
    delete ui->selected_gist_files_list->takeItem(ui->selected_gist_files_list->row(item));
   }

}

void ui_screen::on_create_secret_gist_clicked()
{
    //secret gist
    create_gist(true);

}

void ui_screen::on_create_gist_clicked()
{
    //normal gist
    create_gist(false);
}
void ui_screen::create_gist(bool is_secret) {

    if (ui->selected_gist_files_list->count() > 0) {
        //get the data from list
        ui->create_gist_progress->setVisible(true);
        ui->create_gist_progress_label->setVisible(true);
        ui->create_gist_progress->setRange(0,0);
         QStringList files;
         for(int i = 0; i < ui->selected_gist_files_list->count(); ++i)
         {
             files << ui->selected_gist_files_list->item(i)->text();
             //Do stuff!
         }
        QMap<QString, QString>* files_data = new QMap<QString, QString>();
        foreach(QString file, files) {
            QFileInfo finfo(file);
            QString filename = finfo.fileName();
            QString contents = read_file_to_string(file);
            files_data->insert(filename, contents);
        }
        QJsonObject post_object;
        post_object.insert("description", "created by Gister(https://github.com/naveen17797/Gister)");
        if (is_secret) {
            post_object.insert("public", "false");
        }
        QJsonObject files_object;
        for (auto i = files_data->begin(); i != files_data->end(); i++) {
            QJsonObject content_object;
            content_object.insert("content", i.value());
            files_object.insert(i.key(), content_object);
        }
        post_object.insert("files", files_object);
        QUrl gist_creation_url(CREATE_GIST_URL + "?access_token=" + GISTER_API_TOKEN);
        QNetworkRequest create_gist_request(gist_creation_url);
        create_gist_request.setHeader(QNetworkRequest::ServerHeader, "Authorization: token " + GISTER_API_TOKEN);
        create_gist_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        mgr->post(create_gist_request,  QJsonDocument(post_object).toJson());
        connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(on_create_gist_response_arrive(QNetworkReply*)));

        //loop through the selected files list
        delete  files_data;

    }
    else {
        ui->create_gist_progress_label->setVisible(true);
        ui->create_gist_progress_label->setText("No files selected");
    }
}

QString ui_screen::read_file_to_string(QString& filename) {
    QString text_to_be_returned = "GISTER_ERROR:NO_ACCESS_TO_FILE";
    QFile f(filename);
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream t(&f);
        text_to_be_returned = t.readAll();
        f.close();
    }
    return text_to_be_returned;
}

void ui_screen::on_create_gist_response_arrive(QNetworkReply* reply) {
    QByteArray bts = reply->readAll();
    QString str(bts);
    QJsonDocument d = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObject = d.object();
    if (jsonObject.contains("html_url")) {
        ui->selected_gist_files_list->clear();
        ui->create_gist_progress->setVisible(false);
        ui->create_gist_progress_label->setVisible(false);
        QString gist_url = jsonObject.value("html_url").toString();
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(gist_url);
        ui->gist_html_url->setText(gist_url);
        ui->multiScreen->setCurrentIndex(ui_screen::SHARE_LINK_SCREEN);


    }
    else {
        ui->create_gist_progress->setRange(0,100);
        ui->create_gist_progress->setStyleSheet(danger);
        ui->create_gist_progress->setValue(100);
        ui->create_gist_progress_label->setText("Gist creation failed");

    }

}

void ui_screen::on_create_another_gist_clicked()
{
    ui->multiScreen->setCurrentIndex(ui_screen::GIST_SCREEN);
}

void ui_screen::drawLinearGradient() {
    mgr = new QNetworkAccessManager(this);
    GISTER_SETTINGS = new QSettings(QDir::currentPath() + "/gister.ini", QSettings::IniFormat);
    ui->authenciation_progress->setVisible(false);
    ui->create_gist_progress->setVisible(false);
    ui->create_gist_progress_label->setVisible(false);
     danger = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );border-bottom-right-radius: 5px;border-bottom-left-radius: 5px;border: .px solid black;}";
     safe= "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #78d,stop: 0.4999 #46a,stop: 0.5 #45a,stop: 1 #238 );border-bottom-right-radius: 7px;border-bottom-left-radius: 7px;border: 1px solid black;}";

}