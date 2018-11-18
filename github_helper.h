#ifndef GITHUB_HELPER_H
#define GITHUB_HELPER_H
#include<QString>
#include<QtNetwork/QNetworkAccessManager>
#include<QtNetwork/QNetworkRequest>
class github_helper
{
public:
    github_helper();
    bool is_access_token_valid(QString& access_token);
};

#endif // GITHUB_HELPER_H
