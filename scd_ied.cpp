#include "scd_ied.h"
#include "scd_datatypetemplates.h"
#include "scd.h"

#include <QtCore>
#include <QtXml>


void parseBDa( BDA * pBDa, QString& daFc, QString& pathName, QStringList & listRet)
{
	DataTypeTemplates* ptmp = SCD::instance()->getDataTypeTemplates();
	QString tmpRet = pathName;
	tmpRet.append("$").append(pBDa->name_);
	
	if(pBDa->type_.isEmpty() || pBDa->bType_.toUpper() == "ENUM")
	{
		listRet.push_back(tmpRet +  ":" + daFc);
		
	}
	else
	{
		DAType* pDAType = ptmp->findDAType(pBDa->type_);
		if(pDAType==NULL) return ;
		for(int i = 0; i <pDAType->vtBDA_.count(); ++i)
		{
			parseBDa(pDAType->vtBDA_[i], daFc, tmpRet, listRet);
		}
	}
}
//我改
void parseDa( DA * pDa, QString& pathName,  QString& fc,QStringList & listRet)
{
	if(fc == pDa->fc_ || fc.isEmpty())
	{
	}
	else
	{
		return;
	}
	QString tmpName = pathName;
	tmpName.append("$").append(pDa->name_);
	DataTypeTemplates* ptmp = SCD::instance()->getDataTypeTemplates();	
	if(pDa->type_.isEmpty() || pDa->bType_.toUpper() == "ENUM")
	{
		listRet.push_back((tmpName + ":" + pDa->fc_));
		//qDebug()<<"tmpName"<<tmpName;
	}
	else
	{
		DAType* pDAType = ptmp->findDAType(pDa->type_);
		//qDebug()<<"pDa->type_ ="<<pDa->type_;
		for(int i = 0; i <pDAType->vtBDA_.count(); ++i)
		{
			parseBDa( pDAType->vtBDA_[i],  pDa->fc_, tmpName, listRet);
		}
	}
}

void parseSdo(SDO * pSdo, QString& pathName, QString& fc, QStringList & listRet)
{
	QString tmpName = pathName;
	tmpName.append("$").append(pSdo->name_);
	DataTypeTemplates* ptmp = SCD::instance()->getDataTypeTemplates();	
	DOType * pSdoType = ptmp->findDOType(pSdo->type_);
	QVector<DOType::DoIndex>::ConstIterator cit = pSdoType->vtIndex.begin();

	for(;cit != pSdoType->vtIndex.end(); ++cit)
	{
		if(DOType::DALAB == cit->labType)
		{
			DA * pDa = pSdoType->findDA(cit->key);
			parseDa(pDa, tmpName, fc, listRet);
			
		}
		else if(DOType::SDOLAB == cit->labType)
		{
			stSDO *pSDo = pSdoType->findSDO(cit->key);
			parseSdo(pSDo, tmpName, fc, listRet);
		}
	}
}


void IED::initServices(QDomElement &/*e*/)
{

}

void IED::initAccessPoint(QDomElement &e)
{
   stAccessPoint* ap = new stAccessPoint(this);

   ap->desc_ = e.attribute("desc","");
   ap->name_ = e.attribute("name","");
   ap->router_ = e.attribute("router","");
   ap->clock_ = e.attribute("clock","");

   this->mpAccessPoint_.insert(ap->name_,ap);

   QDomNode n = e.firstChild();
   QDomElement tmp;
   QString name;
   while(!n.isNull())
   {
       tmp = n.toElement();
       name = tmp.tagName();

       if(name == "Server")
       {
           ap->initServer(tmp);
       }


       n = n.nextSibling();
   }


}

void AccessPoint::initServer(QDomElement &e)
{
    this->Server_ = new stServer(this);

    Server_->desc_ = e.attribute("desc","");
    Server_->timeout_ = e.attribute("timeout","");

    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;
    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "Authentication")
        {
            Server_->initAuthentication(tmp);
        }
        else
            if(name == "LDevice")
            {
                Server_->initLDevice(tmp);
            }


        n = n.nextSibling();
    }
}

stLDevice* Server::getLDevice(QString &name)
{
    return this->mpLDevice_.value(name,NULL);
}

