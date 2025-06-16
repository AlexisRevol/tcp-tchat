// include/ChatMessageWidget.hpp
#pragma once

#include <QWidget>
#include <QString>

class QLabel; // Déclaration anticipée

class ChatMessageWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChatMessageWidget(const QString& author, const QString& text, const QString& timestamp, QWidget* parent = nullptr);

    // Pour les messages système (connexion/déconnexion)
    explicit ChatMessageWidget(const QString& systemMessage, QColor color, QWidget* parent = nullptr);

private:
    void setupUi(const QString& author, const QString& text, const QString& timestamp);
    void setupSystemUi(const QString& systemMessage, QColor color);

    QLabel* authorLabel = nullptr;
    QLabel* textLabel = nullptr;
    QLabel* timestampLabel = nullptr;
    QLabel* avatarLabel = nullptr;
};