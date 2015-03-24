/** @file mainwindow.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 3/21/2015</p>
*/

#include <QLayout>
#include <QPlainTextEdit>
#include <QTimer>
#include <QFont>
#include <QCheckBox>
#include <QLabel>

#include "mainwindow.h"
#include "brainf.h"

struct MainWindow::Private
{
    Private(MainWindow * win)
        : win           (win)
        , dataSize      (8)
        , signedData    (false)
    {

    }

    void createWidgets();
    void run();

    MainWindow * win;
    QTimer * timer;
    QPlainTextEdit * editCode, * editInp, * editOut;

    int dataSize;
    bool signedData;
};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow   (parent)
    , p_            (new Private(this))
{
    setMinimumSize(640, 640);
    p_->createWidgets();
}

MainWindow::~MainWindow()
{

}


void MainWindow::Private::createWidgets()
{
    timer = new QTimer(win);
    timer->setSingleShot(true);
    timer->setInterval(200);
    connect(timer, &QTimer::timeout, [=](){ run(); });

    auto w = new QWidget(win);
    win->setCentralWidget(w);

    auto lv = new QVBoxLayout(w);

        lv->addWidget(new QLabel(tr("code"), w) );

        editCode = new QPlainTextEdit(w);
        connect(editCode, &QPlainTextEdit::textChanged, [=](){ timer->start(); });
        lv->addWidget(editCode);
        // setup font
        // XXX monospace does not work for Mac
        QFont f("Monospace");
        f.setStyleHint(QFont::Monospace);
        f.setPixelSize(28);
        editCode->setFont(f);
        editCode->setTabStopWidth(QFontMetrics(editCode->font()).width("    "));

        lv->addWidget(new QLabel(tr("input"), w) );

        editInp = new QPlainTextEdit(w);
        connect(editInp, &QPlainTextEdit::textChanged, [=](){ timer->start(); });
        lv->addWidget(editInp);
        // setup font
        editInp->setFont(f);
        editInp->setTabStopWidth(QFontMetrics(editInp->font()).width("    "));

        auto lh = new QHBoxLayout;
        lv->addLayout(lh);

            auto cb = new QCheckBox(tr("8/16 bit"), w);
            cb->setChecked(dataSize == 16);
            connect(cb, &QCheckBox::stateChanged, [=]()
            {
                dataSize = cb->isChecked() ? 16 : 8;
                run();
            });
            lh->addWidget(cb);

            cb = new QCheckBox(tr("signed"), w);
            cb->setChecked(signedData);
            connect(cb, &QCheckBox::stateChanged, [=]()
            {
                signedData = cb->isChecked();
                run();
            });
            lh->addWidget(cb);

        lv->addWidget(new QLabel(tr("output"), w) );

        editOut = new QPlainTextEdit(w);
        editOut->setReadOnly(true);
        lv->addWidget(editOut);
}

void MainWindow::Private::run()
{
    const int numSteps = 1000000;
    std::string
            code = editCode->toPlainText().toStdString(),
            inp = editInp->toPlainText().toStdString(),
            outp;

    if (dataSize == 8)
    {
        if (signedData)
        {
            Brainf_int8 bf;
            bf.setCode(code);
            bf.setInput(inp);
            bf.run(numSteps);
            outp = bf.outputString();
        }
        else
        {
            Brainf_uint8 bf;
            bf.setCode(code);
            bf.setInput(inp);
            bf.run(numSteps);
            outp = bf.outputString();
        }
    }
    else
    if (dataSize == 16)
    {
        if (signedData)
        {
            Brainf_int16 bf;
            bf.setCode(code);
            bf.setInput(inp);
            bf.run(numSteps);
            outp = bf.outputString();
        }
        else
        {
            Brainf_uint16 bf;
            bf.setCode(code);
            bf.setInput(inp);
            bf.run(numSteps);
            outp = bf.outputString();
        }
    }

    editOut->setPlainText(QString::fromStdString(outp));
}
