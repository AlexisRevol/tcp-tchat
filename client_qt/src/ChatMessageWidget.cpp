// src/ChatMessageWidget.cpp
#include "ChatMessageWidget.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QPainter>
#include <QFont>

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
    // --- 1. Création de l'Avatar (rond avec initiale) ---
    avatarLabel = new QLabel();
    avatarLabel->setFixedSize(40, 40);

    QPixmap avatarPixmap(40, 40);
    avatarPixmap.fill(Qt::transparent); // Important pour un fond transparent

    QPainter painter(&avatarPixmap);
    painter.setRenderHint(QPainter::Antialiasing); // Pour des bords de cercle lisses

    // Génère une couleur de fond unique basée sur le pseudo
    uint hash = static_cast<uint>(qHash(author));
    QColor avatarColor = QColor::fromHsv(hash % 360, 180, 200);
    painter.setBrush(avatarColor);
    painter.setPen(Qt::NoPen); // Pas de bordure pour le cercle
    painter.drawEllipse(0, 0, 40, 40);

    // Dessine l'initiale du pseudo au centre
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
    
    avatarLabel->setPixmap(avatarPixmap);


    // --- 2. Création de la "bulle" de message ---
    // C'est un widget conteneur qui aura un fond coloré grâce au QSS
    QWidget* bubbleWidget = new QWidget();
    // C'est ici la clé : on donne un nom différent pour pouvoir les styler différemment
    bubbleWidget->setObjectName(isOwnMessage ? "bubbleOwn" : "bubbleOther");


    // --- 3. Création des labels de contenu (pseudo, message, heure) ---
    authorLabel = new QLabel(author);
    authorLabel->setObjectName("authorLabel"); // Pour le style QSS (couleur, etc.)
    authorLabel->setStyleSheet("font-weight: bold;"); // On peut garder un style de base ici

    timestampLabel = new QLabel(timestamp);
    timestampLabel->setObjectName("timestampLabel");

    textLabel = new QLabel(text);
    textLabel->setWordWrap(true); // Le texte va à la ligne automatiquement
    textLabel->setObjectName("textLabel");
    

    // --- 4. Organisation du contenu DANS la bulle ---
    QHBoxLayout* topLayout = new QHBoxLayout(); // Pour [Pseudo] ... [Heure]
    topLayout->addWidget(authorLabel);
    topLayout->addStretch();
    topLayout->addWidget(timestampLabel);

    QVBoxLayout* bubbleLayout = new QVBoxLayout(bubbleWidget); // Layout principal de la bulle
    bubbleLayout->setContentsMargins(12, 8, 12, 8); // Padding intérieur à la bulle
    bubbleLayout->setSpacing(4);
    bubbleLayout->addLayout(topLayout);
    bubbleLayout->addWidget(textLabel);
    

    // --- 5. Organisation générale du widget (Avatar + Bulle) ---
    QHBoxLayout* mainLayout = new QHBoxLayout(this); // Layout de ce ChatMessageWidget
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->setSpacing(10);

    if (isOwnMessage) {
        // Si c'est notre message : [Espace] [Bulle]
        mainLayout->addStretch();
        mainLayout->addWidget(bubbleWidget);
        // On n'affiche pas notre propre avatar, comme dans la plupart des messageries
    } else {
        // Si c'est un message d'un autre : [Avatar] [Bulle] [Espace]
        mainLayout->addWidget(avatarLabel, 0, Qt::AlignTop); // Aligne l'avatar en haut
        mainLayout->addWidget(bubbleWidget);
        mainLayout->addStretch();
    }
}

void ChatMessageWidget::setupSystemUi(const QString& systemMessage, QColor color) {
    textLabel = new QLabel(QString("<i>%1</i>").arg(systemMessage));
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setStyleSheet(QString("color: %1;").arg(color.name()));

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->addWidget(textLabel);
}