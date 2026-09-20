#ifndef BROWSERCHOOSER_H
#define BROWSERCHOOSER_H

#include <QWidget>
#include <QVector>
#include "BrowserDetector.h"

class BrowserChooser : public QWidget {
    Q_OBJECT

public:
    explicit BrowserChooser(const QVector<Browser>& browsers, const QString& url, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    int getItemIndexAtY(int y) const;

    QVector<Browser> m_browsers;
    QString m_url;
    int m_hoveredIndex;
    QPoint m_dragStartPos;
    bool m_dragging;

    static constexpr int WINDOW_WIDTH = 320;
    static constexpr int BTN_HEIGHT = 36;
    static constexpr int TITLE_HEIGHT = 40;
    static constexpr int PADDING = 10;
};

#endif
