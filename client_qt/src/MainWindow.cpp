#include "MainWindow.hpp"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QThread>

#include "Client.hpp" // Inclure la définition de notre client

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. Créer les widgets
    setWindowTitle("TCP Tchat");
    chatArea = new QTextEdit(this);
    chatArea->setReadOnly(true);

    messageInput = new QLineEdit(this);
    sendButton = new QPushButton("Envoyer", this);

    // 2. Mettre en page les widgets
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(chatArea);
    layout->addWidget(messageInput);
    layout->addWidget(sendButton);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // 3. Créer et préparer le client
    m_client = new Client();
    clientThread = new QThread(this); // On fait tourner le client dans son propre thread
    m_client->moveToThread(clientThread);

    // 4. Connecter les signaux et les slots (la magie)
    //    De l'UI vers la Logique
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(this, &MainWindow::sendMessageRequested, m_client, &Client::sendMsg);
    connect(this, &MainWindow::connectToServerRequested, m_client, &Client::connectToServer);
    
    //    De la Logique vers l'UI
    connect(m_client, &Client::newMessageReceived, this, &MainWindow::onNewMessageReceived);
    connect(m_client, &Client::connectionStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    
    // Démarrer le thread du client
    clientThread->start();

    // Lancer la connexion initiale
    emit connectToServerRequested("127.0.0.1", 8080);
}

void MainWindow::onSendButtonClicked() {
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        emit sendMessageRequested(message.toStdString());
        messageInput->clear();
    }
}

void MainWindow::onNewMessageReceived(const QString& message) {
    chatArea->append(message);
}

void MainWindow::onConnectionStatusChanged(bool isConnected, const QString& message) {
    chatArea->append(QString("--- %1 ---").arg(message));
    messageInput->setEnabled(isConnected);
    sendButton->setEnabled(isConnected);
}

MainWindow::~MainWindow() {
    QMetaObject::invokeMethod(m_client, "disconnectFromServer", Qt::QueuedConnection);

    clientThread->quit();

    if (!clientThread->wait(3000)) {
        qWarning("Thread did not quit in time, terminating.");
        clientThread->terminate();
        clientThread->wait();
    }
}