map<string,vector<string> >& Server::initDataSetToAddress()
{
     qDebug()<<"Server::initDataSetToAddress(). begin."<<endl;

    QMap<QString,stLDevice*>::iterator it = this->mpLDevice_.begin();

    map<string,vector<string> > tmp;
    while(it != mpLDevice_.end())
    {
        tmp = it.value()->initDataSetToAddress();

        this->mpDataSetToAddress_.insert(tmp.begin(),tmp.end());
        it++;
    }

    qDebug()<<"Server::initDataSetToAddress(). end."<<endl;

    return this->mpDataSetToAddress_;

}
//我改3
map<string,vector<string> >& Server::initDoiToAddress()
{
     qDebug()<<"Server::initDoiToAddress(). begin."<<endl;

    QMap<QString,stLDevice*>::iterator it = this->mpLDevice_.begin();

    map<string,vector<string> > tmp;
    while(it != mpLDevice_.end())
    {
        tmp = it.value()->initDoiToAddress();

        this->mpDoiToAddress_.insert(tmp.begin(),tmp.end());
        it++;
    }

    qDebug()<<"Server::initDoiToAddress(). end."<<endl;

    return this->mpDoiToAddress_;
}

void Server::initAuthentication(QDomElement &/*e*/)
{

}
void Server::initLDevice(QDomElement &e)
{
    stLDevice* ld = new stLDevice(this);

    ld->desc_ = e.attribute("desc","");
    ld->inst_ = e.attribute("inst","");
    ld->ldName_ = e.attribute("ldName","");

    this->mpLDevice_.insert(ld->inst_,ld);

    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;
    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "LN0")
        {
            ld->initLN0(tmp);
        }
        else if(name == "LN")
		{
			ld->initLN(tmp);
		}


        n = n.nextSibling();
    }

}

void LDevice::initLN0(QDomElement &e)
{
   this->LN0_ = new stLN(this);

    LN0_->desc_ = e.attribute("desc","");
    LN0_->lnType_ = e.attribute("lnType","");
    LN0_->lnClass_ = e.attribute("lnClass","");
    LN0_->inst_ = e.attribute("inst","");
    LN0_->prefix_ = e.attribute("prefix","");

    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;
    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "DataSet")
        {
            LN0_->initDataSet(tmp);
        }
        else
            if(name == "ReportControl")
            {
                 LN0_->initReportControl(tmp);
            }
            else
                if(name == "LogControl")
                {
                     LN0_->initLogControl(tmp);
                }
                else
                if(name == "DOI")
                {
                     LN0_->initDOI(tmp);
                }
                else
                if(name == "Inputs")
                    {
                         LN0_->initInputs(tmp);
                    }
                    else
                    if(name == "Log")
                        {
                             LN0_->initLog(tmp);
                        }
                    else
                        if(name == "GSEControl")
                        {
                        }
                    else
                        if(name == "SampledValueControl")
                        {
                    }
                    else
                        if(name == "SettingControl")
                        {

                        }
                        else
                            if(name == "SCLControl")
                            {

                            }


        n = n.nextSibling();
    }
}
void LDevice::initLN(QDomElement &e)
{
    stLN* p = new stLN(this);

     p->desc_ = e.attribute("desc","");
     p->lnType_ = e.attribute("lnType","");
     p->lnClass_ = e.attribute("lnClass","");
     p->inst_ = e.attribute("inst","");
     p->prefix_ = e.attribute("prefix","");


     this->vtLN_.append(p);
     QDomNode n = e.firstChild();
     QDomElement tmp;
     QString name;
     while(!n.isNull())
     {
         tmp = n.toElement();
         name = tmp.tagName();

         if(name == "DataSet")
         {
             p->initDataSet(tmp);
         }
		 /*
		else if(name == "ReportControl")
		{
			//p->initReportControl(tmp);
		}
		else if(name == "LogControl")
		{
			//p->initLogControl(tmp);
		}
		else if(name == "DOI")
		{
			//p->initDOI(tmp);
		}
		else if(name == "Inputs")
		{
			//p->initInputs(tmp);
		}
		else if(name == "Log")
		{
			p->initLog(tmp);
		}
		else if(name == "GSEControl")
		{
		}
		else if(name == "SampledValueControl")
		{
		}
		else if(name == "SettingControl")
		{ 
		}
		else if(name == "SCLControl")
		{ 
		}
		*/
		n = n.nextSibling();
    }	
}


