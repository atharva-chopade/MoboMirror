#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();
    
    void clearScreen();
    void displayMessage(const QString &message);

private:
    void setupUI();
    
    QLabel *messageLabel;
    QVBoxLayout *layout;
};

#endif // VIDEOWIDGET_H
