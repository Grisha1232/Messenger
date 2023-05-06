#include <MNetworking/client/tcp_client.h>
#include <iostream>
#include <thread>
#include <QApplication>
#include <QBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QCloseEvent>
#include <QMessageBox>

using namespace Messenger;

class LoginDialog: public QDialog {
 public:
    LoginDialog(QWidget *parent = nullptr, TCPClient *client = nullptr) : QDialog(parent), client_(client) {
        setWindowTitle("Login");

        QLabel *error_label = new QLabel("", this);
        QLabel *username_label = new QLabel("Username:", this);
        username_edit_ = new QLineEdit(this);

        QPushButton *login_button = new QPushButton("Login", this);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(error_label);
        layout->addWidget(username_label);
        layout->addWidget(username_edit_);
        layout->addWidget(login_button);

        client_->onMessage = [this, error_label](const std::string& message){
          std::cout << message << "\n";
          if (message.find("ok") != std::string::npos) {
              accept();
          } else {
              error_label->setText(QString::fromStdString("this username not allowed"));
          }
        };

        connect(login_button, &QPushButton::clicked, this, [this](){
            std::cout << "button clicked\n";
            std::stringstream name;
            name << username_edit_->text().toStdString();
            client_->postMessage("--login--" + name.str() + "\n");
        });
    }

 private:
    TCPClient *client_;
    QLineEdit *username_edit_;
};

class MainWindow: public QWidget {
 public:
    std::thread t;

    MainWindow() : client_("localhost", 12345) {
        setWindowTitle("Messenger");

        t = std::thread( [this](){ client_.run(); });

        LoginDialog dialog(nullptr, &client_);
        if (dialog.exec() != QDialog::Accepted) {
            std::cout << "closed\n";
            close();
            return;
        }
        std::cout << "continue\n";


        auto *chat_log = new QTextBrowser(this);
        auto *message_edit = new QLineEdit(this);
        auto *send_button = new QPushButton("send", this);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(chat_log);
        layout->addWidget(message_edit);
        layout->addWidget(send_button);


        client_.onMessage = [chat_log](const std::string& message){
          auto mes = message;
          std::erase_if( mes,
                        [](auto ch)
                        {
                          return (ch == '\n' ||
                                  ch == '\r');
                        });
            std::cout << mes << "\n";
            chat_log->append(QString::fromStdString(mes));
        };

        connect(send_button, &QPushButton::clicked, this, [this, message_edit](){
            client_.postMessage(message_edit->text().toStdString() + "\n");
            message_edit->clear();
        });


    }

 private:



    void closeEvent(QCloseEvent *event) override {

        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Messenger",
                                                                 tr("Are you sure?\n"),
                                                                 QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                 QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            client_.stop();
            t.join();
            event->accept();
        }
    }
    TCPClient client_;

};

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);
    MainWindow window;
    window.show();

//    TCPClient client {"localhost", 12345};
//
//    client.onMessage = [](const std::string& message) {
//        std::cout << message << "\n";
//    };
//
//    std::thread t { [&client] () { client.run(); } };
//
//    while (true) {
//        std::string message;
//        getline(std::cin, message);
//        if (message == "\\q") break;
//        message += "\n";
//
//        client.postMessage(message);
//    }
//
//    client.stop();
//    t.join();

//    window.t.join();
    return app.exec();
}
