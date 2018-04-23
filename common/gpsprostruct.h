#ifndef GPSPROSTRUCT_H
#define GPSPROSTRUCT_H


typedef struct GpsprostructBase
{
    unsigned char grp;                  //���
    unsigned char itm;                  //��Ŀ��
    unsigned char kod;                  //kod
	virtual  int  structtobuf(unsigned char *pASDU){ return 0;}
	virtual  int  buftostrcut(unsigned char *pASDU){ return 0;}
	virtual ~GpsprostructBase(){}
}GPSPROSTRUCT_BASE;

//��λ/������
typedef struct tagGpsprostructDes:public GPSPROSTRUCT_BASE 
{

   char szDes[128];                    //���� 
   virtual  int  structtobuf(unsigned char *pASDU)
   {
           unsigned char *p = pASDU;
		   *p++= grp;
		   *p++= itm;
           *p++= kod;
           *p++= 1;
		   unsigned char len = strlen(szDes);
           *p++= len;
           *p++= 1;
           memcpy(p, szDes, len);
		   p += len;
		   return p - pASDU;
   }
   virtual  int  buftostrcut(unsigned char *pASDU)
   { 
	     unsigned char *p = pASDU;
	     grp = *p++;
		 itm = *p++;
         kod = *p++;
         p++;
		 unsigned char len = *p++;
         p++;
         memcpy(szDes, p, len);
		 p += len;

	    return p - pASDU;
   }
   virtual ~tagGpsprostructDes(){}
 

}GPSPROSTRUCT_DES;

//����
typedef struct  taggpsprostruct_range:public GPSPROSTRUCT_BASE
{
	virtual ~taggpsprostruct_range(){}
    unsigned char datatype;  //��������
	unsigned char  width;    //���
	float  mixval;
	float  maxval;
	float  stepval;
	virtual  int  structtobuf(unsigned char *pASDU)
    {
           unsigned char *p = pASDU;
		   *p++= grp;
		   *p++= itm;
           *p++= kod;
           *p++= datatype;
           *p++= width;
           *p++= 3;
		   memcpy(p, &mixval, width);
           p+= width;
		   memcpy(p, &maxval, width);
           p+= width;
		    memcpy(p, &stepval, width);
           p+= width;
		   return p - pASDU;
   }
	virtual  int  buftostrcut(unsigned char *pASDU)
	{
		   unsigned char *p = pASDU;
		   grp = *p++;
		   itm = *p++;
           kod=  *p++;
           datatype = *p++;
           width = *p++;
           p++;
		   memcpy(&mixval,p, width);
           p+= width;
		   memcpy(&maxval,p,  width);
           p+= width;
		    memcpy(&stepval,p,width);
           p+= width;
		   return p - pASDU;
	
	}
   

}GPSPROSTRUCT_RANGE;

//����
typedef struct  taggpsprostruct_precision:public GPSPROSTRUCT_BASE
{
	virtual ~taggpsprostruct_precision(){}
	unsigned char m;
	unsigned  char n;
    virtual  int  structtobuf(unsigned char *pASDU)
    {
           unsigned char *p = pASDU;
		   *p++= grp;
		   *p++= itm;
           *p++= kod;
           *p++= 3;
           *p++= 1;
           *p++= 2;
		   *p++= m;
		   *p++= n;
		   return p - pASDU;
   }
   virtual  int  buftostrcut(unsigned char *pASDU)
	{
		   unsigned char *p = pASDU;
		   grp = *p++;
		   itm = *p++;
           kod=  *p++;
           p++;
           p++;
           p++;
		   m = *p++;
		   n = *p++;		   
		   return p - pASDU;
	
	} 

}GPSPROSTRUCT_PRECISION;


//һ������壬
typedef struct taggpsprostruct_pt
{
   unsigned short grp;
   unsigned short itm;
   char name[128];
   char ref[128];
   float min_v;
   float max_v;
   float step_v;
   unsigned char m;
   unsigned char n;
   char szunit[24];
   unsigned short info_type;
   unsigned char enabled;

}GPSPROSTRUCT_PT;

//���ڱ���ֵ
typedef struct  taggpsprostruct_value:public GPSPROSTRUCT_BASE
{
	~taggpsprostruct_value(){}
     unsigned char datatype; //��������
	 unsigned char width;    //���ݿ��
	 unsigned char number;   //����
	 char          szValue[64];
     virtual  int  structtobuf(unsigned char *pASDU)
    {
           unsigned char *p = pASDU;
		   *p++= grp;
		   *p++= itm;
           *p++= 1;
           *p++= datatype;
           *p++= width;
           *p++= number;
		   memcpy(p, szValue, width * number);

		   p += width * number;

		   return p - pASDU;
   }
   virtual  int  buftostrcut(unsigned char *pASDU)
   {
   
        return 0;
   }


}GPSPROSTRUCT_VALUE;





#endif