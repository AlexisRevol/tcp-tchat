// src/ChatMessageWidget.cpp
#include "ChatMessageWidget.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>

// Constructeur pour un message d'utilisateur normal
ChatMessageWidget::ChatMessageWidget(const QString& author, const QString& text, const QString& timestamp, QWidget* parent)
    : QWidget(parent) {
    setupUi(author, text, timestamp);
}

// Constructeur pour un message système (JOIN/PART)
ChatMessageWidget::ChatMessageWidget(const QString& systemMessage, QColor color, QWidget* parent)
    : QWidget(parent) {
    setupSystemUi(systemMessage, color);
}


void ChatMessageWidget::setupUi(const QString& author, const QString& text, const QString& timestamp) {
    // --- Avatar ---
    avatarLabel = new QLabel();
    // Pour l'instant, un simple carré de couleur. On pourrait charger une image.
    QPixmap avatar(40, 40);
    // On génère une couleur simple basée sur le hash du pseudo
    uint hash = static_cast<uint>(qHash(author));
    avatar.fill(QColor::fromHsv(hash % 360, 200, 230));
    avatarLabel->setPixmap(avatar);
    avatarLabel->setFixedSize(40, 40);
    avatarLabel->setStyleSheet("border-radius: 20px;"); // Pour un avatar rond

    // --- Contenu du message ---
    authorLabel = new QLabel(QString("<b>%1</b>").arg(author));
    authorLabel->setStyleSheet("color: #3498db;"); // Bleu pour le pseudo

    timestampLabel = new QLabel(timestamp);
    timestampLabel->setStyleSheet("color: #95a5a6; font-size: 9pt;");

    textLabel = new QLabel(text);
    textLabel->setWordWrap(true);
    textLabel->setStyleSheet("color: #ecf0f1;");

    // --- Layout ---
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(authorLabel);
    topLayout->addStretch();
    topLayout->addWidget(timestampLabel);

    QVBoxLayout* messageContentLayout = new QVBoxLayout();
    messageContentLayout->addLayout(topLayout);
    messageContentLayout->addWidget(textLabel);
    messageContentLayout->setSpacing(2);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5); // Un peu d'air
    mainLayout->addWidget(avatarLabel);
    mainLayout->addLayout(messageContentLayout);
    mainLayout->setStretch(1, 1); // Le contenu du message prend toute la place
}

void ChatMessageWidget::setupSystemUi(const QString& systemMessage, QColor color) {
    textLabel = new QLabel(QString("<i>%1</i>").arg(systemMessage));
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setStyleSheet(QString("color: %1;").arg(color.name()));

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->addWidget(textLabel);
}