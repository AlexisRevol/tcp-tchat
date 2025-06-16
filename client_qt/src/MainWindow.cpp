// src/MainWindow.cpp
#include "MainWindow.hpp"
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

#include "Client.hpp"
#include "ChatMessageWidget.hpp" 
#include <QThread>
#include <QScrollBar>  

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    clientThread = new QThread(this);
    m_client = new Client();

    m_client->moveToThread(clientThread);

    // Création de l'icône de statut "en ligne"
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor("#2ecc71")); // Vert "émeraude"
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(3, 3, 10, 10); // Un petit cercle au centre du pixmap
    m_onlineIcon = QIcon(pixmap);

    setupUI();

    updateConnectionStatus(false, "Connexion en cours...");
    setupConnections();

    clientThread->start();
    emit connectToServerRequested("127.0.0.1", 8080);
}

void MainWindow::setupUI() {
    setWindowTitle("Modern Tchat");
    setObjectName("mainWindow"); // Pour le QSS

    // --- 1. On crée le header ---
    createHeader();

    // --- 2. On crée le reste de l'interface comme avant ---
    // (chatView, chatModel, userListWidget, etc.)
    chatView = new QListView(this);
    chatView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    chatView->setSelectionMode(QAbstractItemView::NoSelection);
    chatView->setSpacing(5);
    chatView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    chatView->setObjectName("chatView");
    
    chatModel = new QStandardItemModel(this);
    chatView->setModel(chatModel);

    // Zone des utilisateurs
    userListWidget = new QListWidget(this);
    userListWidget->setObjectName("userListWidget");
    userCountLabel = new QLabel("En ligne - 0", this);
    userCountLabel->setObjectName("userCountLabel");
    userCountLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout* userListLayout = new QVBoxLayout();
    userListLayout->addWidget(userCountLabel);
    userListLayout->addWidget(userListWidget);
    userListLayout->setSpacing(0); // Coller le label et la liste
    userListLayout->setContentsMargins(0,0,0,0);
    QWidget* userListContainer = new QWidget();
    userListContainer->setLayout(userListLayout);
    userListContainer->setMinimumWidth(150);

    // Zone de saisie
    messageInput = new QLineEdit(this);
    messageInput->setPlaceholderText("Écrivez votre message ici...");
    messageInput->setObjectName("messageInput");

    sendButton = new QPushButton("Envoyer", this);
    sendButton->setObjectName("sendButton");

    // Layout de la zone de saisie
    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    // Layout de la zone de chat (vue + saisie)
    QVBoxLayout* chatLayout = new QVBoxLayout();
    chatLayout->addWidget(chatView);
    chatLayout->addLayout(inputLayout);
    chatLayout->setContentsMargins(0,0,0,0);
    chatLayout->setSpacing(5); // Espace entre la liste des messages et la barre de saisie
    QWidget* chatWidget = new QWidget();
    chatWidget->setLayout(chatLayout);
    chatWidget->setObjectName("chatWidget");

    // Le Splitter qui sépare les utilisateurs du chat
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(userListContainer);
    mainSplitter->addWidget(chatWidget);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 4);
    mainSplitter->handle(1)->setFixedWidth(2); // Un peu plus visible

    // --- 3. On crée le layout final qui combine Header et Splitter ---
    QWidget* mainContentWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(mainContentWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0); // Pas d'espace entre le header et le contenu

    // On ajoute d'abord le header...
    mainLayout->addWidget(headerWidget);
    // ...puis le splitter en dessous.
    mainLayout->addWidget(mainSplitter);

    // --- 4. Ce nouveau conteneur devient le widget central ---
    setCentralWidget(mainContentWidget);
}

void MainWindow::createHeader() {
    headerWidget = new QWidget();
    headerWidget->setObjectName("headerWidget");
    headerWidget->setFixedHeight(50); // Hauteur fixe pour le bandeau

    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(15, 0, 15, 0);
    headerLayout->setSpacing(10); // Espace entre les éléments du header

    // 1. Icône de salon '#'
    channelIconLabel = new QLabel("#");
    channelIconLabel->setObjectName("channelIconLabel");

    // 2. Nom du salon
    serverNameLabel = new QLabel("Tchat Principal");
    serverNameLabel->setObjectName("serverNameLabel");

    // 3. Ligne de séparation verticale
    separatorLine = new QWidget();
    separatorLine->setFixedWidth(1);
    separatorLine->setObjectName("separatorLine");

    // 4. Sujet du salon (modifiable plus tard)
    channelTopicLabel = new QLabel("Discussions générales sur la programmation C++ & Qt");
    channelTopicLabel->setObjectName("channelTopicLabel");

    // 5. Indicateur de statut (le point de couleur)
    connectionStatusIndicator = new QLabel();
    connectionStatusIndicator->setObjectName("connectionStatusIndicator");
    connectionStatusIndicator->setFixedSize(12, 12); // Un petit cercle

    // 6. Texte du statut
    connectionStatusLabel = new QLabel("Connexion en cours...");
    connectionStatusLabel->setObjectName("connectionStatusLabel");


    // Ajout des widgets au layout
    headerLayout->addWidget(channelIconLabel);
    headerLayout->addWidget(serverNameLabel);
    headerLayout->addWidget(separatorLine);
    headerLayout->addWidget(channelTopicLabel);
    headerLayout->addStretch(); // Pousse le statut de connexion tout à droite
    headerLayout->addWidget(connectionStatusIndicator);
    headerLayout->addWidget(connectionStatusLabel);
}