//shuchang 2015.12.16 add dataset elment
map<string,vector<string> >& DataSet::initDataSetToAddress()
{
    QString name =QString("%1%2/%3$%4")
                  .arg(LN_->LDevice_->Server_->AccessPoint_->IED_->name_)
                  .arg(LN_->LDevice_->inst_)
                  .arg(LN_->lnClass_)
                  .arg(this->name_);

    vector<string> vt;

    int c = vtFCDA_.count();
    int i = 0;

    QString n;
    QStringList ns;

    for(i=0; i<c; i++)
    {
		 ns = vtFCDA_[i]->getGlobalNames();

        for(int x = 0; x<ns.count(); x++)
        {
            vt.push_back(ns.at(x).toStdString());
        }
       /* if(!vtFCDA_[i]->daName_.isEmpty())
        {

            n = vtFCDA_[i]->getGlobalName(LN_->LDevice_->Server_->
                                      AccessPoint_->IED_->name_);

            vt.push_back(n.toStdString());
        }
        else
        {
            ns = vtFCDA_[i]->getGlobalNames();

            for(int x = 0; x<ns.count(); x++)
            {
               vt.push_back(ns.at(x).toStdString());
            }
        }*/

    }

    this->mpDataSetToAddress_[name.toStdString()]=vt;

    return this->mpDataSetToAddress_;

}

void LN::initDataSet(QDomElement &e)
{
    stDataSet *ds = new stDataSet(this);

    ds->desc_ = e.attribute("desc","");
    ds->name_ = e.attribute("name","");

    this->mpDataSet_.insert(ds->name_,ds);

    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;

    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "FCDA")
        {
            ds->initFCDA(tmp);
        }
        else
            if(name == "FCCB")
            {
                ds->initFCCB(tmp);
            }

        n = n.nextSibling();
    }
}

void DataSet::initFCCB(QDomElement &e)
{
    stFCCB *fb = new stFCCB();
    fb->cbName_ = e.attribute("cbName","");
    fb->daName_ = e.attribute("daName","");
    fb->fc_ = e.attribute("fc","");
    fb->ldInst_ = e.attribute("ldInst");
    fb->lnClass_ = e.attribute("lnClass");
    fb->mustUnderstand_ = e.attribute("mustUnderstand","");
    fb->prefix_ = e.attribute("prefix","");

    this->vtFCCB_.append(fb);

}

Server* FCDA::getServer()
{
     return dataSet_->LN_->LDevice_->Server_;
}

AccessPoint* FCDA::getAccessPoint()
{
     return dataSet_->LN_->LDevice_->Server_->AccessPoint_;
}

IED* FCDA::getIed()
{
    return dataSet_->LN_->LDevice_->Server_->AccessPoint_->IED_;
}

LDevice* FCDA::getLDevice()
{
    return dataSet_->LN_->LDevice_;
}

void DataSet::initFCDA(QDomElement &e)
{
    stFCDA *fa = new stFCDA(this);
    fa->ldInst_ = e.attribute("ldInst","");
    fa->prefix_ = e.attribute("prefix","");
    fa->lnClass_ = e.attribute("lnClass","");
    fa->lnInst_ = e.attribute("lnInst","");
    fa->doName_ =e.attribute("doName","");
    fa->daName_ = e.attribute("daName","");
    fa->fc_ = e.attribute("fc","");
    fa->ix_ = e.attribute("ix","");

    this->vtFCDA_.append(fa);

}

void stReportControl::initTrgOps(QDomElement &e)
{
    stTrgOps* p = new stTrgOps();

    p->dchg_ = e.attribute("dchg","");
    p->qchg_ = e.attribute("qchg","");
    p->dupd_ = e.attribute("dupd","");
    p->period_ = e.attribute("period","");
    p->gi_ = e.attribute("gi","");
    p->includeAll_ = e.attribute("includeAll","");

    this->pTrgOps_ = p;
}

void stReportControl::initOptFields(QDomElement &e)
{
    stOptFields* p = new stOptFields();
    p->seqNum_ = e.attribute("seqNum","");
    p->timeStamp_ = e.attribute("timeStamp","");
    p->dataSet_ = e.attribute("dataSet","");
    p->reasonCode_ = e.attribute("reasonCode","");
    p->dataRef_ = e.attribute("dataRef","");
    p->entryID_ = e.attribute("entryID","");
    p->configRef_ = e.attribute("configRef","");
    p->bufOvfl_ = e.attribute("bufOvfl","");
    p->segmentation_ = e.attribute("seqmentation","");

    this->pOptFields_ = p;
}

