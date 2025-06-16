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

/**
 * @class MainWindow
 * @brief La fenêtre principale de l'application de tchat.
 *
 * Gère l'ensemble de l'interface utilisateur (UI), affiche les messages,
 * la liste des utilisateurs et les statuts de connexion.
 * Elle communique avec l'objet Client via des signaux et des slots pour
 * envoyer des messages et recevoir des mises à jour du serveur.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Construit une nouvelle fenêtre principale.
     * @param parent Le widget parent, nullptr par défaut.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructeur. Assure un nettoyage propre du thread client.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Slot déclenché lorsque l'utilisateur clique sur le bouton "Envoyer"
     * ou appuie sur Entrée dans le champ de saisie.
     */
    void onSendButtonClicked();

    /**
     * @brief Slot déclenché lorsque le statut de la connexion au serveur change.
     * @param isConnected Vrai si la connexion est établie, faux sinon.
     * @param message Un message décrivant le nouvel état (ex: "Connecté", "Erreur...").
     */
    void onConnectionStatusChanged(bool isConnected, const QString& message);

    /**
     * @brief Slot déclenché à la réception d'un nouveau message du serveur.
     * @param msg Le message parsé contenant l'auteur, le contenu, etc.
     */
    void onNewMessageReceived(const ParsedMessage& msg);

    /**
     * @brief Slot déclenché lorsque la liste des utilisateurs connectés est mise à jour.
     * @param users La nouvelle liste des pseudonymes.
     */
    void onUserListUpdated(const QStringList& users);

signals:
    /**
     * @brief Signal émis pour demander l'envoi d'un message au serveur.
     * @param message Le contenu du message à envoyer.
     */
    void sendMessageRequested(const std::string& message);

    /**
     * @brief Signal émis pour demander la connexion au serveur.
     * @param ip L'adresse IP du serveur.
     * @param port Le port du serveur.
     */
    void connectToServerRequested(const std::string& ip, int port);

private:
    /**
     * @brief Initialise et configure tous les widgets de l'interface utilisateur.
     */
    void setupUI();

    /**
     * @brief Configure toutes les connexions signal-slot de la classe.
     */
    void setupConnections();

    /**
     * @brief Crée et ajoute un widget de message à la vue du tchat.
     * @param msg Le message parsé à afficher.
     */
    void addMessage(const ParsedMessage& msg);

    /**
     * @brief Crée le bandeau supérieur (header) de l'application.
     */
    void createHeader();
    
    /**
     * @brief Met à jour les indicateurs visuels du statut de connexion dans le header.
     * @param isConnected Vrai si connecté, faux sinon.
     * @param statusText Le texte à afficher (ex: "Connecté").
     */
    void updateConnectionStatus(bool isConnected, const QString& statusText);

    // --- Widgets de l'interface utilisateur ---
    QListView* m_chatView = nullptr;
    QStandardItemModel* m_chatModel = nullptr;

    QLineEdit* m_messageInput = nullptr;
    QPushButton* m_sendButton = nullptr;

    QListWidget* m_userListWidget = nullptr;
    QLabel* m_userCountLabel = nullptr;

    // --- Composants du header ---
    QWidget* m_headerWidget = nullptr;
    QLabel* m_channelIconLabel = nullptr;
    QLabel* m_serverNameLabel = nullptr;
    QWidget* m_separatorLine = nullptr;
    QLabel* m_channelTopicLabel = nullptr;
    QLabel* m_connectionStatusIndicator = nullptr;
    QLabel* m_connectionStatusLabel = nullptr;

    // --- Logique Client & Thread ---
    Client* m_client = nullptr;
    QThread* m_clientThread = nullptr;

    // --- Données et état ---
    QIcon m_onlineIcon;
    QString m_myNickname;
};