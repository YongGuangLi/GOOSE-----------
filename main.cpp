#include <QtCore/QCoreApplication>
#include <QtCore>

#include "scd.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	QString scdFilename;

	//scdFilename = "E:\\工作目录\\大唐程序\\工具\\dtxy_scd\\release\\szlxb.scd";
	//scdFilename = "E:\\scd\\2.scd";
	if(2 != a.argc())
	{
		qDebug() << "please run with the scd file path";
        return 0;
	}
	scdFilename = a.argv()[1];
    SCD* s = SCD::instance();

    qDebug()<<"begin init scd.\n";
	SCD::instance()->init(scdFilename.toStdString());
    qDebug()<<"end init scd.\n";


    qDebug()<<"begin init DataSet to Address.\n";
    SCD::instance()->initDataSetToAddress();
    qDebug()<<"end init DataSet to Address.\n";

 
    map<string,vector<string> >::iterator it = SCD::instance()->getDataSetToAddress().begin();

	QString  filenametxt = scdFilename.mid(0,scdFilename.indexOf("."))+"_dataset_da.txt";
    QFile file(filenametxt);


    if ( file.open(QIODevice::ReadWrite)) {
        QTextStream stream( &file );
 
        while(it != SCD::instance()->getDataSetToAddress().end())
        {
            for(int i = 0; i<it->second.size(); i++)
            {
                stream << QString::fromStdString(it->first) 
                        << QString::fromStdString(it->second[i])<<endl;
            }

            it++;
        }

        file.close();
    }
	qDebug()<<"dataset_da.txt OK"<<endl;
 
    return 0;
}