void stRptEnabled::initClientLN(QDomElement &e)
{
    stClientLN* p = new stClientLN();

    p->desc_ = e.attribute("desc","");
    p->iedName_ = e.attribute("iedName","");
    p->ldInst_ = e.attribute("ldInst","");
    p->prefix_ = e.attribute("prefix","");
    p->lnClass_ = e.attribute("lnClass","");
    p->lnInst_ = e.attribute("lnInst","");

    this->vtClientLN_.append(p);
}

void stReportControl::initRptEnabled(QDomElement &e)
{
    stRptEnabled *p = new stRptEnabled();
    p->desc_ = e.attribute("desc","");
    p->max_ = e.attribute("max","");

    this->pRptEnabled_ = p;

    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;

    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "ClientLN")
        {
          p->initClientLN(tmp);
        }

        n = n.nextSibling();
    }



}


stLN* LDevice::getLN(QString prefix,QString lnClass,QString inst)
{
    stLN* p = NULL;

    if(this->LN0_->prefix_ == prefix &&
       this->LN0_->lnClass_ == lnClass&&
       this->LN0_->inst_ == inst)
    {
        p = this->LN0_;

        return p;
    }

    for(int i = 0; i<this->vtLN_.count(); i++)
    {
        if(vtLN_[i]->prefix_ == prefix &&
           vtLN_[i]->inst_ == inst &&
           vtLN_[i]->lnClass_ == lnClass)
        {
            p = vtLN_[i];

            break;
        }
    }
#if 1
    if(p == NULL)
    {
        qDebug()<<"LDevice = "<<this->inst_
                <<" vtLN count = "<<vtLN_.count()<<endl;

        for(int i = 0; i<this->vtLN_.count(); i++)
        {
            qDebug()<<i<<" prefix ="<<vtLN_[i]->prefix_
                    <<" inst = "<<vtLN_[i]->inst_
                    <<" lnClass = "<<vtLN_[i]->lnClass_
                    <<endl;

//            qDebug()<<i<<" prefix ="<<prefix
//                    <<" inst = "<<inst
//                    <<" lnClass = "<<lnClass
//                    <<endl;
        }

    }
#endif
    return p;
}

map<string,vector<string> >& LDevice::initDataSetToAddress()
{
	qDebug()<<"LDevice::initDataSetToAddress(). begin."<<endl;

    int c = this->vtLN_.count();
    int i = 0;

    map<string,vector<string> > tmp;
    for(i = 0; i<c; i++)
    {
        tmp = vtLN_[i]->initDataSetToAddress();

        this->mpDataSetToAddress_.insert(tmp.begin(),tmp.end());
    }


    tmp = LN0_->initDataSetToAddress();

    this->mpDataSetToAddress_.insert(tmp.begin(),tmp.end());

    qDebug()<<"LDevice::initDataSetToAddress(). end."<<endl;

    return this->mpDataSetToAddress_;
}

//我改 4
map<string,vector<string> >& LDevice::initDoiToAddress()
{
    qDebug()<<"LDevice::initDoiToAddress(). begin."<<endl;

    int c = this->vtLN_.count();
    int i = 0;

    map<string,vector<string> > tmp;
    for(i = 0; i<c; i++)
    {
		qDebug()<<vtLN_[i]->desc_<<vtLN_[i]->lnClass_;
		if(vtLN_[i]->mpDOI_.size()>0)
		{
			tmp = vtLN_[i]->initDoiToAddress();
		
			//qDebug()<<"tmp" <<QString::fromStdString(tmp.begin()->first);
			this->mpDoiToAddress_.insert(tmp.begin(),tmp.end());
		}
    }


    tmp = LN0_->initDoiToAddress();
	
    this->mpDoiToAddress_.insert(tmp.begin(),tmp.end());

    return this->mpDoiToAddress_;
}

