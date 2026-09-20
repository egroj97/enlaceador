#include "BrowserChooser.h"
#include <QPainter>
#include <QMouseEvent>
#include <QProcess>
#include <QGuiApplication>
#include <QScreen>

BrowserChooser::BrowserChooser(const QVector<Browser>& browsers, const QString& url, QWidget* parent)
    : QWidget(parent), m_browsers(browsers), m_url(url), m_hoveredIndex(-1), m_dragging(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setMouseTracking(true);

    int totalHeight = m_browsers.isEmpty()
        ? TITLE_HEIGHT + 80
        : TITLE_HEIGHT + PADDING + m_browsers.size() * (BTN_HEIGHT + 2) + PADDING + 10;

    setFixedSize(WINDOW_WIDTH, totalHeight);

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - WINDOW_WIDTH) / 2;
    int y = (screenGeometry.height() - totalHeight) / 2;
    move(x, y);
}

void BrowserChooser::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(45, 45, 45));

    QRect titleRect(0, 0, WINDOW_WIDTH, TITLE_HEIGHT);
    painter.fillRect(titleRect, QColor(60, 60, 60));

    painter.setPen(QColor(255, 255, 255));
    QFont titleFont("Segoe UI", 14, QFont::Bold);
    painter.setFont(titleFont);
    QRect textRect(PADDING, 10, WINDOW_WIDTH - 35, TITLE_HEIGHT);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, "Elige un navegador");

    QFont closeFont("Segoe UI", 16, QFont::Bold);
    painter.setFont(closeFont);
    painter.setPen(QColor(200, 200, 200));
    QRect closeRect(WINDOW_WIDTH - 30, 8, 30, TITLE_HEIGHT);
    painter.drawText(closeRect, Qt::AlignCenter | Qt::AlignVCenter | Qt::TextSingleLine, QString::fromUtf8("\u00D7"));

    if (m_browsers.isEmpty()) {
        QFont btnFont("Segoe UI", 12);
        painter.setFont(btnFont);
        painter.setPen(QColor(128, 128, 128));
        QRect noRect(PADDING, TITLE_HEIGHT + 30, WINDOW_WIDTH - PADDING, 30);
        painter.drawText(noRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, "No se encontraron navegadores");
    } else {
        QFont btnFont("Segoe UI", 12);
        painter.setFont(btnFont);

        for (int i = 0; i < m_browsers.size(); ++i) {
            int y = TITLE_HEIGHT + PADDING + i * (BTN_HEIGHT + 2);
            QColor color = (i == m_hoveredIndex) ? QColor(80, 80, 80) : QColor(64, 64, 64);

            QRect btnRect(PADDING, y + 1, WINDOW_WIDTH - PADDING * 2, BTN_HEIGHT - 2);
            painter.fillRect(btnRect, color);

            painter.setPen(QColor(255, 255, 255));
            QRect nameRect(PADDING + 15, y + 1, WINDOW_WIDTH - PADDING * 2 - 10, BTN_HEIGHT - 2);
            painter.drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, m_browsers[i].name);
        }
    }
}

void BrowserChooser::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        move(event->globalPosition().toPoint() - m_dragStartPos);
        return;
    }

    int y = event->position().y();
    int idx = getItemIndexAtY(y);

    if (idx != m_hoveredIndex) {
        m_hoveredIndex = idx;
        setCursor(idx >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void BrowserChooser::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int y = event->position().y();

        if (y <= TITLE_HEIGHT) {
            m_dragging = true;
            m_dragStartPos = event->position().toPoint();
        } else {
            int idx = getItemIndexAtY(y);
            if (idx >= 0) {
                QProcess::startDetached(m_browsers[idx].path, {m_url});
                close();
            }
        }
    }
}

void BrowserChooser::mouseReleaseEvent(QMouseEvent*) {
    m_dragging = false;
}

void BrowserChooser::leaveEvent(QEvent*) {
    if (m_hoveredIndex != -1) {
        m_hoveredIndex = -1;
        update();
    }
}

int BrowserChooser::getItemIndexAtY(int y) const {
    int yStart = TITLE_HEIGHT + PADDING;
    int idx = (y - yStart) / (BTN_HEIGHT + 2);
    return (idx >= 0 && idx < m_browsers.size()) ? idx : -1;
}
