// include/MainWindow.hpp
#pragma once

#include <QMainWindow>
#include <QStringListModel>
#include "ParsedMessage.hpp"

// Déclarations anticipées
class QListView; // MODIFIÉ
class QLineEdit;
class QPushButton;
class Client;
class QStandardItemModel; // NOUVEAU
class QLabel; // NOUVEAU
class QListWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSendButtonClicked();
    void onConnectionStatusChanged(bool isConnected, const QString& message);
    void onNewMessageReceived(const ParsedMessage& msg);
    void onUserListUpdated(const QStringList& users);

signals:
    void sendMessageRequested(const std::string& message);
    void connectToServerRequested(const std::string& ip, int port);

private:
    void setupUI();
    void setupConnections();
    void addMessage(const ParsedMessage& msg); // NOUVELLE fonction helper

    // --- Widgets de l'interface ---
    QListView* chatView = nullptr; // MODIFIÉ: Remplacé QTextEdit par QListView
    QStandardItemModel* chatModel = nullptr; // NOUVEAU: Le modèle pour notre chat

    QLineEdit* messageInput = nullptr;
    QPushButton* sendButton = nullptr;

    QListWidget* userListWidget = nullptr;
    QLabel* userCountLabel = nullptr; // NOUVEAU: Pour "En ligne - X"

    Client* m_client = nullptr;
    QThread* clientThread = nullptr;
};