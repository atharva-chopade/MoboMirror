#include "videowidget.h"
#include <QFont>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
    , messageLabel(nullptr)
    , layout(nullptr)
{
    setupUI();
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::setupUI()
{
    layout = new QVBoxLayout(this);
    
    messageLabel = new QLabel(this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);
    
    QFont font = messageLabel->font();
    font.setPointSize(16);
    messageLabel->setFont(font);
    
    QString initialMessage = 
        "<h2>MoboMirror - iPhone Screen Mirroring</h2>"
        "<p>Select a device and click Start to begin mirroring</p>"
        "<p>&nbsp;</p>"
        "<p><b>Features:</b></p>"
        "<ul style='text-align: left; display: inline-block;'>"
        "<li>USB Mirroring - Direct USB connection for high quality</li>"
        "<li>Wireless Mirroring - Mirror over WiFi (requires pairing)</li>"
        "<li>Audio & Video - Full audiovisual streaming</li>"
        "</ul>"
        "<p>&nbsp;</p>"
        "<p style='color: gray;'>The video stream will open in a new GStreamer window when mirroring starts.</p>";
    
    messageLabel->setText(initialMessage);
    
    layout->addWidget(messageLabel);
    setLayout(layout);
    
    // Set dark background
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(30, 30, 30));
    pal.setColor(QPalette::WindowText, Qt::white);
    setPalette(pal);
    messageLabel->setPalette(pal);
}

void VideoWidget::clearScreen()
{
    messageLabel->setText(
        "<h2>Mirroring Stopped</h2>"
        "<p>Select a device and click Start to begin mirroring again</p>"
    );
}

void VideoWidget::displayMessage(const QString &message)
{
    messageLabel->setText(message);
}
