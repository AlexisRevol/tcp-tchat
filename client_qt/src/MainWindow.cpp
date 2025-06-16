// src/MainWindow.cpp
#include "MainWindow.hpp"
#include "Client.hpp"
#include "ChatMessageWidget.hpp"

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QListWidget>
#include <QSplitter>
#include <QStandardItemModel>
#include <QListView>
#include <QLabel>
#include <QDateTime>
#include <QPainter>
#include <QThread>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Le client réseau doit s'exécuter dans un thread séparé
    // pour ne pas bloquer l'interface utilisateur.
    m_clientThread = new QThread(this);
    m_client = new Client();
    m_client->moveToThread(m_clientThread);

    // Création programmatique de l'icône "en ligne"
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor("#2ecc71")); // Vert
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(3, 3, 10, 10);
    m_onlineIcon = QIcon(pixmap);

    // Configuration de l'UI et des connexions
    setupUI();
    setupConnections();
    
    updateConnectionStatus(false, "Connexion...");

    // Démarrage du thread client et demande de connexion initiale
    m_clientThread->start();
    emit connectToServerRequested("127.0.0.1", 8080);
}

MainWindow::~MainWindow() {
    // Procédure de fermeture propre du thread.
    if(m_clientThread && m_clientThread->isRunning()) {
        QMetaObject::invokeMethod(m_client, "disconnectFromServer", Qt::QueuedConnection);

        m_clientThread->quit(); 

        // Attente de la fin du thread avec un timeout
        if (!m_clientThread->wait(3000)) {
            qWarning("Le thread client n'a pas pu être arrêté proprement. Forçage de la terminaison.");
            m_clientThread->terminate();
            m_clientThread->wait();
        }
    }
}


void MainWindow::setupUI() {
    setWindowTitle("Modern Tchat");
    setObjectName("mainWindow"); // ID pour le stylage QSS

    createHeader();

    // Vue principale pour afficher les messages
    m_chatView = new QListView(this);
    m_chatView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_chatView->setSelectionMode(QAbstractItemView::NoSelection);
    m_chatView->setSpacing(5);
    m_chatView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_chatView->setObjectName("chatView");
    
    m_chatModel = new QStandardItemModel(this);
    m_chatView->setModel(m_chatModel);

    // Panneau de droite pour la liste des utilisateurs
    m_userListWidget = new QListWidget(this);
    m_userListWidget->setObjectName("userListWidget");
    m_userCountLabel = new QLabel("En ligne - 0", this);
    m_userCountLabel->setObjectName("userCountLabel");
    m_userCountLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout* userListLayout = new QVBoxLayout();
    userListLayout->addWidget(m_userCountLabel);
    userListLayout->addWidget(m_userListWidget);
    userListLayout->setSpacing(0);
    userListLayout->setContentsMargins(0,0,0,0);
    QWidget* userListContainer = new QWidget();
    userListContainer->setLayout(userListLayout);
    userListContainer->setMinimumWidth(150);

    // Zone de saisie des messages en bas
    m_messageInput = new QLineEdit(this);
    m_messageInput->setPlaceholderText("Écrivez votre message ici...");
    m_messageInput->setObjectName("messageInput");

    m_sendButton = new QPushButton("Envoyer", this);
    m_sendButton->setObjectName("sendButton");

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(m_sendButton);

    // Conteneur pour la vue du tchat et la zone de saisie
    QVBoxLayout* chatLayout = new QVBoxLayout();
    chatLayout->addWidget(m_chatView);
    chatLayout->addLayout(inputLayout);
    chatLayout->setContentsMargins(0,0,0,0);
    chatLayout->setSpacing(5);
    QWidget* chatWidget = new QWidget();
    chatWidget->setLayout(chatLayout);
    chatWidget->setObjectName("chatWidget");

    // Séparateur principal entre la liste des utilisateurs et le tchat
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(userListContainer);
    mainSplitter->addWidget(chatWidget);
    mainSplitter->setStretchFactor(0, 1); // La liste d'utilisateurs prend moins de place
    mainSplitter->setStretchFactor(1, 4); // La zone de tchat prend plus de place

    // Assemblage final de la fenêtre
    QWidget* mainContentWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(mainContentWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0); // Le header est collé au reste
    mainLayout->addWidget(m_headerWidget);
    mainLayout->addWidget(mainSplitter);

    setCentralWidget(mainContentWidget);
}

void MainWindow::createHeader() {
    m_headerWidget = new QWidget();
    m_headerWidget->setObjectName("headerWidget");
    m_headerWidget->setFixedHeight(50);

    QHBoxLayout* headerLayout = new QHBoxLayout(m_headerWidget);
    headerLayout->setContentsMargins(15, 0, 15, 0);
    headerLayout->setSpacing(10);

    // Icône et nom du salon
    m_channelIconLabel = new QLabel("#");
    m_channelIconLabel->setObjectName("channelIconLabel");
    m_serverNameLabel = new QLabel("Tchat Principal");
    m_serverNameLabel->setObjectName("serverNameLabel");

    // Ligne de séparation et sujet du salon
    m_separatorLine = new QWidget();
    m_separatorLine->setFixedWidth(1);
    m_separatorLine->setObjectName("separatorLine");
    m_channelTopicLabel = new QLabel("Discussions générales");
    m_channelTopicLabel->setObjectName("channelTopicLabel");

    // Indicateurs de statut de connexion (à droite)
    m_connectionStatusIndicator = new QLabel();
    m_connectionStatusIndicator->setObjectName("connectionStatusIndicator");
    m_connectionStatusIndicator->setFixedSize(12, 12);
    m_connectionStatusLabel = new QLabel("Connexion...");
    m_connectionStatusLabel->setObjectName("connectionStatusLabel");

    // Ajout des widgets au layout du header
    headerLayout->addWidget(m_channelIconLabel);
    headerLayout->addWidget(m_serverNameLabel);
    headerLayout->addWidget(m_separatorLine);
    headerLayout->addWidget(m_channelTopicLabel);
    headerLayout->addStretch(); // Pousse les éléments suivants tout à droite
    headerLayout->addWidget(m_connectionStatusIndicator);
    headerLayout->addWidget(m_connectionStatusLabel);
}

