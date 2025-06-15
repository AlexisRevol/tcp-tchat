#include "MainWindow.hpp"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout> // Pour les layouts horizontaux
#include <QWidget>
#include <QListWidget> // Pour la liste des utilisateurs
#include <QSplitter> // Pour redimensionner les zones

#include "Client.hpp" 
#include <QThread>

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
    setWindowTitle("TCP Tchat");
    
    // --- Création des widgets ---
    chatArea = new QTextEdit(this);
    chatArea->setReadOnly(true);

    userListWidget = new QListWidget(this);

    messageInput = new QLineEdit(this);
    messageInput->setPlaceholderText("Ecrivez votre message ici...");

    sendButton = new QPushButton("Envoyer", this);

    // --- Mise en page ---
    // Zone de saisie et bouton
    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    // Zone de tchat principale
    QVBoxLayout* chatLayout = new QVBoxLayout();
    chatLayout->addWidget(chatArea);
    chatLayout->addLayout(inputLayout);
    QWidget* chatWidget = new QWidget();
    chatWidget->setLayout(chatLayout);

    // On utilise un QSplitter pour pouvoir redimensionner les zones
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(userListWidget);
    mainSplitter->addWidget(chatWidget);
    mainSplitter->setStretchFactor(1, 3); // La zone de tchat est 3x plus grande

    setCentralWidget(mainSplitter);
}

void MainWindow::setupConnections() {
    // ... (les connexions de signaux/slots que vous aviez déjà) ...
    // De l'UI vers la Logique
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendButtonClicked); // UX: Envoyer avec la touche Entrée

    connect(this, &MainWindow::sendMessageRequested, m_client, &Client::sendMsg);
    connect(this, &MainWindow::connectToServerRequested, m_client, &Client::connectToServer);
    
    // De la Logique vers l'UI
    connect(m_client, &Client::newMessageReceived, this, &MainWindow::onNewMessageReceived);
    connect(m_client, &Client::connectionStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(m_client, &Client::userListUpdated, this, &MainWindow::onUserListUpdated);
}

void MainWindow::onSendButtonClicked() {
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        emit sendMessageRequested(message.toStdString());
        messageInput->clear();
    }
}

// Cette fonction doit maintenant recevoir le ParsedMessage complet pour être plus intelligente.
// Modifions le signal/slot pour passer le ParsedMessage.
// 1. Client.hpp:   signal newMessageReceived(const ParsedMessage& msg);
// 2. Client.cpp:   emit newMessageReceived(parsed_msg);
// 3. MainWindow.hpp: slot onNewMessageReceived(const ParsedMessage& msg);
// 4. MainWindow.cpp: connect(m_client, &Client::newMessageReceived, this, &MainWindow::onNewMessageReceived);

// Nouvelle implémentation du slot dans MainWindow.cpp
void MainWindow::onNewMessageReceived(const ParsedMessage& msg) {
    QString html;
    
    // Couleurs définies pour notre thème
    const QString joinColor = "#2ecc71"; // Vert
    const QString partColor = "#e74c3c"; // Rouge
    const QString chatColor = "#ecf0f1"; // Blanc cassé
    const QString pseudoColor = "#3498db"; // Bleu

    switch(msg.command) {
        case Command::JOIN:
            if (!msg.params.empty()) {
                html = QString("<p style='color:%1;'><i>--- %2 a rejoint le tchat. ---</i></p>")
                           .arg(joinColor, QString::fromStdString(msg.params[0]));
            }
            break;
        case Command::PART:
             if (!msg.params.empty()) {
                html = QString("<p style='color:%1;'><i>--- %2 a quitte le tchat. ---</i></p>")
                           .arg(partColor, QString::fromStdString(msg.params[0]));
            }
            break;
        case Command::MSG:
            if (msg.params.size() >= 2) {
                QString pseudo = QString::fromStdString(msg.params[0]);
                QString message = QString::fromStdString(msg.params[1]).toHtmlEscaped(); // Échapper le HTML du message utilisateur
                html = QString("<p><b style='color:%1;'>%2</b>: <span style='color:%3;'>%4</span></p>")
                           .arg(pseudoColor, pseudo, chatColor, message);
            }
            break;
        default:
            break;
    }

    if (!html.isEmpty()) {
        chatArea->append(html);
    }
}

void MainWindow::onConnectionStatusChanged(bool isConnected, const QString& message) {
    chatArea->append(QString("--- %1 ---").arg(message));
    messageInput->setEnabled(isConnected);
    sendButton->setEnabled(isConnected);
}

void MainWindow::onUserListUpdated(const QStringList& users) {
    userListWidget->clear();
    userListWidget->addItems(users);
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