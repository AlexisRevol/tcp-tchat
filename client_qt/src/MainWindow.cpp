// src/MainWindow.cpp
#include "MainWindow.hpp"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QListWidget>
#include <QSplitter>
#include <QStandardItemModel> // NOUVEAU
#include <QListView>           // NOUVEAU
#include <QLabel>              // NOUVEAU
#include <QDateTime>           // NOUVEAU

#include "Client.hpp"
#include "ChatMessageWidget.hpp" // NOUVEAU
#include <QThread>
#include <QScrollBar>          // NOUVEAU

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    clientThread = new QThread(this);
    m_client = new Client();

    m_client->moveToThread(clientThread);

    setupUI();
    setupConnections();

    clientThread->start();
    emit connectToServerRequested("127.0.0.1", 8080);
}

void MainWindow::setupUI() {
    setWindowTitle("Modern Tchat");
    setObjectName("mainWindow"); // Pour le QSS

    // --- Création des widgets ---
    chatView = new QListView(this);
    chatView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Pas d'édition
    chatView->setSelectionMode(QAbstractItemView::NoSelection);   // Pas de sélection
    chatView->setSpacing(5); // Espace entre les messages
    chatView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel); // Scrolling fluide
    chatView->setObjectName("chatView");
    
    chatModel = new QStandardItemModel(this);
    chatView->setModel(chatModel);

    // --- Zone de la liste des utilisateurs ---
    userListWidget = new QListWidget(this);
    userListWidget->setObjectName("userListWidget");
    userCountLabel = new QLabel("En ligne - 0", this);
    userCountLabel->setObjectName("userCountLabel");
    userCountLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout* userListLayout = new QVBoxLayout();
    userListLayout->addWidget(userCountLabel);
    userListLayout->addWidget(userListWidget);
    QWidget* userListContainer = new QWidget();
    userListContainer->setLayout(userListLayout);
    userListContainer->setMinimumWidth(150); // Largeur minimale

    messageInput = new QLineEdit(this);
    messageInput->setPlaceholderText("Écrivez votre message ici...");
    messageInput->setObjectName("messageInput");

    sendButton = new QPushButton("Envoyer", this);
    sendButton->setObjectName("sendButton");
    // On pourrait ajouter une icone ici plus tard

    // --- Mise en page ---
    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    QVBoxLayout* chatLayout = new QVBoxLayout();
    chatLayout->addWidget(chatView);
    chatLayout->addLayout(inputLayout);
    QWidget* chatWidget = new QWidget();
    chatWidget->setLayout(chatLayout);
    chatWidget->setObjectName("chatWidget");

    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(userListContainer); // Le container avec le label
    mainSplitter->addWidget(chatWidget);
    mainSplitter->setStretchFactor(0, 1); // Zone user 1 part
    mainSplitter->setStretchFactor(1, 4); // Zone chat 4 parts
    mainSplitter->handle(1)->setFixedWidth(1); // Poignée de splitter fine

    setCentralWidget(mainSplitter);
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
                QString message = QString::fromStdString(msg.params[1]); // Pas besoin de .toHtmlEscaped() ici
                QString timestamp = QDateTime::currentDateTime().toString("HH:mm");
                messageWidget = new ChatMessageWidget(pseudo, message, timestamp);
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
    // Au lieu d'afficher dans la zone de chat principale, on pourrait avoir
    // une barre de statut en bas. Pour l'instant, on le met comme message système.
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
    userListWidget->addItems(users);
    
    // Mettre à jour le compteur !
    userCountLabel->setText(QString("En ligne - %1").arg(users.count()));
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