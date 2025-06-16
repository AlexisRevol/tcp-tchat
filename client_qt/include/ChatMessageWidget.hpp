#pragma once

#include <QWidget>
#include <QString>
#include <QColor>

class QLabel; // Déclaration anticipée

/**
 * @class ChatMessageWidget
 * @brief Un widget personnalisé pour afficher un seul message dans la liste de tchat.
 *
 * Ce widget peut afficher deux types de messages :
 * 1. Un message utilisateur normal avec avatar, pseudo, contenu et heure.
 * 2. Un message système (ex: "X a rejoint") qui est centré et coloré.
 */
class ChatMessageWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construit un widget pour un message d'utilisateur.
     * @param author Le pseudonyme de l'auteur.
     * @param text Le contenu du message.
     * @param timestamp L'heure d'envoi du message.
     * @param isOwnMessage Vrai si le message a été envoyé par l'utilisateur local.
     * @param parent Le widget parent.
     */
    explicit ChatMessageWidget(const QString& author, const QString& text, const QString& timestamp, bool isOwnMessage = false, QWidget* parent = nullptr);

    /**
     * @brief Construit un widget pour un message système (connexion, déconnexion, etc.).
     * @param systemMessage Le texte à afficher.
     * @param color La couleur du texte du message.
     * @param parent Le widget parent.
     */
    explicit ChatMessageWidget(const QString& systemMessage, QColor color, QWidget* parent = nullptr);

private:
    /**
     * @brief Configure l'UI pour un message utilisateur standard.
     */
    void setupUi(const QString& author, const QString& text, const QString& timestamp, bool isOwnMessage);
    
    /**
     * @brief Configure l'UI pour un message système.
     */
    void setupSystemUi(const QString& systemMessage, QColor color);

    QLabel* m_authorLabel = nullptr;
    QLabel* m_textLabel = nullptr;
    QLabel* m_timestampLabel = nullptr;
    QLabel* m_avatarLabel = nullptr;
};