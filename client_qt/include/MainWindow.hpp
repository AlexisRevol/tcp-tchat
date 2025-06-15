#pragma once

#include <QMainWindow>
#include <QStringListModel> 
#include <QListWidget>
#include "ParsedMessage.hpp"

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
    void onNewMessageReceived(const ParsedMessage& msg);
    void onUserListUpdated(const QStringList& users);

signals:
    // Signaux que la vue envoie à la logique
    void sendMessageRequested(const std::string& message);
    void connectToServerRequested(const std::string& ip, int port);

private:
    void setupUI(); // Nouvelle fonction pour organiser la création de l'UI
    void setupConnections(); // Nouvelle fonction pour organiser les signaux/slots

    // Les widgets de l'interface
    QTextEdit* chatArea = nullptr;
    QLineEdit* messageInput = nullptr;
    QPushButton* sendButton = nullptr;
    QListWidget* userListWidget = nullptr;
    
    Client* m_client = nullptr;
    QThread* clientThread = nullptr;
};