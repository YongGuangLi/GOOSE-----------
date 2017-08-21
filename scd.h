#ifndef SCD_H
#define SCD_H



#include <string>
#include <vector>
#include <map>


class QDomElement;


struct Header;
struct Substation;
struct Communication;
struct IED;
struct DataTypeTemplates;

using namespace std;

class SCD
{
public:


    static SCD* instance();
    static void close_singleton();
    bool init(string &filename);
    void initDataSetToAddress();
	void initDoiToAddress();

    map<string,vector<string> >& getDataSetToAddress();
	map<string,vector<string> >& getDoiToAddress();

    DataTypeTemplates* getDataTypeTemplates();

private:
    Header *header_;
    map<string,Substation*> mpSubstation_;
    Communication *communication_;
    map<string,IED*> mpIED_;
    DataTypeTemplates *dataTypeTemplates_;

    bool bInit_;

    map<string,vector<string> > mpDataSetToDa_;
	map<string,vector<string> > mpDoiToDa_;

    string filename_;

    static SCD *instance_;

private:

    void initHeader(QDomElement& e);
    void initSubstation(QDomElement& e);
    void initCommunication(QDomElement& e);
    void initIED(QDomElement& e);
    void initDataTypeTemplates(QDomElement& e);

    SCD();
    ~SCD();

};

#endif // SCD_H