void MainWindow::setupConnections() {
    // ... (les connexions existantes restent les mêmes) ...
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendButtonClicked);

    connect(this, &MainWindow::sendMessageRequested, m_client, &Client::sendMsg);
    connect(this, &MainWindow::connectToServerRequested, m_client, &Client::connectToServer);
    
    connect(m_client, &Client::newMessageReceived, this, &MainWindow::onNewMessageReceived);
    connect(m_client, &Client::connectionStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(m_client, &Client::userListUpdated, this, &MainWindow::onUserListUpdated);
}

void MainWindow::onSendButtonClicked() {
    QString message = messageInput->text().trimmed(); // .trimmed() pour enlever les espaces
    if (!message.isEmpty()) {
        emit sendMessageRequested(message.toStdString());
        messageInput->clear();
    }
}

// Slot qui reçoit le message du client et l'ajoute à la vue
void MainWindow::onNewMessageReceived(const ParsedMessage& msg) {
    addMessage(msg);
}

// Nouvelle fonction pour ajouter un message à la vue
void MainWindow::addMessage(const ParsedMessage& msg) {
    QWidget* messageWidget = nullptr;
    
    // Couleurs définies pour le thème
    const QColor joinColor = QColor("#2ecc71"); // Vert
    const QColor partColor = QColor("#e74c3c"); // Rouge
    const QColor statusColor = QColor("#95a5a6"); // Gris pour le status

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
                bool isOwn = (pseudo == m_myNickname);
                
                messageWidget = new ChatMessageWidget(pseudo, message, timestamp, isOwn);
            }
            break;
        default:
            return; // Ne rien faire si on ne reconnaît pas la commande
    }

    if (messageWidget) {
        QStandardItem* item = new QStandardItem();
        // C'est ici que la magie opère : on définit la taille de l'item
        // et Qt utilisera notre widget pour le dessiner !
        item->setSizeHint(messageWidget->sizeHint()); 
        
        chatModel->appendRow(item);
        
        // On attache notre widget personnalisé à l'index de l'item
        QModelIndex index = chatModel->indexFromItem(item);
        chatView->setIndexWidget(index, messageWidget);

        // Scroll automatique vers le bas
        chatView->scrollToBottom();
    }
}


void MainWindow::onConnectionStatusChanged(bool isConnected, const QString& message) {
    // La mise à jour du header est maintenant gérée par notre nouvelle fonction
    if (isConnected) {
        if (m_myNickname.isEmpty()) m_myNickname = "Vous"; // Logique pour le pseudo
        updateConnectionStatus(true, "Connecté");
    } else {
        // 'message' contient la raison (ex: "Déconnecté du serveur", "Erreur de connexion")
        updateConnectionStatus(false, message);
    }

    // Le reste de la logique reste le même...
    ChatMessageWidget* statusWidget = new ChatMessageWidget(message, QColor("#95a5a6"));
    
    QStandardItem* item = new QStandardItem();
    item->setSizeHint(statusWidget->sizeHint());
    chatModel->appendRow(item);
    QModelIndex index = chatModel->indexFromItem(item);
    chatView->setIndexWidget(index, statusWidget);
    
    messageInput->setEnabled(isConnected);
    sendButton->setEnabled(isConnected);
}

void MainWindow::onUserListUpdated(const QStringList& users) {
    userListWidget->clear();
    for (const QString& user : users) {
        QListWidgetItem* item = new QListWidgetItem(m_onlineIcon, user);
        userListWidget->addItem(item);
    }
    
    userCountLabel->setText(QString("EN LIGNE — %1").arg(users.count()));
}

void MainWindow::updateConnectionStatus(bool isConnected, const QString& statusText) {
    connectionStatusLabel->setText(statusText);

    QString styleSheet;
    if (isConnected) {
        // Style pour "Connecté" (vert)
        styleSheet = "background-color: #2ecc71; border-radius: 6px;"; // Vert + cercle
        connectionStatusLabel->setStyleSheet("color: #b9bbbe;"); // Couleur de texte normale
    } else if (statusText.contains("Déconnecté")) {
        // Style pour "Déconnecté" (rouge)
        styleSheet = "background-color: #e74c3c; border-radius: 6px;"; // Rouge + cercle
        connectionStatusLabel->setStyleSheet("color: #e74c3c;"); // Texte en rouge aussi
    } else {
        // Style pour "Connexion en cours..." ou "Erreur" (orange/jaune)
        styleSheet = "background-color: #f1c40f; border-radius: 6px;"; // Jaune/Orange + cercle
        connectionStatusLabel->setStyleSheet("color: #f1c40f;");
    }
    
    connectionStatusIndicator->setStyleSheet(styleSheet);
    connectionStatusIndicator->setToolTip(statusText); // Infobulle au survol du point
}

MainWindow::~MainWindow() {
    if(clientThread && clientThread->isRunning()) {
        QMetaObject::invokeMethod(m_client, "disconnectFromServer", Qt::QueuedConnection);

        clientThread->quit();

        if (!clientThread->wait(3000)) {
            qWarning("Thread did not quit in time, terminating.");
            clientThread->terminate();
            clientThread->wait();
        }
    }
}