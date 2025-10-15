#ifndef PRINTVIEW_H
#define PRINTVIEW_H

#include <QWebView>
#include <QPrinter>
#include <QScopedPointer>
#include <QPrintDialog>

class PrintView : public QObject
{
    Q_OBJECT
    QWebView* m_webview;
    QScopedPointer<QPrinter> m_printer;

public:
    explicit PrintView(QObject *parent = 0);
    ~PrintView();

signals:

public slots:
    void showPrintDialog() {
        if (!m_printer) m_printer.reset(new QPrinter);
        QScopedPointer<QPrintDialog> dialog(new QPrintDialog(m_printer.data(), nullptr));
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog.data(), SIGNAL(accepted(QPrinter*)), SLOT(print(QPrinter*)));
        dialog->show();
        dialog.take(); // The dialog will self-delete
    }
    void print(QPrinter* printer) {
        m_webview->print(printer);
    }
};

#endif // PRINTVIEW_H