void MainWindow::setupConnections() {
    // Actions de l'utilisateur dans l'UI
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendButtonClicked);

    // Communication de la MainWindow vers le Client (dans son thread)
    connect(this, &MainWindow::sendMessageRequested, m_client, &Client::sendMsg);
    connect(this, &MainWindow::connectToServerRequested, m_client, &Client::connectToServer);
    
    // Communication du Client vers la MainWindow (thread-safe)
    connect(m_client, &Client::newMessageReceived, this, &MainWindow::onNewMessageReceived);
    connect(m_client, &Client::connectionStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(m_client, &Client::userListUpdated, this, &MainWindow::onUserListUpdated);
}

void MainWindow::onSendButtonClicked() {
    QString message = m_messageInput->text().trimmed();
    if (!message.isEmpty()) {
        emit sendMessageRequested(message.toStdString());
        m_messageInput->clear();
    }
}

void MainWindow::onNewMessageReceived(const ParsedMessage& msg) {
    addMessage(msg);
}

void MainWindow::addMessage(const ParsedMessage& msg) {
    QWidget* messageWidget = nullptr;
    
    // Couleurs sémantiques pour les messages système
    const QColor joinColor = QColor("#2ecc71");   // Vert
    const QColor partColor = QColor("#e74c3c");   // Rouge
    const QColor statusColor = QColor("#95a5a6"); // Gris

    switch(msg.command) {
        case Command::JOIN:
            if (!msg.params.empty()) {
                QString systemText = QString("%1 a rejoint le tchat.").arg(QString::fromStdString(msg.params[0]));
                messageWidget = new ChatMessageWidget(systemText, joinColor);
            }
            break;
        case Command::PART:
             if (!msg.params.empty()) {
                QString systemText = QString("%1 a quitté le tchat.").arg(QString::fromStdString(msg.params[0]));
                messageWidget = new ChatMessageWidget(systemText, partColor);
            }
            break;
        case Command::MSG:
            if (msg.params.size() >= 2) {
                QString pseudo = QString::fromStdString(msg.params[0]);
                QString message = QString::fromStdString(msg.params[1]);
                QString timestamp = QDateTime::currentDateTime().toString("HH:mm");
                bool isOwn = (pseudo == m_myNickname); // Détermine si le message est de l'utilisateur local
                
                messageWidget = new ChatMessageWidget(pseudo, message, timestamp, isOwn);
            }
            break;
        default:
            qWarning() << "Commande de message non reconnue reçue.";
            return;
    }

    if (messageWidget) {
        QStandardItem* item = new QStandardItem();
        // Le modèle doit connaître la taille du widget pour que la vue l'affiche correctement.
        item->setSizeHint(messageWidget->sizeHint()); 
        
        m_chatModel->appendRow(item);
        
        // On attache notre widget personnalisé à l'index de l'item dans la vue.
        QModelIndex index = m_chatModel->indexFromItem(item);
        m_chatView->setIndexWidget(index, messageWidget);

        // Défilement automatique vers le message le plus récent.
        m_chatView->scrollToBottom();
    }
}


void MainWindow::onConnectionStatusChanged(bool isConnected, const QString& message) {
    if (isConnected) {
        if (m_myNickname.isEmpty()) m_myNickname = "Vous";
        updateConnectionStatus(true, "Connecté");
    } else {
        updateConnectionStatus(false, message);
    }

    // Ajoute un message de statut dans la fenêtre de tchat.
    ChatMessageWidget* statusWidget = new ChatMessageWidget(message, QColor("#95a5a6"));
    QStandardItem* item = new QStandardItem();
    item->setSizeHint(statusWidget->sizeHint());
    m_chatModel->appendRow(item);
    QModelIndex index = m_chatModel->indexFromItem(item);
    m_chatView->setIndexWidget(index, statusWidget);
    
    // Active ou désactive la zone de saisie en fonction de l'état de la connexion.
    m_messageInput->setEnabled(isConnected);
    m_sendButton->setEnabled(isConnected);
}

void MainWindow::onUserListUpdated(const QStringList& users) {
    m_userListWidget->clear();
    for (const QString& user : users) {
        QListWidgetItem* item = new QListWidgetItem(m_onlineIcon, user);
        m_userListWidget->addItem(item);
    }
    
    m_userCountLabel->setText(QString("EN LIGNE — %1").arg(users.count()));
}

void MainWindow::updateConnectionStatus(bool isConnected, const QString& statusText) {
    m_connectionStatusLabel->setText(statusText);

    QString styleSheet;
    if (isConnected) {
        styleSheet = "background-color: #2ecc71; border-radius: 6px;"; // Vert
        m_connectionStatusLabel->setStyleSheet("color: #b9bbbe;");
    } else if (statusText.contains("Déconnecté")) {
        styleSheet = "background-color: #e74c3c; border-radius: 6px;"; // Rouge
        m_connectionStatusLabel->setStyleSheet("color: #e74c3c;");
    } else {
        styleSheet = "background-color: #f1c40f; border-radius: 6px;"; // Jaune/Orange
        m_connectionStatusLabel->setStyleSheet("color: #f1c40f;");
    }
    
    m_connectionStatusIndicator->setStyleSheet(styleSheet);
    m_connectionStatusIndicator->setToolTip(statusText);
}