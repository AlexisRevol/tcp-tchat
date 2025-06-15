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
    
    // TODO: Connecter un signal pour mettre à jour la liste des utilisateurs
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