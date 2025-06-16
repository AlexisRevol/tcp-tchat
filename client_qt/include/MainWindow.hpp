// include/MainWindow.hpp
#pragma once

#include <QMainWindow>
#include <QStringListModel>
#include <QIcon>
#include "ParsedMessage.hpp"

// Déclarations anticipées
class QListView; 
class QLineEdit;
class QPushButton;
class Client;
class QStandardItemModel; 
class QLabel;
class QListWidget;
class QWidget;

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
    void addMessage(const ParsedMessage& msg); 
    void createHeader();
    void updateConnectionStatus(bool isConnected, const QString& statusText);

    // --- Widgets de l'interface ---
    QListView* chatView = nullptr; 
    QStandardItemModel* chatModel = nullptr; 

    QLineEdit* messageInput = nullptr;
    QPushButton* sendButton = nullptr;

    QListWidget* userListWidget = nullptr;
    QLabel* userCountLabel = nullptr; 

    Client* m_client = nullptr;
    QThread* clientThread = nullptr;

    QIcon m_onlineIcon;

    // --- Widgets du Header (mis à jour) ---
    QWidget* headerWidget = nullptr;
    QLabel* channelIconLabel = nullptr; // Pour l'icône '#'
    QLabel* serverNameLabel = nullptr;
    QWidget* separatorLine = nullptr;   // La ligne de séparation
    QLabel* channelTopicLabel = nullptr; // Le sujet du salon
    QLabel* connectionStatusIndicator = nullptr; // Le point de couleur
    QLabel* connectionStatusLabel = nullptr; // Le texte "Connecté" ou "Déconnexion..."

    QString m_myNickname;
};