map<string,vector<string> >& LN::initDataSetToAddress()
{
    // QMap<QString,stDataSet*> mpDataSet_;

    QMap<QString,stDataSet*>::iterator it= mpDataSet_.begin();

    map<string,vector<string> > tmp;

    while(it != mpDataSet_.end())
    {
        tmp = it.value()->initDataSetToAddress();

        this->mpDataSetToAddress_.insert(tmp.begin(),tmp.end());

        it++;
    }

    return this->mpDataSetToAddress_;
}
//我改5
map<string,vector<string> >& LN::initDoiToAddress()
{

    QMap<QString,stDOI*>::iterator it= mpDOI_.begin();

    map<string,vector<string> > tmp;
	
    while(it != mpDOI_.end())
    {

        tmp = it.value()->initDoiToAddress();

        this->mpDoiToAddress_.insert(tmp.begin(),tmp.end());

        it++;
    }

    return this->mpDoiToAddress_;
}


map<string,vector<string> >& IED::initDataSetToAddress()
{
    qDebug()<<"IED::initDataSetToAddress(). begin."<<endl;

    QMap<QString,stAccessPoint*>::iterator it;

    map<string,vector<string> > tmp;

    for(it = mpAccessPoint_.begin(); it!=mpAccessPoint_.end(); it++)
    {
       tmp = it.value()->initDataSetToAddress();

       this->mpDataSetToAddress_.insert(tmp.begin(),tmp.end());

    }

    qDebug()<<"IED::initDataSetToAddress(). end."<<endl;

    return this->mpDataSetToAddress_;

}
//我改doi
map<string,vector<string> >& IED::initDoiToAddress()
{
    qDebug()<<"IED::initDoiToAddress(). begin."<<endl;

    QMap<QString,stAccessPoint*>::iterator it;

	//根据LN 获取
    map<string,vector<string> > tmp;
	
    for(it = mpAccessPoint_.begin(); it!=mpAccessPoint_.end(); it++)
    {
       tmp = it.value()->initDoiToAddress();

       this->mpDoiToAddress_.insert(tmp.begin(),tmp.end());

    }

    qDebug()<<"IED::initDoiToAddress(). end."<<endl;

    return this->mpDoiToAddress_;

}

void stLN::initReportControl(QDomElement &e)
{
    stReportControl *p = new stReportControl();

    p->desc_ = e.attribute("desc","");
    p->name_ = e.attribute("name","");
    p->datSet_ = e.attribute("datSet","");
    p->intgPd_ = e.attribute("intgPd","");
    p->rptID_ = e.attribute("rptID","");
    p->confRev_ = e.attribute("confRev","");
    p->buffered_ = e.attribute("buffered","");
    p->bufTime_ = e.attribute("bufTime","");
    p->indexed_ = e.attribute("indexed","");

    this->mpReportControl_.insert(p->name_,p);

    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;

    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "TrgOps")
        {
           p->initTrgOps(tmp);
        }
        else
            if(name == "OptFields")
            {
                p->initOptFields(tmp);
            }
            else
                if(name == "RptEnabled")
                {
                    p->initRptEnabled(tmp);
                }

        n = n.nextSibling();
    }

}


void stLogControl::initTrgOps(QDomElement &e)
{
    stTrgOps* p = new stTrgOps();

    p->dchg_ = e.attribute("dchg","");
    p->qchg_ = e.attribute("qchg","");
    p->dupd_ = e.attribute("dupd","");
    p->period_ = e.attribute("period","");
    p->gi_ = e.attribute("gi","");
    p->includeAll_ = e.attribute("includeAll","");

    this->pTrgOps_  = p;
}
void stLN::initLogControl(QDomElement &e)
{
    stLogControl* p = new stLogControl();

    p->desc_ = e.attribute("desc","");
    p->name_ = e.attribute("name","");
    p->datSet_ = e.attribute("datSet","");
    p->intgPd_ = e.attribute("intgPd","");
    p->ldInst_ =e.attribute("ldInst","");
    p->prefix_ = e.attribute("prefix","");
    p->lnClass_ = e.attribute("lnClass","");
    p->lnInst_ = e.attribute("lnInst","");
    p->logName_ = e.attribute("logName","");
    p->logEna_ = e.attribute("logEna","");
    p->reasonCode_ = e.attribute("reasonCode","");

    this->mpLogControl_.insert(p->name_,p);


    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;

    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "TrgOps")
        {
           p->initTrgOps(tmp);
        }

        n = n.nextSibling();
    }

}

void stDAI::initVal(QDomElement &e)
{
    stVal* p = new stVal();

    p->sGroup_ = e.attribute("sGroup","");

    p->val_ = e.nodeValue();
}

