// src/ChatMessageWidget.cpp
#include "ChatMessageWidget.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>

// Constructeur pour un message d'utilisateur normal
ChatMessageWidget::ChatMessageWidget(const QString& author, const QString& text, const QString& timestamp, bool isOwnMessage, QWidget* parent)
    : QWidget(parent) {
    setupUi(author, text, timestamp, isOwnMessage);
}

// Constructeur pour un message système (JOIN/PART)
ChatMessageWidget::ChatMessageWidget(const QString& systemMessage, QColor color, QWidget* parent)
    : QWidget(parent) {
    setupSystemUi(systemMessage, color);
}


void ChatMessageWidget::setupUi(const QString& author, const QString& text, const QString& timestamp, bool isOwnMessage) {
    // 1. Création de l'Avatar (rond avec initiale)
    m_avatarLabel = new QLabel();
    m_avatarLabel->setFixedSize(40, 40);

    QPixmap avatarPixmap(40, 40);
    avatarPixmap.fill(QColor("#36393f"));

    QPainter painter(&avatarPixmap);
    painter.setRenderHint(QPainter::Antialiasing); // Pour des bords de cercle lisses

    // Génère une couleur de fond unique basée sur le hachage du pseudo
    uint hash = static_cast<uint>(qHash(author));
    QColor avatarColor = QColor::fromHsv(hash % 360, 180, 200);
    painter.setBrush(avatarColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 40, 40);

    // Dessine l'initiale du pseudo au centre de l'avatar
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPixelSize(20);
    font.setBold(true);
    painter.setFont(font);
    QString initial;
    if (author.isEmpty()) {
        initial = "?";
    } else {
        initial = author.at(0).toUpper();
    }
    painter.drawText(avatarPixmap.rect(), Qt::AlignCenter, initial);
    
    m_avatarLabel->setPixmap(avatarPixmap);


    // 2. Création de la "bulle" de message
    QWidget* bubbleWidget = new QWidget();
    // L'identifiant d'objet permet un stylage QSS différent pour ses propres messages et ceux des autres.
    bubbleWidget->setObjectName(isOwnMessage ? "bubbleOwn" : "bubbleOther");

    // 3. Création des labels de contenu (pseudo, message, heure)
    m_authorLabel = new QLabel(author);
    m_authorLabel->setObjectName("authorLabel");
    m_authorLabel->setStyleSheet("font-weight: bold; color: " + avatarColor.name() + ";");

    m_timestampLabel = new QLabel(timestamp);
    m_timestampLabel->setObjectName("timestampLabel");

    m_textLabel = new QLabel(text);
    m_textLabel->setWordWrap(true);
    m_textLabel->setObjectName("textLabel");
    
    // 4. Organisation du contenu DANS la bulle
    QHBoxLayout* topLayout = new QHBoxLayout(); // [Pseudo] ... [Heure]
    topLayout->addWidget(m_authorLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_timestampLabel);

    QVBoxLayout* bubbleLayout = new QVBoxLayout(bubbleWidget);
    bubbleLayout->setContentsMargins(12, 8, 12, 8);
    bubbleLayout->setSpacing(4);
    bubbleLayout->addLayout(topLayout);
    bubbleLayout->addWidget(m_textLabel);
    
    // 5. Organisation générale du widget (Avatar + Bulle)
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->setSpacing(10);

    if (isOwnMessage) {
        // [Espace] [Bulle]
        mainLayout->addStretch();
        mainLayout->addWidget(bubbleWidget);
    } else {
        // [Avatar] [Bulle] [Espace]
        mainLayout->addWidget(m_avatarLabel, 0, Qt::AlignTop); // Aligne l'avatar en haut
        mainLayout->addWidget(bubbleWidget);
        mainLayout->addStretch();
    }
}

void ChatMessageWidget::setupSystemUi(const QString& systemMessage, QColor color) {
    m_textLabel = new QLabel(QString("<i>%1</i>").arg(systemMessage));
    m_textLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setStyleSheet(QString("color: %1;").arg(color.name()));

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->addWidget(m_textLabel);
}