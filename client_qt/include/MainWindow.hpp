#pragma once

#include <QMainWindow>

// Déclarations anticipées pour éviter les #include dans le .h
class QTextEdit;
class QLineEdit;
class QPushButton;
class Client;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slots qui réagissent aux actions de l'utilisateur ou du client
    void onSendButtonClicked();
    void onConnectionStatusChanged(bool isConnected, const QString& message);
    void onNewMessageReceived(const QString& message);

signals:
    // Signaux que la vue envoie à la logique
    void sendMessageRequested(const std::string& message);
    void connectToServerRequested(const std::string& ip, int port);

private:
    // Les widgets de notre interface
    QTextEdit* chatArea;
    QLineEdit* messageInput;
    QPushButton* sendButton;
    
    // Un pointeur vers notre logique client
    Client* m_client;
    QThread* clientThread;
};