//我改6
map<string,vector<string> >& stDOI::initDoiToAddress()
{

#define MYFC "myfc"
    QString keyName =QString("%1%2/%3%4%5")
					.arg(LN_->LDevice_->Server_->AccessPoint_->IED_->name_)
					.arg(LN_->LDevice_->inst_)
					.arg(LN_->prefix_)
					.arg(LN_->lnClass_)
					.arg(LN_->inst_);
	qDebug()<<keyName;
	QVector<stDA*> tmpVctDa;

	DataTypeTemplates* ptmp = SCD::instance()->getDataTypeTemplates();

	LNodeType* pLNodeType =  ptmp->findLNodeType(this->LN_->lnType_);
	qDebug()<<this->LN_->lnType_;

   if(pLNodeType == NULL)
   {
       qDebug()<<"stDOI::initDoiToAddress() LNodeType \n";

       goto ERROR;
   }


   DO* pDo = pLNodeType->findDO(this->name_);
   qDebug()<<this->name_;
   if(pDo == NULL )
   {
       qDebug()<<"stDOI::initDoiToAddress() DO \n";
        goto ERROR;
   }



   DOType* pDoType = ptmp->findDOType(pDo->type_);
    qDebug()<<pDo->type_;
   if(pDoType == NULL)
   {
        qDebug()<<"stDOI::initDoiToAddress() DOType \n";
          goto ERROR;
   }

   QVector<DOType::DoIndex>::ConstIterator cit = pDoType->vtIndex.begin();

	for(;cit != pDoType->vtIndex.end(); ++cit)
	{
		QString fc;
		QStringList ret;
		
		if(DOType::DALAB == cit->labType)
		{
			DA * pDa = pDoType->findDA(cit->key);
			QString tmpKeyName = (keyName + "$" + pDa->fc_ + "$" + this->name_);
			QString tmpName = tmpKeyName;
		
			parseDa(pDa, tmpName, fc, ret);
		}
		else if(DOType::SDOLAB == cit->labType)
		{
			QString tmpKeyName = (keyName + "$" + MYFC + "$" + this->name_);
			QString tmpName = tmpKeyName;
			
			stSDO *pSDo = pDoType->findSDO(cit->key);
			parseSdo(pSDo, tmpName, fc, ret);
		}

		for(int x = 0; x<ret.count(); x++)
        {
			QString doFc = ret.at(x).mid(ret.at(x).indexOf(":") + 1);
			QString keyPath = keyName + "$" + doFc + "$" + this->name_;
			QString value = ret.at(x).mid(0, ret.at(x).indexOf(":"));
			if(1 == value.contains(MYFC))
			{
				value.replace(value.indexOf(MYFC), 4, doFc);
			}
			map<string,vector<string> >::iterator cit = mpDoiToAddress_.find(keyPath.toStdString());
			if(cit == mpDoiToAddress_.end())
			{
				vector<string> vt;
				vt.push_back(value.toStdString());
				this->mpDoiToAddress_[keyPath.toStdString()]=vt;
			}
			else
			{
				cit->second.push_back(value.toStdString());
			 
			}
        }
	}

	 ERROR:
		// qDebug()<<"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeerror"<<endl;


   return this->mpDoiToAddress_;

}

void stDOI::initDAI(QDomElement &e)
{
    stDAI* p = new stDAI();
    p->desc_ = e.attribute("desc","");
    p->name_ = e.attribute("name","");
    p->sAddr_ = e.attribute("sAddr","");
    p->valKind_ = e.attribute("valKind","");
    p->ix_ = e.attribute("ix","");

    this->mpDAI_.insert(p->name_,p);

    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;

    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "Val")
        {
            p->initVal(tmp);
        }


        n = n.nextSibling();
    }


}
void stDOI::initSDI(QDomElement &/*e*/)
{

}
void stLN::initDOI(QDomElement &e)
{
    stDOI* p = new stDOI(this);

    p->desc_ = e.attribute("desc","");
    p->name_ = e.attribute("name","");
    p->ix_ = e.attribute("ix","");
    p->accessControl_ = e.attribute("accessControl","");

    this->mpDOI_.insert(p->name_,p);

    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;

    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "SDI")
        {
            p->initSDI(tmp);
        }
        else
            if(name == "DAI")
            {
                p->initDAI(tmp);
            }

        n = n.nextSibling();
    }


}

