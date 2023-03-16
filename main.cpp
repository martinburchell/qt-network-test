#include <QApplication>
#include <QByteArray>
#include <QDialog>
#include <QListIterator>
#include <QMapIterator>
#include <QPlainTextEdit>
#include <QPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSsl>
#include <QtNetwork/QSslConfiguration>
#include <QUrlQuery>
#include <QUuid>
#include <QVBoxLayout>
#include <QWidget>

class TestDialog : public QDialog
{
    Q_OBJECT
public:
    TestDialog(QWidget* parent) : QDialog(parent)
    {
        auto mainlayout = new QVBoxLayout();
        setLayout(mainlayout);
        m_editor = new QPlainTextEdit();
        m_editor->setReadOnly(true);
        m_editor->setTextInteractionFlags(Qt::NoTextInteraction);
        m_editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        mainlayout->addWidget(m_editor);

        QNetworkRequest request;
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setProtocol(QSsl::SecureProtocols);
        request.setSslConfiguration(config);
        QListIterator<QSslCertificate> cert_it(QSslConfiguration::systemCaCertificates());
        status_message("CA Certificates:");
        while (cert_it.hasNext()) {
            status_message(cert_it.next().toText());
        }

        QUrl url("https://webhook.site/e5b919b3-01dd-4f77-bbc3-f77b2be3ab68");
        request.setUrl(url);

        QMap<QString, QString> dict;
        dict["operation"] = "check_device_registered";
        dict["camcops_version"] = "2.4.15";
        dict["device"] = QUuid::createUuid().toString();

        QUrlQuery postdata;
        QMapIterator<QString, QString> dict_it(dict);
        while (dict_it.hasNext()) {
            dict_it.next();
            postdata.addQueryItem(QUrl::toPercentEncoding(dict_it.key()),
                                  QUrl::toPercentEncoding(dict_it.value()));
        }

        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/x-www-form-urlencoded");
        const QByteArray final_data = postdata.toString(QUrl::FullyEncoded).toUtf8();
        status_message("Request to server: " + final_data);
        status_message(&"... sending " [ final_data.length()]);

        auto mgr = new QNetworkAccessManager();
        QObject::connect(mgr, &QNetworkAccessManager::finished,
                         this, &TestDialog::reply);
        QObject::connect(mgr, &QNetworkAccessManager::sslErrors,
                         this, &TestDialog::ssl_errors);

        mgr->post(request, final_data);
    }

    void reply(QNetworkReply* reply)
    {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            status_message("Network failure: " + reply->errorString());
            return;
        }
        auto reply_data = reply->readAll();
        status_message(&"... received " [ reply_data.length()]);

        status_message("Network reply (raw): " + reply_data);
    }

    void ssl_errors(QNetworkReply *reply, const QList<QSslError> &errors)
    {
        status_message("SSL Errors:");
        QListIterator<QSslError> it(errors);
        while (it.hasNext()) {
            auto error = it.next();
            status_message(error.errorString());
        }
        QSslConfiguration config = reply->sslConfiguration();
        status_message("Peer certificate: " + config.peerCertificate().toText());
        QListIterator<QSslCertificate> cert_it(config.peerCertificateChain());
        status_message("Peer certificate chain:");
        while (cert_it.hasNext()) {
            status_message(cert_it.next().toText());
        }
    }

    void status_message(const QString& msg) const
    {
        m_editor->appendPlainText(msg);
    }

protected:
    QPointer<QPlainTextEdit> m_editor;
};

int main(int argc, char* argv[])
{
    QApplication app(argc,argv);

    TestDialog dialog(nullptr);
    dialog.exec();

    return app.exec();
}

#include "main.moc"
