/** @file main.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 3/21/2015</p>
*/

#include <iostream>

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include "mainwindow.h"
#include "brainf.h"
#include "genepool.h"
#include "brainfgene.h"

int testGene()
{
    GenePool pool;
    {
        std::vector<Gene*> genes;
        genes.push_back( new BrainfGene() );
        pool.initialize(genes);
    }

    Gene * g = pool.genes()[0].get();

    for (int i=0; i<100; ++i)
    {
        g->mutate(.5, .5);
        std::cout << g->toString() << std::endl;
    }

    return 0;
}

int testBf()
{
    Brainf<unsigned char> bf;

    // Hello, world!s
    bf.setCode("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.");
    bf.setCode(">++++++++[<+++++++++>-]<.>>+>+>++>[-]+<[>[->+<<++++>]<<]>.+++++++..+++.>>+++++++.<<<[[-]<[-]>]<+++++++++++++++.>>.+++.------.--------.>>+.>++++.");

    // http://esoteric.sange.fi/brainfuck/bf-source/prog/fibonacci.txt
    bf.setCode("+++++++++++"
               ">+>>>>++++++++++++++++++++++++++++++++++++++++++++"
               ">++++++++++++++++++++++++++++++++<<<<<<[>[>>>>>>+>"
               "+<<<<<<<-]>>>>>>>[<<<<<<<+>>>>>>>-]<[>++++++++++[-"
               "<-[>>+>+<<<-]>>>[<<<+>>>-]+<[>[-]<[-]]>[<<[>>>+<<<"
               "-]>>[-]]<<]>>>[>>+>+<<<-]>>>[<<<+>>>-]+<[>[-]<[-]]"
               ">[<<+>>[-]]<<<<<<<]>>>>>[+++++++++++++++++++++++++"
               "+++++++++++++++++++++++.[-]]++++++++++<[->-<]>++++"
               "++++++++++++++++++++++++++++++++++++++++++++.[-]<<"
               "<<<<<<<<<<[>>>+>+<<<<-]>>>>[<<<<+>>>>-]<-[>>.>.<<<"
               "[-]]<<[>>+>+<<<-]>>>[<<<+>>>-]<<[<+>-]>[<+>-]<<<-]");

    // print '0' "++++[>++++[>+++<-]<-]>>."

    bf.run();

    std::cout << "[" << bf.outputStringNum() << "]" << std::endl;
    std::cout << "[" << bf.outputString() << "]" << std::endl;

    return 0;
}


// Small endless evolutionary loop
int breed()
{
    GenePool pool;

    {
        std::vector<Gene*> genes;
        for (int i=0; i<100; ++i)
            genes.push_back( new BrainfGene() );

        pool.initialize(genes);
        pool.evaluate();
    }


    double f = 0.;
    int i=0;
    for (; f<99.99999; ++i)
    {
        pool.nextGeneration();
        pool.evaluate();
        f = pool.getBest()->fitness();

        if (i % 1000 == 0)
        {
            std::cout << "\nGENERATION " << i << std::endl;
            pool.dump(20);
        }
    }

    std::cout << "\nGENERATION " << i << std::endl;
    pool.evaluate();
    pool.dump();

    return 0;
}


int main(//int, char**)
         int argc, char *argv[])
{
    //return testBf();
    //return testGene();
    //return breed();

    QApplication a(argc, argv);
    QFile f(":/appstyle.css");
    if (f.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream s(&f);
        a.setStyleSheet(s.readAll());
    }

    MainWindow w;
    w.show();

    return a.exec();
}


/** @page bf_stuff

    @code

    +++[>
        # create a '0'
        >>[-]<<
        ++++[>++++[>+++<-]<-]>>
        # output 0 to 9
        >[-]++++++++++[<.+>-]
        # print '='
        <+++.
        <<
    <-]


    "Hello World?"
    ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.+++++++++++++++++++++++++++++.+++++++..+++.-------------------------------------------------------------------------------.+++++++++++++++++++++++++++++++++++++++++++++++++++++++.++++++++++++++++++++++++.+++.------.--------.-------------------------------------.


    [convert ascii to decimal, with comma!]
    // ascii ' ' at cell 0
    >++++++++[<++++>-]
    // input next char
    ,[
        // convert to decimal
        >>++++++++++<<[->+>-[>+>>]>[+[-<+>]>+>>]<<<<<<]>>[-]>>>++++++++++<[->-[>+>>]>[+[-
        <+>]>+>>]<<<<<]>[-]>>[>++++++[-<++++++++>]<.<<+>+>[-]]<[<[->-<]++++++[->++++++++
        <]>.[-]]<<++++++[-<++++++++>]<.[-]<<[-<+>]<
        // print space
        <.>
    // next char
    ,]

    @endcode

 **/