void stInputs::initExtRef(QDomElement &e)
{
    stExtRef* p = new stExtRef();

    p->desc_ = e.attribute("desc","");

    p->iedName_ = e.attribute("iedName","");
    p->ldInst_ = e.attribute("ldInst","");
    p->prefix_ = e.attribute("prefix","");
    p->lnClass_ = e.attribute("lnClass","");
    p->lnInst_ = e.attribute("lnInst","");
    p->doName_ = e.attribute("doName","");
    p->daName_ = e.attribute("daName","");
    p->intAddr_ = e.attribute("intAddr","");

    this->vtExtRef_.append(p);

}

void stLN::initInputs(QDomElement &e)
{
    stInputs* p = new stInputs();

    p->desc_ = e.attribute("desc","");



    this->pInputs_ = p;

    QDomNode n = e.firstChild();
    QDomElement tmp;
    QString name;

    while(!n.isNull())
    {
        tmp = n.toElement();
        name = tmp.tagName();

        if(name == "ExtRef")
        {
            p->initExtRef(tmp);
        }
        n = n.nextSibling();
    }

}
void stLN::initLog(QDomElement &/*e*/)
{

}


//2015.12.16 shuchang Dataset to elment

QStringList FCDA::getGlobalNames()
{
	QString strTmp;
    QStringList ret;
	QStringList ret2;
	QStringList daList;
	QStringList doList;
	QString name = QString("%1%2/%3%4%5$%6")
                      .arg(getIed()->name_)
                      .arg(this->ldInst_)
                      .arg(this->prefix_)
                      .arg(this->lnClass_)
                      .arg(this->lnInst_)
                      .arg(this->fc_);

	QString elmentName = name;

	QString doName;
	QString daName;

	//shuchang 12.16
	if(this->doName_.length()>0)
	{
		doName = this->doName_;
		doName = doName.replace('.','$');

		elmentName += "$"+doName; 
	}

	ret2.push_back( "=" + elmentName);
	/*
	if(this->daName_.length()>0)
	{
		daName = this->daName_;
		daName = daName.replace('.','$');

		elmentName += "$"+daName; 
	}


	/////////////////////////
	QVector<stDA*> tmpVctDa;

    stLDevice * p = this->getServer()->getLDevice(this->ldInst_);

    if (p == NULL)
    {
        qDebug()<<"FCDA::getGlobalNames() stLDevice \n";
          goto ERROR;
    }



    stLN* pLN = p->getLN(this->prefix_,this->lnClass_,this->lnInst_);

    if(pLN == NULL )
    {
        qDebug()<<"FCDA::getGlobalNames() stLN \n";

        goto ERROR;
    }


    DataTypeTemplates* ptmp = SCD::instance()->getDataTypeTemplates();

   LNodeType* pLNodeType =  ptmp->findLNodeType(pLN->lnType_);

   if(pLNodeType == NULL)
   {
       qDebug()<<"FCDA::getGlobalNames() LNodeType \n";

       goto ERROR;
   }

   doList = this->doName_.split(".");
   if(doList.count() < 1)
   {
	   qDebug()<<"FCDA::getGlobalNames() DO \n";
        goto ERROR;
   }

   DO* pDo = pLNodeType->findDO(doList.at(0));

   if(pDo == NULL )
   {
       qDebug()<<"FCDA::getGlobalNames() DO \n";
        goto ERROR;
   }
   DOType* pDoType = ptmp->findDOType(pDo->type_);

   if(pDoType == NULL)
   {
        qDebug()<<"FCDA::getGlobalNames() DOType \n";
          goto ERROR;
   }
    name.append("$").append(doList.at(0));

   for(int i = 1; i <doList.count(); ++i)
   {
	   name.append("$").append(doList.at(i));
	   stSDO *pSDo = pDoType->findSDO(doList.at(i));
	   //pDo = pLNodeType->findDO(doList.at(i));

	   if(pSDo == NULL )
	   {
		   qDebug()<<"FCDA::getGlobalNames() SDo \n";
			goto ERROR;
	   }

	   pDoType = ptmp->findDOType(pSDo->type_);
	   if(pDoType == NULL)
	   {
			qDebug()<<"FCDA::getGlobalNames() DOType \n";
			  goto ERROR;
	   }
   }

   if(this->daName_.isEmpty())
   {
	   QVector<DOType::DoIndex>::ConstIterator cit = pDoType->vtIndex.begin();

	   for(;cit != pDoType->vtIndex.end(); ++cit)
	   {
		   if(DOType::DALAB == cit->labType)
		   {
				DA * pDa = pDoType->findDA(cit->key);
				parseDa(pDa, name, this->fc_, ret);
		   }
		   else if(DOType::SDOLAB == cit->labType)
		   {
			   stSDO *pSDo = pDoType->findSDO(cit->key);
			   parseSdo(pSDo, name, this->fc_, ret);
		   }
	   }
   }
   else
   {
	   daList = this->daName_.split(".");
	   if(daList.count() < 1)
	   {
		   //qDebug()<<"FCDA::getGlobalNames() DA \n";
			goto ERROR;
	   }
	   DA * pDa = pDoType->findDA(daList.at(0));
	   if(pDa == NULL )
	   {
		   //qDebug()<<"FCDA::getGlobalNames() DA \n";
			goto ERROR;
	   }
	   DAType* pDaType = ptmp->findDAType(pDa->type_);

	   if(pDaType == NULL)
	   {
			//qDebug()<<"FCDA::getGlobalNames() pDaType \n";
			 goto ERROR;
	   }

	   if(1 == daList.count())
	   {
		   parseDa(pDa, name,this->fc_,ret);
	   }
	   else
	   {
		   name.append("$").append(daList.at(0));

			stBDA * pBDa;
			for(int i = 1; i <daList.count(); ++i)
		   {
			   pBDa = pDaType->findBDA(daList.at(i));
			   if(pBDa == NULL )
			   {
				   //qDebug()<<"FCDA::getGlobalNames() BDA \n";
					goto ERROR;
			   }

			   if(i == daList.count() - 1)
			   {
				   break;
			   }

			   name.append("$").append(daList.at(i));

			   pDaType = ptmp->findDAType(pBDa->type_);
			   if(pDaType == NULL )
			   {
					//qDebug()<<"FCDA::getGlobalNames() DaType \n";
					  goto ERROR;
			   }
		   }
			parseBDa(pBDa,  this->fc_, name,ret);
	   }
	    
   }

   for(int x = 0; x<ret.count(); x++)
	{
		strTmp = QString(":%1=%2").arg(elmentName).arg(ret.at(x).mid(0, ret.at(x).indexOf(":")));
		//ret2.push_back(ret.at(x).mid(0, ret.at(x).indexOf(":")));
		ret2.push_back(strTmp);
	}
	*/

 ERROR:
         //qDebug()<<"...........begin........................"<<endl
         //        << "  IED: " << this->getIed()->name_ <<endl
         //        << "  AccessPoint " <<this->getAccessPoint()->name_<<endl
         //        << "  LDevice "<<this->getLDevice()->inst_<<endl
         //        << "  DataSet "<<this->dataSet_->name_<<endl
         //        << " ldInst "<<this->ldInst_<<endl
         //        << " prefix "<<this->prefix_<<endl
         //        <<" lnClass "<<this->lnClass_<<endl
         //        <<" lnInst "<<this->lnInst_<<endl
         //        <<" fc "<<this->fc_<<endl
         //        <<" doName "<< this->doName_<<endl
         //        <<"..............end......................"<<endl;

     return ret2;

}

QString FCDA::getGlobalName(QString n)
{
    QString name = QString("%1%2/%3%4%5$%6$%7$%8")
                   .arg(n)
                   .arg(this->ldInst_)
                   .arg(this->prefix_)
                   .arg(this->lnClass_)
                   .arg(this->lnInst_)
                   .arg(this->fc_)
                   .arg(this->doName_)
                   .arg(this->daName_);

    return name;


}


map<string,vector<string> >& AccessPoint::initDataSetToAddress()
{
    qDebug()<<"AccessPoint::initDataSetToAddress(). begin."<<endl;
    mpDataSetToAddress_ = this->Server_->initDataSetToAddress();

    qDebug()<<"AccessPoint::initDataSetToAddress(). end."<<endl;
    return this->mpDataSetToAddress_;
}
//我改2
map<string,vector<string> >& AccessPoint::initDoiToAddress()
{
    qDebug()<<"AccessPoint::initDoiToAddress(). begin."<<endl;
    mpDoiToAddress_ = this->Server_->initDoiToAddress();

    qDebug()<<"AccessPoint::initDoiToAddress(). end."<<endl;
    return this->mpDoiToAddress_;
}







